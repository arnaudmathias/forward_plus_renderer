#include "env.hpp"
#include "game.hpp"
#include "renderer.hpp"

int main(int argc, char **argv) {
  // Env env(1600, 900);
  Env env(0, 0);
  if (env.window == nullptr) {
    return (EXIT_FAILURE);
  }
  render::Renderer renderer(env.width, env.height);
  Game game;
  while (!glfwWindowShouldClose(env.window)) {
    env.update();
    glfwPollEvents();
    game.update(env);
    renderer.update(env);
    renderer.clearScreen();
    game.render(env, renderer);
    glfwSwapBuffers(env.window);
  }
  return (EXIT_SUCCESS);
}
