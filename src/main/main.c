#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <stdint.h>
#include <renderer/renderer.h>
#include <game/game.h>

#define MAX_KEY 256
#define WIDTH 800
#define HEIGHT 600

static struct {
  SDL_Window *window;
  SDL_GLContext *gl;
  game_t g;
  int mouse_x;
  int mouse_y;
  int key[MAX_KEY];
} sdl;

void glew_init();
void sdl_init();
int sdl_poll();
void sdl_quit();

int update();

int main(int argc, char *argv[])
{
  sdl_init();
  glew_init();
  renderer_init();
  game_init(&sdl.g);
  
  while (update());
  
  renderer_deinit();
  sdl_quit();
  return 0;
}

int update()
{
  if (!sdl_poll()) {
    return 0;
  }
  
  game_update(&sdl.g, sdl.key, sdl.mouse_x / (float) WIDTH, sdl.mouse_y / (float) HEIGHT);
  renderer_render(&sdl.g);
  
  SDL_GL_SwapWindow(sdl.window);
  
  return 1;
}

void sdl_init(int width, int height)
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL_Error: %s\n", SDL_GetError());
    return;
  }
  
  sdl.window = SDL_CreateWindow(
    "nui",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    WIDTH,
    HEIGHT,
    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
  );
  
  sdl.gl = SDL_GL_CreateContext(sdl.window);
  
  sdl.mouse_x = 0;
  sdl.mouse_y = 0;
  
  for (int i = 0; i < MAX_KEY; i++) {
    sdl.key[i] = 0;
  }
  
  SDL_SetRelativeMouseMode(1);
}

int sdl_poll()
{
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      return 0;
    case SDL_KEYUP:
      if (event.key.keysym.sym >= MAX_KEY) break;
      sdl.key[event.key.keysym.sym] = 0;
      break;
    case SDL_KEYDOWN:
      if (event.key.keysym.sym >= MAX_KEY) break;
      sdl.key[event.key.keysym.sym] = 1;
      break;
    case SDL_MOUSEMOTION:
      sdl.mouse_x += event.motion.xrel;
      sdl.mouse_y -= event.motion.yrel;
      break;
    }
  }
  
  return 1;
}

void sdl_quit()
{
  SDL_GL_DeleteContext(sdl.gl);
  SDL_DestroyWindow(sdl.window);
  SDL_Quit();
}

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  if (type == GL_DEBUG_TYPE_ERROR) {
    fprintf(
      stderr,
      "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
      (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
      type,
      severity,
      message
    );
  }
}

void glew_init()
{
  glewExperimental = 1;
  
  GLenum status = glewInit();
  if (status != GLEW_OK) {
    printf("failed to initialize GLEW: %s\n", glewGetErrorString(status));
    exit(1);
  }
  
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);
}
