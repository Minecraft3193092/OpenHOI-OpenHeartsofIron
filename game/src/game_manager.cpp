// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "game_manager.hpp"
#include "gui/gui_manager.hpp"
#include "state/menu_state.hpp"

#include <hoibase/file/file_access.hpp>

#include <Ogre.h>
#include <OgreLogManager.h>
#include <OgreOverlaySystem.h>
#include <OgreRTShaderSystem.h>
#include <OgreRoot.h>
#include <OgreSTBICodec.h>
#include <OgreTextureManager.h>
#include <SDL.h>
#include <SDL_syswm.h>
#include <boost/format.hpp>
#ifdef OPENHOI_OS_WINDOWS
#  include <OgreD3D11RenderSystem.h>
#  include <expandedresources.h>
#  include <shellscalingapi.h>
#endif

#include <exception>

#ifdef OPENHOI_OS_WINDOWS
// Direct3D11 causes blurry font textures and thus we disable support for it at
// the moment
//#define ENABLE_DIRECT3D11
#endif

namespace openhoi {

// Initializes the game manager
GameManager::GameManager() {
  // Create options instance and load options from file
  options = new Options();

  // Create root object of OGRE system
  std::string logFile = (FileAccess::getUserGameConfigDirectory() /
                         filesystem::path(OPENHOI_GAME_NAME ".log"))
                            .u8string();
  root = OGRE_NEW Ogre::Root("", "", logFile);

  // Create the overlay system
  overlaySystem = OGRE_NEW Ogre::OverlaySystem();

  // Load the render system
  loadRenderSystem();

  // Load the STBI codec plugin for image processing
  root->loadPlugin(getPluginPath(OGRE_PLUGIN_STBI));

  // Load the ParticleFX plugin
  root->loadPlugin(getPluginPath(OGRE_PLUGIN_PARTICLEFX));

  // Initialize root
  root->initialise(false);

  // Create window
  createWindow();

  // Initialize audio
  initializeAudio();

  // Locate resources
  locateResources();

  // Initialize real-time shader system
  if (Ogre::RTShader::ShaderGenerator::initialize()) {
    // Get shader generator
    shaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
    // Add ourself as a material manager listener
    Ogre::MaterialManager::getSingleton().addListener(this);
  }

  // Load resources
  loadResources();

  // Add ourself as an frame listener
  root->addFrameListener(this);

  // Create GUI manager instance
  guiManager = new GuiManager();

  // Create a generic scene manager
  sceneManager = root->createSceneManager();
  sceneManager->addRenderQueueListener(overlaySystem);
  sceneManager->addRenderQueueListener(guiManager);

  // Register our scene with the RTSS
  Ogre::RTShader::ShaderGenerator::getSingleton().addSceneManager(sceneManager);

  // Initialize GUI manager
  guiManager->initialize(sceneManager, root->getRenderSystem(), window.sdl);

  // Create camera
  createCamera();

  // Save options to file
  options->saveToFile();

  // Create state manager and startup with menu state
  stateManager = new StateManager();
  stateManager->startup(new MenuState());
}

// Destroys the game manager
GameManager::~GameManager() {
  // Shutdown state manager
  if (stateManager) {
    stateManager->requestStateChange(nullptr);
    delete stateManager;
  }

  // Destroy GUI manager
  if (guiManager) delete guiManager;

  // Unregister the material manager listener
  Ogre::MaterialManager::getSingleton().setActiveScheme(
      Ogre::MaterialManager::DEFAULT_SCHEME_NAME);
  Ogre::MaterialManager::getSingleton().removeListener(this);
  Ogre::RTShader::ShaderGenerator::destroy();

  // Destroy window
  destroyWindow();

  // Destroy audio manager
  if (audioManager) delete audioManager;

  // Destroy overlay system
  if (overlaySystem) delete overlaySystem;

  // Destroy options
  if (options) delete options;
}

// Gets the game options
Options* const& GameManager::getOptions() const { return options; }

// Gets the state manager
StateManager* const& GameManager::getStateManager() const {
  return stateManager;
}

// Gets the GUI manager
GuiManager* const& GameManager::getGuiManager() const { return guiManager; }

// Gets the OGRE root
Ogre::Root* const& GameManager::getRoot() const { return root; }

// Gets the OGRE scene manager
Ogre::SceneManager* const& GameManager::getSceneManager() const {
  return sceneManager;
}

// Gets the OGRE camera
Ogre::Camera* const& GameManager::getCamera() const { return camera; }

// Get the OGRE render window
Ogre::RenderWindow* const& GameManager::getRenderWindow() const {
  return window.ogre;
}

// Start the main loop
void GameManager::run() {
#ifdef OPENHOI_OS_WINDOWS
  // Gets the expected number of exclusive CPU sets that are available to the
  // app when in Windows Game Mode
  ULONG exclusiveCpuCount;
  HRESULT result = GetExpandedResourceExclusiveCpuCount(&exclusiveCpuCount);
  if (result == S_OK) {
    Ogre::LogManager::getSingletonPtr()->logMessage(
        (boost::format(
             "Windows Game Mode provides usa total of %d exclusive CPU cores") %
         exclusiveCpuCount)
            .str());
  } else {
    Ogre::LogManager::getSingletonPtr()->logMessage(
        "Unable to get number of exclusive CPU cores provided by Windows Game "
        "Mode",
        Ogre::LogMessageLevel::LML_WARNING);
  }
#endif

  // Start rendering. This will call the main game loop and block here until the
  // game ends
  root->startRendering();
}

// Request game exit
void GameManager::requestExit() { root->queueEndRendering(true); }

// Gets the full path to the provided OGRE plugin
std::string GameManager::getPluginPath(std::string pluginName) {
  filesystem::path pluginDirectory = FileAccess::getOgrePluginDirectory();
  if (!pluginDirectory.empty()) {
    return (pluginDirectory / pluginName).u8string();
  } else {
    return pluginName;
  }
}

// Initialize audio
void GameManager::initializeAudio() {
  // Create audio manager
  audioManager = new AudioManager();

  // Try to set pre-defined audio device
  if (!options->getAudioDevice().empty()) {
    for (auto const& audioDevice : audioManager->getPossibleDevices()) {
      if (audioDevice->getFriendlyName() == options->getAudioDevice()) {
        audioManager->setDevice(audioDevice);
        break;
      }
    }
  }

  // Update last device in options
  options->setAudioDevice(audioManager->getDevice()->getFriendlyName());
}

// Load and configure the render system
void GameManager::loadRenderSystem() {
#if defined(OPENHOI_OS_WINDOWS) && defined(ENABLE_DIRECT3D11)
  // Prefer DirectX11 on Windows
  try {
    root->loadPlugin(getPluginPath(OGRE_PLUGIN_RS_D3D11));
  } catch (const std::exception&) {
    // do nothing
  }

  if (root->getAvailableRenderers().empty()) {
#endif
    // Check if we can use OpenGL3+
    // We also try to detect if we are running inside a VM because VM's OpenGL
    // support is very bad and it will most likely crash with GL3+
    if (!OS::isRunningInVirtualMachine()) {
      try {
        root->loadPlugin(getPluginPath(OGRE_PLUGIN_RS_GL3PLUS));
      } catch (const std::exception&) {
        // do nothing
      }
    }
#if defined(OPENHOI_OS_WINDOWS) && defined(ENABLE_DIRECT3D11)
  }
#endif

  if (root->getAvailableRenderers().empty()) {
    // Use legacy OpenGL as a fallback
    root->loadPlugin(getPluginPath(OGRE_PLUGIN_RS_GL));
  }

  // Get loaded render system
  Ogre::RenderSystem* renderSystem = root->getAvailableRenderers().front();

  // Configure render system
  bool d3d11 = false;
#if defined(OPENHOI_OS_WINDOWS) && defined(ENABLE_DIRECT3D11)
  if (Ogre::D3D11RenderSystem* d3d11rs =
          dynamic_cast<Ogre::D3D11RenderSystem*>(renderSystem)) {
    d3d11 = true;

    // Set driver type to hardware
    renderSystem->setConfigOption("Driver type", "Hardware");

    // Disable NVIDIA PerfHUD
    renderSystem->setConfigOption("Allow NVPerfHUD", "No");

    // Set floating-point mode to fastest
    renderSystem->setConfigOption("Floating-point mode", "Fastest");

    // Set DirectX feature levels
    renderSystem->setConfigOption("Min Requested Feature Levels", "9.1");
    renderSystem->setConfigOption("Max Requested Feature Levels", "11.0");
  }
#endif
  if (!d3d11) {
    // Set RTT Preferred Mode to FBO
    renderSystem->setConfigOption("RTT Preferred Mode", "FBO");
  }

  // Set best possible video mode
  setBestPossibleVideoMode(renderSystem);

  // Set Full Screen Anti-Aliasing (FSAA)
  renderSystem->setConfigOption(
      "FSAA", std::to_string(options->getFullScreenAntiAliasing()));

  // Set VSync (turn always on in windowed screen mode because disabling VSync
  // can cause timing issues at lower frame rates here)
  renderSystem->setConfigOption(
      "VSync", (options->isVerticalSync() ||
                options->getWindowMode() != WindowMode::FULLSCREEN)
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
  root->setRenderSystem(renderSystem);
}

// Set best possible video mode
void GameManager::setBestPossibleVideoMode(Ogre::RenderSystem* renderSystem) {
  // Get all possible video modes for the current render system
  const Ogre::ConfigOption& configValue =
      renderSystem->getConfigOptions().at("Video Mode");
  for (auto it = configValue.possibleValues.begin();
       it != configValue.possibleValues.end(); ++it) {
    std::string possibleVideoMode = *it;
    if (std::next(it) == configValue.possibleValues.end() ||
        (!options->getVideoMode().empty() &&
         (possibleVideoMode == options->getVideoMode() ||
          possibleVideoMode.find(options->getVideoMode() + " ", 0) ==
              0 /* D3D11 adds color depth to video mode */))) {
      // Set video mode if it is the latest one or if it matches the one stored
      // in the config
      options->setVideoMode(possibleVideoMode);
      break;
    }
  }

  // Set video mode
  renderSystem->setConfigOption("Video Mode", options->getVideoMode());

#ifdef OPENHOI_OS_WINDOWS
  if (options->getWindowMode() != WindowMode::FULLSCREEN) {
    // Enable DPI awareness on Windows. Otherwise, openhoi's window size be
    // increased by Windows' desktop scaling factor (e.g. 125%)
    Ogre::LogManager::getSingletonPtr()->logMessage("Enabling DPI awareness");
    HRESULT result = SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
    if (result != S_OK) {
      Ogre::LogManager::getSingletonPtr()->logMessage(
          "Unable to DPI awareness. Checking for monitor scale factor",
          Ogre::LogMessageLevel::LML_WARNING);

      // Something did not work. But this may not be an issue if the scale
      // factor is 100%. We we have to check that first
      HMONITOR monitor =
          MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
      DEVICE_SCALE_FACTOR scaleFactor;
      result = GetScaleFactorForMonitor(monitor, &scaleFactor);
      bool switchToFullscreen = false;
      if (result == S_OK && scaleFactor != DEVICE_SCALE_FACTOR_INVALID) {
        // Check scale factor. If it is 100%, we can just continue. If not, we
        // maybe have to select a lower resolution..
        Ogre::LogManager::getSingletonPtr()->logMessage(
            (boost::format("The primary monitor's scale factor is at %d%%") %
             scaleFactor)
                .str());
        if (scaleFactor != SCALE_100_PERCENT) {
          // Scale factor is not 100%. Thus, we have to switch to full screen
          switchToFullscreen = true;
        }
      } else {
        Ogre::LogManager::getSingletonPtr()->logMessage(
            "Unable to get scale factor of primary monitor",
            Ogre::LogMessageLevel::LML_WARNING);
        switchToFullscreen = true;
      }
      if (switchToFullscreen) {
        Ogre::LogManager::getSingletonPtr()->logMessage(
            "Switching to fullscreen");
        options->setWindowMode(WindowMode::FULLSCREEN);
      }
    }
  }
#endif

  // Set fullscreen flag
  renderSystem->setConfigOption(
      "Full Screen",
      options->getWindowMode() == WindowMode::FULLSCREEN ? "Yes" : "No");
}

// Create a new render window
void GameManager::createWindow() {
  window = {nullptr, nullptr};
  Ogre::NameValuePairList miscParams = Ogre::NameValuePairList();

  if (!SDL_WasInit(SDL_INIT_VIDEO)) SDL_InitSubSystem(SDL_INIT_VIDEO);

  auto windowDesc = root->getRenderSystem()->getRenderWindowDescription();
  miscParams.insert(windowDesc.miscParams.begin(), windowDesc.miscParams.end());
  windowDesc.miscParams = miscParams;
  windowDesc.name = OPENHOI_GAME_NAME;

  // Create SDL window
  Uint32 flags = SDL_WINDOW_ALLOW_HIGHDPI;
  if (options->getWindowMode() == WindowMode::BORDERLESS)
    flags |= SDL_WINDOW_BORDERLESS;
  else if (options->getWindowMode() == WindowMode::FULLSCREEN)
    flags |= SDL_WINDOW_FULLSCREEN;
  window.sdl = SDL_CreateWindow(windowDesc.name.c_str(), SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, windowDesc.width,
                                windowDesc.height, flags);

  // Set window manager information
  SDL_SysWMinfo wmInfo;
  SDL_VERSION(&wmInfo.version);
  SDL_GetWindowWMInfo(window.sdl, &wmInfo);
#ifdef OPENHOI_OS_WINDOWS
  windowDesc.miscParams["externalWindowHandle"] =
      Ogre::StringConverter::toString(size_t(wmInfo.info.win.window));
#elif defined(OPENHOI_OS_LINUX) || defined(OPENHOI_OS_BSD)
  windowDesc.miscParams["parentWindowHandle"] =
      Ogre::StringConverter::toString(size_t(wmInfo.info.x11.window));
#elif defined(OPENHOI_OS_MACOS)
  windowDesc.miscParams["externalWindowHandle"] =
      Ogre::StringConverter::toString(size_t(wmInfo.info.cocoa.window));
#endif

  window.ogre = root->createRenderWindow(windowDesc);
}

// Destroy the render window
void GameManager::destroyWindow() {
  if (window.ogre) root->destroyRenderTarget(window.ogre);

  if (window.sdl) SDL_DestroyWindow(window.sdl);

  SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

// Locate resources
void GameManager::locateResources() {
  filesystem::path assetRoot = FileAccess::getAssetRootDirectory();

  // Declare all font resources
  Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
      (assetRoot / "fonts").u8string(), "FileSystem", Ogre::RGN_DEFAULT);

  // Declare all texture resources
  Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
      (assetRoot / "graphics").u8string(), "FileSystem", Ogre::RGN_DEFAULT);
  Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
      (assetRoot / "graphics" / "coat_of_arms").u8string(), "FileSystem",
      OPENHOI_RSG_COA_TEXTURES);
  Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
      (assetRoot / "graphics" / "flags").u8string(), "FileSystem",
      OPENHOI_RSG_FLAG_TEXTURES);

  // Declare all material resources
  Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
      (assetRoot / "materials").u8string(), "FileSystem", Ogre::RGN_DEFAULT);
  if (Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("glsl")) {
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
        (assetRoot / "materials" / "glsl").u8string(), "FileSystem",
        Ogre::RGN_DEFAULT);
  } else if (Ogre::GpuProgramManager::getSingleton().isSyntaxSupported(
                 "hlsl")) {
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
        (assetRoot / "materials" / "hlsl").u8string(), "FileSystem",
        Ogre::RGN_DEFAULT);
  }

  // Declare all particle resources
  Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
      (assetRoot / "particles").u8string(), "FileSystem", Ogre::RGN_DEFAULT);

  // Declare all mesh resources
  Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
      (assetRoot / "meshes").u8string(), "FileSystem", Ogre::RGN_DEFAULT);
}

// Load resources
void GameManager::loadResources() {
  // Disable mipmapping
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(0);

  // Initialize and load resource groups
  std::array<std::string, 3> defaultResourceGroups = {
      Ogre::RGN_DEFAULT, OPENHOI_RSG_COA_TEXTURES, OPENHOI_RSG_FLAG_TEXTURES};
  for (const auto& resourceGroup : defaultResourceGroups) {
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(
        resourceGroup);
    Ogre::ResourceGroupManager::getSingleton().loadResourceGroup(resourceGroup);
  }

  // Load background audio and other audio effects
  filesystem::path audioDirectory =
      FileAccess::getAssetRootDirectory() / "audio";
  audioManager->loadBackgroundMusicAsync(audioDirectory / "background");
  audioManager->loadEffects(audioDirectory);
}

// Poll for events
void GameManager::pollEvents() {
  if (!window.sdl) {
    // SDL events not initialized
    return;
  }

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    // Handle event in GUI
    guiManager->handleEvent(event);

    // Handle event for the rest of the game
    switch (event.type) {
      case SDL_KEYDOWN:
        keyDown(event);
        break;
      case SDL_KEYUP:
        keyUp(event);
        break;
      case SDL_MOUSEBUTTONDOWN:
        mouseDown(event);
        break;
      case SDL_MOUSEBUTTONUP:
        mouseUp(event);
        break;
      case SDL_MOUSEWHEEL:
        mouseWheelRolled(event);
        break;
      case SDL_MOUSEMOTION:
        mouseMoved(event);
        break;
      case SDL_FINGERDOWN:
        fingerDown(event);
        break;
      case SDL_FINGERUP:
        fingerUp(event);
        break;
      case SDL_FINGERMOTION:
        fingerMoved(event);
        break;
      case SDL_WINDOWEVENT:
        if (event.window.event != SDL_WINDOWEVENT_RESIZED) continue;
        if (event.window.windowID != SDL_GetWindowID(window.sdl)) continue;
        window.ogre->windowMovedOrResized();
        break;
      case SDL_QUIT:
        root->queueEndRendering();
        break;
      default:
        break;
    }
  }

#ifdef OPENHOI_OS_MACOS
  // Hacky workaround for black window on macOS
  for (const auto& win : windows) {
    SDL_SetWindowSize(win.sdl, win.ogre->getWidth(), win.ogre->getHeight());
    win.ogre->windowMovedOrResized();
  }
#endif
}

// Key down event
void GameManager::keyDown(const SDL_Event evt) {}

// Key up event
void GameManager::keyUp(const SDL_Event evt) {
  if (evt.key.keysym.sym == SDLK_CARET) {
    // Show debug console on caret button click
    guiManager->toggleDebugConsole();
  }
}

// Mouse button down event
void GameManager::mouseDown(const SDL_Event evt) {}

// Mouse button up event
void GameManager::mouseUp(const SDL_Event evt) {}

// Mouse wheel rolled event
void GameManager::mouseWheelRolled(const SDL_Event evt) {}

// Mouse moved event
void GameManager::mouseMoved(const SDL_Event evt) {}

// Finger down  event
void GameManager::fingerDown(const SDL_Event evt) {}

// Finger up event
void GameManager::fingerUp(const SDL_Event evt) {}

// Finger moved event
void GameManager::fingerMoved(const SDL_Event evt) {}

// Frame started event
bool GameManager::frameStarted(const Ogre::FrameEvent& /*evt*/) {
  // Poll for events
  pollEvents();

  // Start new GUI frame
  guiManager->newFrame();

  // Update GUI in current state
  stateManager->updateGui();

  // Refresh audio status
  audioManager->updateStats();

  return true;
}

// Frame rendering queued event (overrides OGRE Bites)
bool GameManager::frameRenderingQueued(const Ogre::FrameEvent& evt) {
  // Update state
  stateManager->updateState(evt);

  return true;
}

// Create camera
void GameManager::createCamera() {
  // Create the camera
  camera = sceneManager->createCamera(
      OPENHOI_BUILD_DYNAMIC_OBJECT_NAME("Player_Cam"));

  // Position it at 100 in Z direction
  camera->setPosition(Ogre::Vector3(0, 0, 100));
  // Look -300 back along Z
  camera->lookAt(Ogre::Vector3(0, 0, -300));

  // Set clip distances
  camera->setNearClipDistance(0.1f);
  camera->setFarClipDistance(500000);  // Some very high value..
  if (root->getRenderSystem()->getCapabilities()->hasCapability(
          Ogre::RSC_INFINITE_FAR_PLANE)) {
    // If possible, set infinite far clip distance
    camera->setFarClipDistance(0);
  }

  // Create one viewport, entire window
  Ogre::Viewport* vp = getRenderWindow()->addViewport(camera);
  vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

  // Alter the camera aspect ratio to match the viewport
  camera->setAspectRatio(Ogre::Real(vp->getActualWidth()) /
                         Ogre::Real(vp->getActualHeight()));
  camera->setAutoAspectRatio(true);
}

// Hook point where shader based technique will be created. Will be called
// whenever the material manager won't find appropriate technique to satisfy
// the target scheme name. If the scheme name is out target RT Shader System
// scheme name we will try to create shader generated technique for it
Ogre::Technique* GameManager::handleSchemeNotFound(
    unsigned short /*schemeIndex*/, const Ogre::String& schemeName,
    Ogre::Material* originalMaterial, unsigned short /*lodIndex*/,
    const Ogre::Renderable* /*rend*/) {
  if (!shaderGenerator->hasRenderState(schemeName)) return nullptr;

  // Create shader generated technique for this material
  if (!shaderGenerator->createShaderBasedTechnique(
          *originalMaterial, Ogre::MaterialManager::DEFAULT_SCHEME_NAME,
          schemeName))
    return nullptr;

  // Force creating the shaders for the generated technique
  shaderGenerator->validateMaterial(schemeName, originalMaterial->getName(),
                                    originalMaterial->getGroup());

  // Grab the generated technique
  Ogre::Material::Techniques::const_iterator it;
  for (it = originalMaterial->getTechniques().begin();
       it != originalMaterial->getTechniques().end(); ++it) {
    Ogre::Technique* curTech = *it;

    if (curTech->getSchemeName() == schemeName) return curTech;
  }

  return nullptr;
}

// Called right before illuminated passes were created, so that owner of
// runtime generated technique can handle this
bool GameManager::beforeIlluminationPassesCleared(Ogre::Technique* tech) {
  if (shaderGenerator->hasRenderState(tech->getSchemeName())) {
    Ogre::Material* mat = tech->getParent();
    shaderGenerator->invalidateMaterialIlluminationPasses(
        tech->getSchemeName(), mat->getName(), mat->getGroup());
    return true;
  }
  return false;
}

// Called right after illuminated passes were created, so that owner of
// runtime generated technique can handle this
bool GameManager::afterIlluminationPassesCreated(Ogre::Technique* tech) {
  if (shaderGenerator->hasRenderState(tech->getSchemeName())) {
    Ogre::Material* mat = tech->getParent();
    shaderGenerator->validateMaterialIlluminationPasses(
        tech->getSchemeName(), mat->getName(), mat->getGroup());
    return true;
  }
  return false;
}

}  // namespace openhoi