// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "game_manager.hpp"

#include <OgreLogManager.h>
#include <OgreTextureManager.h>
#include <hoibase/file/file_access.hpp>
#ifdef OPENHOI_OS_WINDOWS
#  include <RenderSystems/Direct3D11/OgreD3D11RenderSystem.h>
#else
#  include <RenderSystems/GL3Plus/OgreGL3PlusRenderSystem.h>
#endif

namespace openhoi {
// Initializes the game manager
GameManager::GameManager() {
  // Create options instance
  this->options.reset(new Options());
 
  // Create root object of OGRE system
  std::string logFile = (FileAccess::GetUserGameConfigDirectory() /
                         filesystem::path("client.log"))
                            .u8string();
  this->root.reset(OGRE_NEW Ogre::Root("", "", logFile));

  // Create render sytem
  this->CreateRenderSystem();

  // Disable mipmapping
  //Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(0);

  // Create a generic scene manager
  this->sceneManager.reset(this->root->createSceneManager(Ogre::ST_GENERIC));

  // Create camera
  this->CreateCamera();
}

// Destroys the GameManager
GameManager::~GameManager() {}

// Gets the game options
std::unique_ptr<Options> const& GameManager::GetOptions() const {
  return this->options;
}

// Gets the state manager
std::unique_ptr<StateManager> const& GameManager::GetStateManager() const {
  return this->stateManager;
}

// Gets the OGRE root object
std::unique_ptr<Ogre::Root> const& GameManager::GetRoot() const {
  return this->root;
}

// Gets the OGRE render window
std::unique_ptr<Ogre::RenderWindow> const& GameManager::GetWindow() const {
  return this->window;
}

// Gets the OGRE scene manager
std::shared_ptr<Ogre::SceneManager> const& GameManager::GetSceneManager()
    const {
  return this->sceneManager;
}

// Gets the OGRE camera
std::shared_ptr<Ogre::Camera> const& GameManager::GetCamera() const {
  return this->camera;
}

// Start the main loop
void GameManager::Run() {
  // xxx
}

// Request game exit
void GameManager::RequestExit() {
  // xxxx
}

// Create the render system
void GameManager::CreateRenderSystem() {
#ifdef OPENHOI_OS_WINDOWS
  // Create Direct3D 11 render system
  this->renderSystem.reset(OGRE_NEW Ogre::D3D11RenderSystem());

  // Set driver type to hardware
  this->renderSystem->setConfigOption("Driver type", "Hardware");

  // Disable NVIDIA PerfHUD
  this->renderSystem->setConfigOption("Allow NVPerfHUD", "No");

  // Set floating-point mode to double precision
  this->renderSystem->setConfigOption("Floating-point mode", "Consistent");

  // Set DirectX feature levels
  this->renderSystem->setConfigOption("Min Requested Feature Levels", "9.1");
  this->renderSystem->setConfigOption("Max Requested Feature Levels", "11.1");
#else
  // Create OpenGL 3 + render system
  this->renderSystem.reset(OGRE_NEW Ogre::GL3PlusRenderSystem());

  // Set RTT Preferred Mode to FBO
  this->renderSystem->setConfigOption("RTT Preferred Mode", "FBO");
#endif

  // Set video mode
  this->renderSystem->setConfigOption("Video Mode",
                                      this->options->GetVideoMode());

  // Set Full Screen Anti-Aliasing (FSAA)
  this->renderSystem->setConfigOption(
      "FSAA", std::to_string(this->options->GetFullScreenAntiAliasing()));

  if (!this->options->GetRenderingDevice().empty()) {
    // Set Rendering device
    this->renderSystem->setConfigOption("Rendering Device",
                                        this->options->GetRenderingDevice());
  }

  // Set window mode
  this->renderSystem->setConfigOption(
      "Full Screen",
      this->options->GetWindowMode() == WindowMode::FULLSCREEN ? "Yes" : "No");
  // TODO: Borderless

  // Set VSync (turn always on in windowed screen mode because disabling VSync
  // can cause timing issues at lower frame rates here)
  this->renderSystem->setConfigOption(
      "VSync", (this->options->IsVerticalSync() ||
                !this->options->GetWindowMode() == WindowMode::FULLSCREEN)
                   ? "Yes"
                   : "No");

  // Set VSync interval
  this->renderSystem->setConfigOption("VSync Interval", "1");

  // Disable sRGB Gamma conversion
  this->renderSystem->setConfigOption("sRGB Gamma Conversion", "No");

  // Add render system to OGRE root and set it as the default render system
  this->root->addRenderSystem(this->renderSystem.get());
  this->root->setRenderSystem(this->renderSystem.get());

  // Print out render system details to log
  Ogre::LogManager::getSingletonPtr()->logMessage(
      "*** Render system config options ***");
  const Ogre::ConfigOptionMap& configOptions =
      this->renderSystem->getConfigOptions();
  for (auto opt : configOptions) {
    Ogre::LogManager::getSingletonPtr()->logMessage(" [*] " + opt.first);
    for (Ogre::StringVector::iterator vals = opt.second.possibleValues.begin();
         vals != opt.second.possibleValues.end(); vals++)
      Ogre::LogManager::getSingletonPtr()->logMessage("   -> " + *vals);
  }
}

// Create camera
void GameManager::CreateCamera() {
  this->camera.reset(this->sceneManager->createCamera("Player"));

  // Position it at 100 in Z direction
  this->camera->setPosition(Ogre::Vector3(0, 0, 100));
  // Look -300 back along Z
  this->camera->lookAt(Ogre::Vector3(0, 0, -300));

  // Set clip distances
  this->camera->setNearClipDistance(0.1f);
  this->camera->setFarClipDistance(500000);
  if (this->renderSystem->getCapabilities()->hasCapability(
          Ogre::RSC_INFINITE_FAR_PLANE)) {
    // If possible, set infinite far clip distance
    this->camera->setFarClipDistance(0);
  }

  // Set auto aspect ratio
  this->camera->setAutoAspectRatio(true);
}

}  // namespace openhoi