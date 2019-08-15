// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "game_manager.hpp"
#include "state/menu_state.hpp"

#include <ImguiManager.h>
#include <OgreLogManager.h>
#include <OgreOverlaySystem.h>
#include <OgreRTShaderSystem.h>
#include <OgreRoot.h>
#include <OgreSTBICodec.h>
#include <OgreTextureManager.h>
#include <SDL.h>
#include <hoibase/file/file_access.hpp>

#include <exception>

// Direct3D11 causes blurry font textures and thus we disable support for it at
// the moment
//#define ENABLE_DIRECT3D11

namespace openhoi {

// Initializes the game manager
GameManager::GameManager() : OgreBites::ApplicationContext(OPENHOI_GAME_NAME) {
  // Create options instance
  options = new Options();

  // Initialize render system and resources
  initApp();

  // Create ImGui manager and configure GUI
  Ogre::ImguiManager::createSingleton();
  configureGui();

  // Add input listeners
  addInputListener(this);
  addInputListener(Ogre::ImguiManager::getSingleton().getInputListener());

  // Create a generic scene manager
  sceneManager = mRoot->createSceneManager();
  sceneManager->addRenderQueueListener(mOverlaySystem);
  Ogre::ImguiManager::getSingleton().init(sceneManager);

  // Register our scene with the RTSS
  Ogre::RTShader::ShaderGenerator::getSingleton().addSceneManager(sceneManager);

  // Create camera
  createCamera();

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
#if defined(OPENHOI_OS_WINDOWS) && defined(ENABLE_DIRECT3D11)
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
#if defined(OPENHOI_OS_WINDOWS) && defined(ENABLE_DIRECT3D11)
  }
#endif

  if (mRoot->getAvailableRenderers().empty()) {
    // Use legacy OpenGL as a fallback
    mRoot->loadPlugin(getPluginPath(OGRE_PLUGIN_RS_GL));
  }

  // Get loaded render system
  Ogre::RenderSystem* renderSystem = mRoot->getAvailableRenderers().front();

  // Configure render system
#if defined(OPENHOI_OS_WINDOWS) && defined(ENABLE_DIRECT3D11)
  if (directX) {
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
                options->GetWindowMode() != WindowMode::FULLSCREEN)
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
  filesystem::path assetRoot = FileAccess::getAssetRootDirectory();

  // Declare all audio resources
  Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
      (assetRoot / "audio").u8string(), "FileSystem", Ogre::RGN_DEFAULT);

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
}

// Load resources (overrides OGRE Bites)
void GameManager::loadResources() {
  // Disable mipmapping
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(0);

  // Initialize and load resource groups
  std::array<std::string, 3> defaultResourceGroups = {
      Ogre::RGN_DEFAULT, OPENHOI_RSG_COA_TEXTURES, OPENHOI_RSG_FLAG_TEXTURES};
  for (std::string& resourceGroup : defaultResourceGroups) {
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(
        resourceGroup);
    Ogre::ResourceGroupManager::getSingleton().loadResourceGroup(resourceGroup);
  }

  // Initialize font
  Ogre::ImguiManager::getSingleton().addFont("gui/default", Ogre::RGN_DEFAULT);
}

// Frame started event (override OGRE Bites)
bool GameManager::frameStarted(const Ogre::FrameEvent& evt) {
  bool continueRendering = OgreBites::ApplicationContext::frameStarted(evt);
  if (continueRendering) {
    Ogre::ImguiManager::getSingleton().newFrame(
        evt.timeSinceLastFrame, Ogre::Rect(0, 0, getRenderWindow()->getWidth(),
                                           getRenderWindow()->getHeight()));
    ImGui::ShowDemoWindow();
  }
  return continueRendering;
}

// Frame rendering queued event (overrides OGRE Bites)
bool GameManager::frameRenderingQueued(const Ogre::FrameEvent& evt) {
  bool continueRendering =
      OgreBites::ApplicationContext::frameRenderingQueued(evt);
  if (continueRendering) {
    // Update state
    stateManager->updateState();
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

  // Create one viewport, entire window
  Ogre::Viewport* vp = getRenderWindow()->addViewport(camera);
  vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

  // Alter the camera aspect ratio to match the viewport
  camera->setAspectRatio(Ogre::Real(vp->getActualWidth()) /
                         Ogre::Real(vp->getActualHeight()));
  camera->setAutoAspectRatio(true);
}

// Configure GUI
void GameManager::configureGui() {
  // Get ImGui IO
  ImGuiIO& io = ImGui::GetIO();

  // Disable INI file
  io.IniFilename = NULL;

  // Enable docking
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  // Set GUI style
  ImGuiStyle& style = ImGui::GetStyle();
  style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.25f, 0.25f, 0.25f, 0.96f);
  style.Colors[ImGuiCol_ChildBg] = ImVec4(0.25f, 0.25f, 0.25f, 0.96f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(0.25f, 0.25f, 0.25f, 0.96f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.12f, 0.12f, 0.12f, 0.71f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(0.42f, 0.42f, 0.42f, 0.54f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.67f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.335f, 0.335f, 0.335f, 1.000f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.24f, 0.24f, 0.24f, 0.53f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] =
      ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] =
      ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.54f, 0.54f, 0.54f, 0.35f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.52f, 0.52f, 0.52f, 0.59f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.38f, 0.38f, 0.38f, 0.94f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.47f, 0.47f, 0.47f, 0.98f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.76f, 0.76f, 0.76f, 0.98f);
  style.Colors[ImGuiCol_Separator] = ImVec4(0.000f, 0.000f, 0.000f, 0.137f);
  style.Colors[ImGuiCol_SeparatorHovered] =
      ImVec4(0.700f, 0.671f, 0.600f, 0.290f);
  style.Colors[ImGuiCol_SeparatorActive] =
      ImVec4(0.702f, 0.671f, 0.600f, 0.674f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.48f, 0.47f, 0.45f, 0.29f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.70f, 0.67f, 0.60f, 0.67f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogramHovered] =
      ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
  style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
  style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
  style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_NavWindowingHighlight] =
      ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
  style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
  style.Colors[ImGuiCol_Tab] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
  style.Colors[ImGuiCol_TabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
  style.Colors[ImGuiCol_TabActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
  style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
  style.Colors[ImGuiCol_TabUnfocusedActive] =
      ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
  style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.85f, 0.85f, 0.85f, 0.28f);

  style.PopupRounding = 3;

  style.WindowPadding = ImVec2(4, 4);
  style.FramePadding = ImVec2(6, 4);
  style.ItemSpacing = ImVec2(6, 2);

  style.ScrollbarSize = 18;

  style.WindowBorderSize = 1;
  style.ChildBorderSize = 1;
  style.PopupBorderSize = 1;
  style.FrameBorderSize = 1;
  style.TabBorderSize = 1;

  style.WindowRounding = 3;
  style.ChildRounding = 3;
  style.FrameRounding = 3;
  style.ScrollbarRounding = 2;
  style.GrabRounding = 3;
  style.TabRounding = 3;
}

}  // namespace openhoi