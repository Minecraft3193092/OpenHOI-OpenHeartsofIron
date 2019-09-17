// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "gui/gui_manager.hpp"

#include <hoibase/openhoi.hpp>

#include <OgreFont.h>
#include <OgreFontManager.h>
#include <OgreFrameListener.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreMaterialManager.h>
#include <OgrePrerequisites.h>
#include <OgreRenderTarget.h>
#include <OgreRoot.h>
#include <OgreTechnique.h>
#include <OgreTexture.h>
#include <OgreTextureManager.h>
#include <OgreViewport.h>

#include <cassert>

namespace openhoi {

// Initializes the GUI manager
GuiManager::GuiManager() {
  // Create ImGui context
  ImGui::CreateContext();

  // Configure ImGui
  configureGui();

  // Create font texture
  createFontTexture();

  // Create GUI material
  createMaterial();

  // Create debug console
  debugConsole = new DebugConsole();
}

// Destroys the GUI manager
GuiManager::~GuiManager() {
  // Destroy debug console
  delete debugConsole;

  // Destroy ImGui context
  auto imGuiContext = ImGui::GetCurrentContext();
  if (imGuiContext) ImGui::DestroyContext();
}

// Render queue ended event
void GuiManager::renderQueueEnded(Ogre::uint8 queueGroupId,
                                  const Ogre::String& invocation,
                                  bool& /*repeatThisInvocation*/) {
  if (queueGroupId != Ogre::RENDER_QUEUE_OVERLAY || invocation == "SHADOWS")
    return;

  Ogre::RenderSystem* renderSystem =
      Ogre::Root::getSingletonPtr()->getRenderSystem();
  Ogre::Viewport* vp = renderSystem->_getViewport();

  if (!vp || (!vp->getTarget()->isPrimary()) || frameEnded) return;

  frameEnded = true;
  ImGuiIO& io = ImGui::GetIO();

  // Construct projection matrix, taking texel offset corrections in account
  const float texelOffsetX = renderSystem->getHorizontalTexelOffset();
  const float texelOffsetY = renderSystem->getVerticalTexelOffset();
  const float L = texelOffsetX;
  const float R = io.DisplaySize.x + texelOffsetX;
  const float T = texelOffsetY;
  const float B = io.DisplaySize.y + texelOffsetY;

  renderable.xform = Ogre::Matrix4(
      2.0f / (R - L), 0.0f, 0.0f, (L + R) / (L - R), 0.0f, -2.0f / (B - T),
      0.0f, (T + B) / (B - T), 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

  // Instruct ImGui to Render() and process the resulting CmdList-s
  ImGui::Render();
  ImDrawData* drawData = ImGui::GetDrawData();

  int vpWidth = vp->getActualWidth();
  int vpHeight = vp->getActualHeight();

  for (int i = 0; i < drawData->CmdListsCount; ++i) {
    const ImDrawList* drawList = drawData->CmdLists[i];
    renderable.updateVertexData(drawList->VtxBuffer, drawList->IdxBuffer);

    unsigned int startIdx = 0;

    for (int j = 0; j < drawList->CmdBuffer.Size; ++j) {
      // Create a renderable and fill it's buffers
      const ImDrawCmd* drawCmd = &drawList->CmdBuffer[j];

      // Set scissoring
      int scLeft = static_cast<int>(drawCmd->ClipRect.x);
      int scTop = static_cast<int>(drawCmd->ClipRect.y);
      int scRight = static_cast<int>(drawCmd->ClipRect.z);
      int scBottom = static_cast<int>(drawCmd->ClipRect.w);

      scLeft = scLeft < 0
                   ? 0
                   : (scLeft > vpWidth
                          ? vpWidth
                          : scLeft);  // Clamp bounds to viewport dimensions
      scRight = scRight < 0 ? 0 : (scRight > vpWidth ? vpWidth : scRight);
      scTop = scTop < 0 ? 0 : (scTop > vpHeight ? vpHeight : scTop);
      scBottom = scBottom < 0 ? 0 : (scBottom > vpHeight ? vpHeight : scBottom);

      if (renderable.material->getSupportedTechniques().empty())
        renderable.material->load();  // Support for adding lights run time

      Ogre::Pass* pass = renderable.material->getBestTechnique()->getPass(0);
      Ogre::TextureUnitState* st = pass->getTextureUnitState(0);
      if (drawCmd->TextureId != 0) {
        Ogre::ResourceHandle handle = (Ogre::ResourceHandle)drawCmd->TextureId;
        Ogre::TexturePtr texture = Ogre::static_pointer_cast<Ogre::Texture>(
            Ogre::TextureManager::getSingleton().getByHandle(handle));
        if (texture) {
          st->setTexture(texture);
          st->setTextureFiltering(Ogre::TFO_TRILINEAR);
        }
      } else {
        st->setTexture(fontTexture);
        st->setTextureFiltering(Ogre::TFO_NONE);
      }
      renderSystem->setScissorTest(true, scLeft, scTop, scRight, scBottom);

      // Render
      renderable.renderOperation.indexData->indexStart = startIdx;
      renderable.renderOperation.indexData->indexCount = drawCmd->ElemCount;
      sceneManager->_injectRenderWithPass(pass, &renderable, false);

      // Update counts
      startIdx += drawCmd->ElemCount;
    }
  }
  renderSystem->setScissorTest(false);
}

// Initialize GUI manager
void GuiManager::initialize(Ogre::SceneManager* sceneManager) {
  // Set scene manager reference
  this->sceneManager = sceneManager;
}

// Create font texture
void GuiManager::createFontTexture() {
  // Build texture atlas
  ImGuiIO& io = ImGui::GetIO();
  if (io.Fonts->Fonts.empty()) io.Fonts->AddFontDefault();

  unsigned char* pixels;
  int width, height;
  io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

  fontTexture = Ogre::TextureManager::getSingleton().createManual(
      OPENHOI_BUILD_DYNAMIC_OBJECT_NAME("ImGui_Font_Texture"),
      Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME,
      Ogre::TEX_TYPE_2D, width, height, 1, 1, Ogre::PF_BYTE_RGBA);
  fontTexture->getBuffer()->blitFromMemory(Ogre::PixelBox(
      Ogre::Box(0, 0, width, height), Ogre::PF_BYTE_RGBA, pixels));

  codePointRanges.clear();
}

// Create GUI material
void GuiManager::createMaterial() {
  renderable.material = std::dynamic_pointer_cast<Ogre::Material>(
      Ogre::MaterialManager::getSingleton()
          .createOrRetrieve(
              "imgui/material",
              Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME)
          .first);
  Ogre::Pass* pass = renderable.material->getTechnique(0)->getPass(0);
  pass->setCullingMode(Ogre::CULL_NONE);
  pass->setDepthFunction(Ogre::CMPF_ALWAYS_PASS);
  pass->setLightingEnabled(false);
  pass->setVertexColourTracking(Ogre::TVC_DIFFUSE);
  pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
  pass->setSeparateSceneBlendingOperation(Ogre::SBO_ADD, Ogre::SBO_ADD);
  pass->setSeparateSceneBlending(
      Ogre::SBF_SOURCE_ALPHA, Ogre::SBF_ONE_MINUS_SOURCE_ALPHA,
      Ogre::SBF_ONE_MINUS_SOURCE_ALPHA, Ogre::SBF_ZERO);

  Ogre::TextureUnitState* texUnit = pass->createTextureUnitState();
  texUnit->setTexture(fontTexture);
  texUnit->setTextureFiltering(Ogre::TFO_NONE);

  renderable.material->load();
}

// Load a single font
ImFont* GuiManager::loadFont(std::string name) {
  Ogre::FontPtr font =
      Ogre::FontManager::getSingleton().getByName(name, Ogre::RGN_DEFAULT);
  assert(font && font->getType() == Ogre::FT_TRUETYPE);
  Ogre::DataStreamPtr dataStreamPtr =
      Ogre::ResourceGroupManager::getSingleton().openResource(font->getSource(),
                                                              font->getGroup());
  Ogre::MemoryDataStream ttfChunk(dataStreamPtr,
                                  false);  // transfer ownership to ImGui

  // Convert codepoint ranges for ImGui
  std::vector<ImWchar> codePointRange;
  for (const auto& r : font->getCodePointRangeList()) {
    codePointRange.push_back(r.first);
    codePointRange.push_back(r.second);
  }

  ImGuiIO& io = ImGui::GetIO();
  const ImWchar* cprangePtr = io.Fonts->GetGlyphRangesDefault();
  if (!codePointRange.empty()) {
    codePointRange.push_back(0);  // terminate
    codePointRanges.push_back(codePointRange);
    // ptr must persist until createFontTexture
    cprangePtr = codePointRanges.back().data();
  }

  ImFontConfig cfg;
#ifdef _WIN32
#  pragma warning(push)
#  pragma warning(disable : 4996)
#endif
  strncpy(cfg.Name, name.c_str(), 40);
#ifdef _WIN32
#  pragma warning(pop)
#endif
  return io.Fonts->AddFontFromMemoryTTF(ttfChunk.getPtr(), (int)ttfChunk.size(),
                                        font->getTrueTypeSize(), &cfg,
                                        cprangePtr);
}

// Gets the default font
ImFont* GuiManager::getDefaultFont() {
  assert(defaultFont);
  return defaultFont;
}

// Gets the big font
ImFont* GuiManager::getBigFont() {
  assert(bigFont);
  return bigFont;
}

// Toggle debug console
void GuiManager::toggleDebugConsole() { debugConsole->toggle(); }

// Render new GUI frame
void GuiManager::newFrame(const Ogre::FrameEvent& evt, Ogre::uint32 windowWidth,
                          Ogre::uint32 windowHeight) {
  // Start new ImGui frame
  frameEnded = false;
  ImGuiIO& io = ImGui::GetIO();
  io.DeltaTime = std::max(evt.timeSinceLastFrame, 1e-4f);

  // Read keyboard modifiers inputs
  io.KeyAlt = false;
  io.KeySuper = false;

  // Setup display size (every frame to accommodate for window resizing)
  io.DisplaySize = ImVec2((float)windowWidth, (float)windowHeight);

  // Start the ImGui frame
  ImGui::NewFrame();

  // Draw debug console
  debugConsole->draw();
}

// Configure GUI
void GuiManager::configureGui() {
  // Load default dont
  defaultFont = loadFont("gui/default");
  // Load big font
  bigFont = loadFont("gui/big");

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