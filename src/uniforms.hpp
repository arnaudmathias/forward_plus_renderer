#pragma once
#include "env.hpp"

class Uniforms {
  GLint location;
  std::vector<Uniform> uniforms;
  glm::vec3 iResolution;
  float iTime;
  float iTimeDelta;
  float iFrame;
  float iChannelTime[4];
  glm::vec4 iMouse;
  glm::vec4 iDate;
  float iSampleRate;
  glm::vec3 iChannelResolution[4];
  /* samplerXX iChanneli; */
};
#include "env.hpp"
