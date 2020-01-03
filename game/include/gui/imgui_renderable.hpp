// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include <OgreCamera.h>
#include <OgrePrerequisites.h>
#include <OgreRenderOperation.h>
#include <OgreRenderable.h>
#include <imgui.h>

namespace openhoi {

// ImGui renderable
class ImGuiRenderable final : public Ogre::Renderable {
 public:
  // ImGui renderable constructor
  ImGuiRenderable();
  // ImGui renderable destructor
  ~ImGuiRenderable();

  // Update vertex data
  void updateVertexData(const ImVector<ImDrawVert>& vtxBuf,
                        const ImVector<ImDrawIdx>& idxBuf);

  // Get squared view depth from camera
  Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const;

  // Retrieves a weak reference to the material the GUI
  virtual const Ogre::MaterialPtr& getMaterial() const;

  // Gets the world transform matrix / matrices for the GUI
  virtual void getWorldTransforms(Ogre::Matrix4* xform) const;

  // Gets the render operation required to send this object to the frame buffer
  virtual void getRenderOperation(Ogre::RenderOperation& op);

  // Get the list of lights attached to the GUI
  virtual const Ogre::LightList& getLights() const;

  Ogre::MaterialPtr material;
  Ogre::Matrix4 xform;
  Ogre::RenderOperation renderOperation;
};

}  // namespace openhoi