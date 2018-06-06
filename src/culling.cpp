#include "culling.hpp"

FrustrumCulling::FrustrumCulling(void) {}

FrustrumCulling::FrustrumCulling(FrustrumCulling const &src) { *this = src; }

FrustrumCulling::~FrustrumCulling(void) {}

FrustrumCulling &FrustrumCulling::operator=(FrustrumCulling const &rhs) {
  if (this != &rhs) {
  }
  return (*this);
}

void FrustrumCulling::updateViewPlanes(const glm::mat4 &proj_view) {
  glm::mat4 tproj_view = glm::transpose(proj_view);
  frustrum_planes[0] = glm::normalize(tproj_view[0] + tproj_view[3]);
  frustrum_planes[1] = glm::normalize(tproj_view[3] - tproj_view[0]);
  frustrum_planes[2] = glm::normalize(tproj_view[3] - tproj_view[1]);
  frustrum_planes[3] = glm::normalize(tproj_view[3] + tproj_view[1]);
  frustrum_planes[4] = glm::normalize(tproj_view[2]);
  frustrum_planes[5] = glm::normalize(tproj_view[3] - tproj_view[2]);
}

bool FrustrumCulling::cull(const glm::vec3 &center, const glm::vec3 &halfsize) {
  bool visible = true;
  for (int plane_id = 0; plane_id < 6; plane_id++) {
    glm::vec3 absPlane = glm::abs(glm::vec3(frustrum_planes[plane_id]));
    float d = dot(center, glm::vec3(frustrum_planes[plane_id]));
    float r = dot(halfsize, absPlane);
    if (d + r < -frustrum_planes[plane_id].w) {
      return (false);
    }
  }
  return (true);
}
