#include "env.hpp"
#include "game.hpp"
#include "renderer.hpp"

int main(int argc, char **argv) {
  Env env(1280, 720);
  env.inputHandler.mouseDisabled = true;
  env.inputHandler.keybrDisabled = true;
  render::Renderer renderer(env.width, env.height);
  GL_DUMP_ERROR("renderer init");
  Game game;
  while (!glfwWindowShouldClose(env.window)) {
    env.update();
    glfwPollEvents();
    game.update(env);
    renderer.update(env);
    renderer.clearScreen();
    game.render(env, renderer);
    // GL_DUMP_ERROR("draw loop");
    glfwSwapBuffers(env.window);
  }
  return (EXIT_SUCCESS);
}
