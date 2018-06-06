#pragma once
#include <iostream>
#include "anim.hpp"

class FrustrumCulling {
 public:
  FrustrumCulling(void);
  FrustrumCulling(FrustrumCulling const& src);
  ~FrustrumCulling(void);
  FrustrumCulling& operator=(FrustrumCulling const& rhs);
  void updateViewPlanes(const glm::mat4& proj_view);
  bool cull(const glm::vec3& center, const glm::vec3& halfsize);

 private:
  glm::vec4 frustrum_planes[6];
};
