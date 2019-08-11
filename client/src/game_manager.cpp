// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "game_manager.hpp"
#include "state/menu_state.hpp"

#include <OgreLogManager.h>
#include <OgreOverlaySystem.h>
#include <OgreRTShaderSystem.h>
#include <OgreRoot.h>
#include <OgreSTBICodec.h>
#include <OgreTextureManager.h>
#include <SDL.h>
#include <hoibase/file/file_access.hpp>

#include <exception>

namespace openhoi {

// Initializes the game manager
GameManager::GameManager() : OgreBites::ApplicationContext(OPENHOI_GAME_NAME) {
  // Create options instance
  options = new Options();

  // Initialize render system and resources
  initApp();

  // Add input listener
  addInputListener(this);

  // Create a generic scene manager
  sceneManager = mRoot->createSceneManager(Ogre::ST_GENERIC);

  // Register our scene with the RTSS
  Ogre::RTShader::ShaderGenerator* shaderGenerator =
      Ogre::RTShader::ShaderGenerator::getSingletonPtr();
  shaderGenerator->addSceneManager(sceneManager);

  // Create camera
  createCamera();

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
  for (std::string& resourceGroup : defaultResourceGroups) {
    Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup(
        resourceGroup);
    Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup(
        resourceGroup);
  }

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

// Gets the OGRE camera
Ogre::Camera* const& GameManager::getCamera() const { return camera; }

// Start the main loop
void GameManager::run() {
  // Start rendering. This will call the main game loop and block here until the
  // game ends
  mRoot->startRendering();
}

// Request game exit
void GameManager::requestExit() { mRoot->queueEndRendering(true); }

// Gets the full path to the provided OGRE plugin
std::string GameManager::getPluginPath(std::string pluginName) {
  filesystem::path pluginDirectory = FileAccess::getOgrePluginDirectory();
  if (!pluginDirectory.empty()) {
    return (pluginDirectory / pluginName).u8string();
  } else {
    return pluginName;
  }
}

// Creates the OGRE root (overrides OGRE Bites)
void GameManager::createRoot() {
  // Create root object of OGRE system
  std::string logFile = (FileAccess::getUserGameConfigDirectory() /
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

  // Load the STBI codec plugin for image processing
  mRoot->loadPlugin(getPluginPath(OGRE_PLUGIN_STBI));
}

// Load and configure the render system
void GameManager::loadRenderSystem() {
  bool directX = false;
#ifdef OPENHOI_OS_WINDOWS
  // Prefer DirectX11 on Windows
  try {
    mRoot->loadPlugin(getPluginPath(OGRE_PLUGIN_RS_D3D11));
    directX = true;
  } catch (const std::exception&) {
    // do nothing
  }

  if (mRoot->getAvailableRenderers().empty()) {
#endif
    // Check if we can use OpenGL3+
    // We also try to detect if we are running inside a VM because VM's OpenGL
    // support is very bad and it will most likely crash with GL3+
    if (!OS::isRunningInVirtualMachine()) {
      try {
        mRoot->loadPlugin(getPluginPath(OGRE_PLUGIN_RS_GL3PLUS));
      } catch (const std::exception&) {
        // do nothing
      }
    }
#ifdef OPENHOI_OS_WINDOWS
  }
#endif

  if (mRoot->getAvailableRenderers().empty()) {
    // Use legacy OpenGL as a fallback
    mRoot->loadPlugin(getPluginPath(OGRE_PLUGIN_RS_GL));
  }

  // Get loaded render system
  Ogre::RenderSystem* renderSystem = mRoot->getAvailableRenderers().front();

  // Configure render system
#ifdef OPENHOI_OS_WINDOWS
  if (directX) {
    // Set driver type to hardware
    renderSystem->setConfigOption("Driver type", "Hardware");

    // Disable NVIDIA PerfHUD
    renderSystem->setConfigOption("Allow NVPerfHUD", "No");

    // Set floating-point mode to double precision
    renderSystem->setConfigOption("Floating-point mode", "Consistent");

    // Set DirectX feature levels
    renderSystem->setConfigOption("Min Requested Feature Levels", "9.1");
    renderSystem->setConfigOption("Max Requested Feature Levels", "11.0");
  }
#endif
  if (!directX) {
    // Set RTT Preferred Mode to FBO
    renderSystem->setConfigOption("RTT Preferred Mode", "FBO");
  }

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
  // Declare all audio resources
  // declareResources(FileAccess::GetAssetRootDirectory() / "audio", "Audio");

  // Declare all texture resources
  declareResources(FileAccess::getAssetRootDirectory() / "graphics", "Texture");
  declareResources(
      FileAccess::getAssetRootDirectory() / "graphics" / "coat_of_arms",
      "Texture", OPENHOI_RSG_COA_TEXTURES);
  declareResources(FileAccess::getAssetRootDirectory() / "graphics" / "flags",
                   "Texture", OPENHOI_RSG_FLAG_TEXTURES);

  // Declare all material resources
  declareResources(FileAccess::getAssetRootDirectory() / "materials",
                   "Material");
}

// Declare resources in the provided directory (non-recusive!) with the provided
// type for the given resource group
void GameManager::declareResources(
    filesystem::path directory, std::string resourceType,
    std::string resourceGroup /* = Ogre::RGN_DEFAULT */) {
  // Add resource location
  Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
      directory.u8string(), "FileSystem", resourceGroup);

  filesystem::directory_iterator endItr;
  for (filesystem::directory_iterator itr(directory); itr != endItr; ++itr) {
    // Check if resource is a file
    if (!filesystem::is_directory(itr->status())) {
      // Declare single resource file
      Ogre::ResourceGroupManager::getSingleton().declareResource(
          itr->path().filename().u8string(), resourceType, resourceGroup);
    }
  }
}

// Load resources (overrides OGRE Bites)
void GameManager::loadResources() {
  // Disable mipmapping
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(0);

  // Initialize and load resource groups
  for (std::string& resourceGroup : defaultResourceGroups) {
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(
        resourceGroup);
    Ogre::ResourceGroupManager::getSingleton().loadResourceGroup(resourceGroup);
  }
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

// Create camera
void GameManager::createCamera() {
  // Create the camera
  camera = sceneManager->createCamera("PlayerCam");

  // Position it at 100 in Z direction
  camera->setPosition(Ogre::Vector3(0, 0, 100));
  // Look -300 back along Z
  camera->lookAt(Ogre::Vector3(0, 0, -300));

  // Set clip distances
  camera->setNearClipDistance(0.1f);
  camera->setFarClipDistance(500000);
  if (mRoot->getRenderSystem()->getCapabilities()->hasCapability(
          Ogre::RSC_INFINITE_FAR_PLANE)) {
    // If possible, set infinite far clip distance
    camera->setFarClipDistance(0);
  }
}

}  // namespace openhoi