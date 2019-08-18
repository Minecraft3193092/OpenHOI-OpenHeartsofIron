// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "state.hpp"

namespace openhoi {

// Game state class
class GameState : public State {
 public:
  // GameState destructor
  ~GameState(){};

  // Called when the state is started up
  void Startup();

  // Called when the state is shutting down
  void Shutdown();

  // Used to create the scene
  void CreateScene();

  // Used to update the scene
  void UpdateScene();

  // Used to remove the scene
  void RemoveScene();

 private:
  GLuint vertexArray;
  GLuint vertexBuffer;
  GLuint vertexShader;
  GLuint fragmentShader;
  GLuint shaderProgram;
  int verticesCount;
};

}  // namespace openhoi