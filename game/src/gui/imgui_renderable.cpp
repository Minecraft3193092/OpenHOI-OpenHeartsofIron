// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "gui/imgui_renderable.hpp"

#include <OgreHardwareBufferManager.h>

namespace openhoi {

// ImGui renderable constructor
ImGuiRenderable::ImGuiRenderable() {
  // Use identity projection and view matrices
  mUseIdentityProjection = true;
  mUseIdentityView = true;

  renderOperation.vertexData = OGRE_NEW Ogre::VertexData();
  renderOperation.indexData = OGRE_NEW Ogre::IndexData();

  renderOperation.vertexData->vertexCount = 0;
  renderOperation.vertexData->vertexStart = 0;

  renderOperation.indexData->indexCount = 0;
  renderOperation.indexData->indexStart = 0;
  renderOperation.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;
  renderOperation.useIndexes = true;
  renderOperation.useGlobalInstancingVertexBufferIsAvailable = false;

  Ogre::VertexDeclaration* decl = renderOperation.vertexData->vertexDeclaration;

  // Vertex declaration
  size_t offset = 0;
  decl->addElement(0, offset, Ogre::VET_FLOAT2, Ogre::VES_POSITION);
  offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT2);
  decl->addElement(0, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES,
                   0);
  offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT2);
  decl->addElement(0, offset, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);

  // By default we want ImGui renderables to still work in wireframe mode
  setPolygonModeOverrideable(false);
}

// ImGui renderable destructor
ImGuiRenderable::~ImGuiRenderable() {
  OGRE_DELETE renderOperation.vertexData;
  OGRE_DELETE renderOperation.indexData;
}

// Update vertex data
void ImGuiRenderable::updateVertexData(const ImVector<ImDrawVert>& vtxBuf,
                                       const ImVector<ImDrawIdx>& idxBuf) {
  Ogre::VertexBufferBinding* bind =
      renderOperation.vertexData->vertexBufferBinding;

  if (bind->getBindings().empty() ||
      bind->getBuffer(0)->getNumVertices() != (size_t)vtxBuf.size()) {
    bind->setBinding(
        0, Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
               sizeof(ImDrawVert), vtxBuf.size(),
               Ogre::HardwareBuffer::HBU_WRITE_ONLY));
  }
  if (!renderOperation.indexData->indexBuffer ||
      renderOperation.indexData->indexBuffer->getNumIndexes() !=
          (size_t)idxBuf.size()) {
    renderOperation.indexData->indexBuffer =
        Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
            Ogre::HardwareIndexBuffer::IT_16BIT, idxBuf.size(),
            Ogre::HardwareBuffer::HBU_WRITE_ONLY);
  }

  // Copy all vertices
  bind->getBuffer(0)->writeData(0, vtxBuf.size_in_bytes(), vtxBuf.Data, true);
  renderOperation.indexData->indexBuffer->writeData(0, idxBuf.size_in_bytes(),
                                                    idxBuf.Data, true);

  renderOperation.vertexData->vertexStart = 0;
  renderOperation.vertexData->vertexCount = vtxBuf.size();
}

// Get squared view depth from camera
Ogre::Real ImGuiRenderable::getSquaredViewDepth(const Ogre::Camera* cam) const {
  (void)cam;
  return (Ogre::Real)0;
}

// Retrieves a weak reference to the material the GUI
const Ogre::MaterialPtr& ImGuiRenderable::getMaterial() const {
  return material;
}

// Gets the world transform matrix / matrices for the GUI
void ImGuiRenderable::getWorldTransforms(Ogre::Matrix4* xform) const {
  *xform = this->xform;
}

// Gets the render operation required to send this object to the frame buffer
void ImGuiRenderable::getRenderOperation(Ogre::RenderOperation& op) {
  op = renderOperation;
}

// Get the list of lights attached to the GUI
const Ogre::LightList& ImGuiRenderable::getLights() const {
  static const Ogre::LightList l;
  return l;
}

}  // namespace openhoi