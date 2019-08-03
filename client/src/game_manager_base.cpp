// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "game_manager.hpp"

#include <OgreLogManager.h>
#include <OgreOverlaySystem.h>
#include <OgreRTShaderSystem.h>
#include <OgreRoot.h>
#include <OgreTextureManager.h>
#include <hoibase/file/file_access.hpp>
#ifdef OPENHOI_OS_WINDOWS
#  include <OgreD3D11RenderSystem.h>
#else
#  include <OgreGL3PlusRenderSystem.h>
#endif
#include <SDL.h>

namespace openhoi {

// Initializes the game manager base
GameManagerBase::GameManagerBase(std::shared_ptr<Options> options)
    : OgreBites::ApplicationContext(OPENHOI_GAME_NAME) {
  // Set options instance
  this->options = options;

  // Initialize render system and resources
  initApp();

  // Add input listener
  addInputListener(this);

  /*
  // Create render window
  CreateRenderWindow();

  // Create camera
  CreateCamera();

  // register for input events
  addInputListener(this);

  // Register as a Window listener
  Ogre::WindowEventUtilities::addWindowEventListener(this->window.get(), this);

  // Add frame listener
  root->addFrameListener(this);
  */
}

// Destroys the game manager base
GameManagerBase::~GameManagerBase() {
  // Unload menu resources
  Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup(
      OPENHOI_GENERAL_RESOURCE_GROUP);
  Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup(
      OPENHOI_GENERAL_RESOURCE_GROUP);
}

// Creates the OGRE root (overrides OGRE Bites)
void GameManagerBase::createRoot() {
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

  // Create the render system
  CreateRenderSystem();
}

// Create the render system
void GameManagerBase::CreateRenderSystem() {
#ifdef OPENHOI_OS_WINDOWS
  // Create Direct3D 11 render system
  renderSystem.reset(OGRE_NEW Ogre::D3D11RenderSystem());

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
  // Create OpenGL 3 + render system
  renderSystem.reset(OGRE_NEW Ogre::GL3PlusRenderSystem());

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

  // Add render system to OGRE root and set it as the default render system
  mRoot->addRenderSystem(renderSystem.get());
  mRoot->setRenderSystem(renderSystem.get());

  // Print out render system details to log
  Ogre::LogManager::getSingletonPtr()->logMessage(
      "*** Render system config options ***");
  const Ogre::ConfigOptionMap& configOptions = renderSystem->getConfigOptions();
  for (auto opt : configOptions) {
    Ogre::LogManager::getSingletonPtr()->logMessage(" [*] " + opt.first);
    for (Ogre::StringVector::iterator vals = opt.second.possibleValues.begin();
         vals != opt.second.possibleValues.end(); vals++)
      Ogre::LogManager::getSingletonPtr()->logMessage("   -> " + *vals);
  }
}

// Create a new render window  (overrides OGRE Bites)
OgreBites::NativeWindowPair GameManagerBase::createWindow(
    const Ogre::String& name, uint32_t w /* = 0*/, uint32_t h /* = 0*/,
    Ogre::NameValuePairList miscParams /* = Ogre::NameValuePairList() */) {
  OgreBites::NativeWindowPair nwp =
      OgreBites::ApplicationContext::createWindow(name, w, h, miscParams);

  // Adjust window style
  if (options->GetWindowMode() != WindowMode::FULLSCREEN) {
    OgreBites::NativeWindowType* windowHnd = nwp.native;
    SDL_SetWindowResizable(windowHnd, SDL_FALSE);
    if (options->GetWindowMode() == WindowMode::BORDERLESS) {
      SDL_SetWindowBordered(windowHnd, SDL_FALSE);
    }
  }

  return nwp;
}

// Locate resources (overrides OGRE Bites)
void GameManagerBase::locateResources() {
  Ogre::ResourceGroupManager::getSingleton().createResourceGroup(
      OPENHOI_GENERAL_RESOURCE_GROUP);
  Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
      FileAccess::GetAssetRootDirectory().u8string(), "FileSystem",
      OPENHOI_GENERAL_RESOURCE_GROUP);
}

// Load resources (overrides OGRE Bites)
void GameManagerBase::loadResources() {
  Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(
      OPENHOI_GENERAL_RESOURCE_GROUP);
}

/*
// Create camera
void GameManager::CreateCamera() {
  camera.reset(sceneManager->createCamera("Player"));

  // Position it at 100 in Z direction
  camera->setPosition(Ogre::Vector3(0, 0, 100));
  // Look -300 back along Z
  camera->lookAt(Ogre::Vector3(0, 0, -300));

  // Set clip distances
  camera->setNearClipDistance(0.1f);
  camera->setFarClipDistance(500000);
  if (renderSystem->getCapabilities()->hasCapability(
          Ogre::RSC_INFINITE_FAR_PLANE)) {
    // If possible, set infinite far clip distance
    camera->setFarClipDistance(0);
  }

  // Set auto aspect ratio
  camera->setAutoAspectRatio(true);
}

// Create render window
void GameManager::CreateRenderWindow() {
  window.reset(root->initialise(true, OPENHOI_GAME_NAME));

#ifdef OPENHOI_OS_WINDOWS
  // Adjust window style
  if (options->GetWindowMode() != WindowMode::FULLSCREEN) {
    OPENHOI_WINDOW_HANDLE_TYPE windowHnd = 0;
    window->getCustomAttribute("WINDOW", &windowHnd);
    LONG_PTR lStyle = GetWindowLongPtr(windowHnd, GWL_STYLE);
    lStyle &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MAXIMIZE);
    if (options->GetWindowMode() == WindowMode::BORDERLESS) {
      lStyle &= ~(WS_CAPTION | WS_SYSMENU);
    }
    SetWindowLongPtr(windowHnd, GWL_STYLE, lStyle);
  }
#endif
}

// Frame rendering queue event
bool GameManager::frameRenderingQueued(const Ogre::FrameEvent& evt) {
  // Check if window is closed
  if (window->isClosed()) return false;

  // Check if user wants to exit from the game
  if (root->endRenderingQueued()) return false;

  // Capture devices (keyboard & mouse)
  // TODO

  // Update current state
  // stateManager->updateState(evt);

  // TODO: ImGui?

  return true;
}

// Window resized event
void GameManager::windowResized(Ogre::RenderWindow* rw) {
  // Get new window metrics
  unsigned int width, height, depth;
  int left, top;
  rw->getMetrics(width, height, depth, left, top);

  // Get mouse state and set new bounds
  // TODO: Update mound coords
}

// Window closing event
bool GameManager::windowClosing(Ogre::RenderWindow* rw) {
  if (rw == window.get()) RequestExit();

  return true;
}

// Window closed event
void GameManager::windowClosed(Ogre::RenderWindow* rw) {
  if (rw == window.get()) {
    // TODO: Destroy input
  }
}
*/

}  // namespace openhoi
