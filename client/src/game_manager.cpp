// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "game_manager.hpp"
#include "state/menu_state.hpp"

#include <hoibase/file/file_access.hpp>
#include <OgreLogManager.h>
#include <OgreOverlaySystem.h>
#include <OgreRTShaderSystem.h>
#include <OgreRoot.h>
#include <OgreSTBICodec.h>
#include <OgreTextureManager.h>
#ifdef OPENHOI_OS_WINDOWS
#  include <OgreD3D11Plugin.h>
#else
#  include <OgreGL3PlusPlugin.h>
#endif
#include <SDL.h>

namespace openhoi {

// Initializes the game manager
GameManager::GameManager() : OgreBites::ApplicationContext(OPENHOI_GAME_NAME) {
  // Create options instance
  options = new Options();

  // Initialize render system and resources
  initApp();

  // Add input listener
  addInputListener(this);

  /*
  // Create camera
  CreateCamera();

  // Add frame listener
  root->addFrameListener(this);
  */

  // Create a generic scene manager
  sceneManager = mRoot->createSceneManager(Ogre::ST_GENERIC);

  // Register our scene with the RTSS
  Ogre::RTShader::ShaderGenerator* shaderGenerator =
      Ogre::RTShader::ShaderGenerator::getSingletonPtr();
  shaderGenerator->addSceneManager(sceneManager);

  // Disable mipmapping
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(0);

  // Create state manager and startup with menu state
  stateManager = new StateManager();
  stateManager->Startup(new MenuState());
}

// Destroys the game manager
GameManager::~GameManager() {
  // Shutdown state manager
  if (stateManager) {
    stateManager->RequestStateChange(nullptr);
    delete stateManager;
  }

  // Unload resources
  Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup(
      Ogre::RGN_DEFAULT);
  Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup(
      Ogre::RGN_DEFAULT);

  // Close down the game
  closeApp();

  // Unload options
  if (options) delete options;
}

// Gets the game options
Options* const& GameManager::getOptions() const { return options; }

// Gets the state manager
StateManager* const& GameManager::getStateManager() const {
  return stateManager;
}

// Gets the OGRE scene manager
Ogre::SceneManager* const& GameManager::getSceneManager() const {
  return sceneManager;
}

// Start the main loop
void GameManager::run() {
  // Start rendering. This will call the main game loop and block here until the
  // game ends
  mRoot->startRendering();
}

// Request game exit
void GameManager::requestExit() { mRoot->queueEndRendering(true); }

// Creates the OGRE root (overrides OGRE Bites)
void GameManager::createRoot() {
  // Create root object of OGRE system
  std::string logFile = (FileAccess::GetUserGameConfigDirectory() /
                         filesystem::path("client.log"))
                            .u8string();
  mRoot = OGRE_NEW Ogre::Root("", "", logFile);

#ifdef OGRE_STATIC_LIB
  // Load static plugins
  mStaticPluginLoader.load();
#endif

  // Create the overlay system
  mOverlaySystem = OGRE_NEW Ogre::OverlaySystem();

  // Load the render system
  loadRenderSystem();

  // Load the STBI codec for image processing
  mRoot->installPlugin(OGRE_NEW Ogre::STBIPlugin());
}

// Load and configure the render system
void GameManager::loadRenderSystem() {
  // Load and install render system plugin
  Ogre::Plugin* renderSystemPlugin;
#ifdef OPENHOI_OS_WINDOWS
  renderSystemPlugin = OGRE_NEW Ogre::D3D11Plugin();
#else
  renderSystemPlugin = OGRE_NEW Ogre::OgreGL3PlusPlugin();
#endif
  mRoot->installPlugin(renderSystemPlugin);

  // http://wiki.ogre3d.org/SafelyLoadingRenderSystems

  // Get loaded render system
  Ogre::RenderSystem* renderSystem = mRoot->getAvailableRenderers().front();

#ifdef OPENHOI_OS_WINDOWS
  // Set driver type to hardware
  renderSystem->setConfigOption("Driver type", "Hardware");

  // Disable NVIDIA PerfHUD
  renderSystem->setConfigOption("Allow NVPerfHUD", "No");

  // Set floating-point mode to double precision
  renderSystem->setConfigOption("Floating-point mode", "Consistent");

  // Set DirectX feature levels
  renderSystem->setConfigOption("Min Requested Feature Levels", "9.1");
  renderSystem->setConfigOption("Max Requested Feature Levels", "11.0");
#else
  // Set RTT Preferred Mode to FBO
  renderSystem->setConfigOption("RTT Preferred Mode", "FBO");
#endif

  // Set video mode
  renderSystem->setConfigOption("Video Mode", options->GetVideoMode());

  // Set Full Screen Anti-Aliasing (FSAA)
  renderSystem->setConfigOption(
      "FSAA", std::to_string(options->GetFullScreenAntiAliasing()));

  if (!options->GetRenderingDevice().empty()) {
    // Set Rendering device
    renderSystem->setConfigOption("Rendering Device",
                                  options->GetRenderingDevice());
  }

  // Set window mode
  renderSystem->setConfigOption(
      "Full Screen",
      options->GetWindowMode() == WindowMode::FULLSCREEN ? "Yes" : "No");

  // Set VSync (turn always on in windowed screen mode because disabling VSync
  // can cause timing issues at lower frame rates here)
  renderSystem->setConfigOption(
      "VSync", (options->IsVerticalSync() ||
                !options->GetWindowMode() == WindowMode::FULLSCREEN)
                   ? "Yes"
                   : "No");

  // Disable sRGB Gamma conversion
  renderSystem->setConfigOption("sRGB Gamma Conversion", "No");

  // Print out render system details to log
#ifdef _DEBUG
  Ogre::LogManager::getSingletonPtr()->logMessage(
      "*** Render system config options ***");
  const Ogre::ConfigOptionMap& configOptions = renderSystem->getConfigOptions();
  for (auto opt : configOptions) {
    Ogre::LogManager::getSingletonPtr()->logMessage(" [*] " + opt.first);
    for (Ogre::StringVector::iterator vals = opt.second.possibleValues.begin();
         vals != opt.second.possibleValues.end(); vals++)
      Ogre::LogManager::getSingletonPtr()->logMessage("   -> " + *vals);
  }
#endif

  // Set active render system
  mRoot->setRenderSystem(renderSystem);
}

// Create a new render window  (overrides OGRE Bites)
OgreBites::NativeWindowPair GameManager::createWindow(
    const Ogre::String& name, uint32_t w /* = 0*/, uint32_t h /* = 0*/,
    Ogre::NameValuePairList miscParams /* = Ogre::NameValuePairList() */) {
  OgreBites::NativeWindowPair nwp =
      OgreBites::ApplicationContext::createWindow(name, w, h, miscParams);

  // Adjust window style
  if (options->GetWindowMode() != WindowMode::FULLSCREEN) {
    OgreBites::NativeWindowType* windowHnd = nwp.native;
    SDL_SetWindowResizable(windowHnd, SDL_FALSE);
    if (options->GetWindowMode() == WindowMode::BORDERLESS)
      SDL_SetWindowBordered(windowHnd, SDL_FALSE);
  }

  return nwp;
}

// Locate resources (overrides OGRE Bites)
void GameManager::locateResources() {
  // Loop through whole audio directory and declare every single resource in it
  // declareResources(FileAccess::GetAssetRootDirectory() / "audio", "Texture");

  // Loop through whole graphics directory and declare every single resource in
  // it
  declareResources(FileAccess::GetAssetRootDirectory() / "graphics", "Texture");

  // Loop through whole material directory and declare every single resource in
  // it
  declareResources(FileAccess::GetAssetRootDirectory() / "materials",
                   "Material");
}

// Recursively declare resources with the provided type
void GameManager::declareResources(filesystem::path directory,
                                   std::string resourceType) {
  // Add resource location
  Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
      directory.u8string(), "FileSystem", Ogre::RGN_DEFAULT);

  filesystem::directory_iterator endItr;
  for (filesystem::directory_iterator itr(directory); itr != endItr; ++itr) {
    // Check if resource is directory or file
    filesystem::path path = itr->path();
    if (is_directory(itr->status())) {
      // Check for single resources in this directory
      declareResources(path, resourceType);
    } else {
      // Declare single resource file
      Ogre::ResourceGroupManager::getSingleton().declareResource(
          path.filename().u8string(), resourceType, Ogre::RGN_DEFAULT);
    }
  }
}

// Load resources (overrides OGRE Bites)
void GameManager::loadResources() {
  Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(
      Ogre::RGN_DEFAULT);
  Ogre::ResourceGroupManager::getSingleton().loadResourceGroup(
      Ogre::RGN_DEFAULT);
}

// Frame rendering queued event (overrides OGRE Bites)
bool GameManager::frameRenderingQueued(const Ogre::FrameEvent& evt) {
  bool continueRendering =
      OgreBites::ApplicationContext::frameRenderingQueued(evt);
  if (continueRendering) {
    // Update state
    stateManager->UpdateState();
  }
  return continueRendering;
}

}  // namespace openhoi