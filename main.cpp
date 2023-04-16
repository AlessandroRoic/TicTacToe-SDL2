#include <SDL.h>
#include <fmt/format.h>

const char *WINDOW_TITLE = "TicTacToe";

bool isRunning = true;
SDL_Window *window;
SDL_Renderer *renderer;
int screenWidth = 640;
int screenHeight = 480;

bool init();

void update();

void handleKeyboardEvent(SDL_Event event);

void handleMouseEvent(SDL_Event event);

void logMessage(const std::string &message, bool isError);

bool init() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    logMessage("Failed to init SDL", true);
    return false;
  }
  //Get device display mode
  SDL_DisplayMode displayMode;
  if (SDL_GetCurrentDisplayMode(0, &displayMode) == 0) {
    screenWidth = displayMode.w;
    screenHeight = displayMode.h;
  }
  window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight,
                            SDL_WINDOW_SHOWN);
  if (!window) {
    logMessage("Failed to load window", true);
    return false;
  }
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) {
    logMessage("Failed to load renderer", true);
    return false;
  }
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  return true;
}

void update() {
  SDL_Event event;
  while (SDL_PollEvent(&event) != 0) {
    if (event.type == SDL_QUIT) {
      isRunning = false;
      return;
    } else if (event.type == SDL_KEYDOWN) {
      handleKeyboardEvent(event);
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
      handleMouseEvent(event);
    }
  }
}

void render() {
  //Clear screen
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

  SDL_RenderDrawLine(renderer, 0, screenHeight / 3, screenWidth, screenHeight / 3);
  SDL_RenderDrawLine(renderer, 0, (screenHeight * 2) / 3, screenWidth, (screenHeight * 2) / 3);

  SDL_RenderDrawLine(renderer, screenWidth / 3, 0, screenWidth / 3, screenHeight);
  SDL_RenderDrawLine(renderer, (screenWidth * 2) / 3, 0, (screenWidth * 2) / 3, screenHeight);

  //Update screen
  SDL_RenderPresent(renderer);
}

void close() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  renderer = nullptr;
  window = nullptr;
  SDL_Quit();
}

void handleKeyboardEvent(SDL_Event event) {

}

void handleMouseEvent(SDL_Event event) {

}

void logMessage(const std::string &message, bool isError) {
  std::string formattedMessage = fmt::format("{} : {}", message, isError ? SDL_GetError() : "");
  SDL_Log("%s", formattedMessage.c_str());
}

int main() {
  if (!init()) return 1;

  while (isRunning) {
    update();
    render();
  }

  close();

  return 0;
}
