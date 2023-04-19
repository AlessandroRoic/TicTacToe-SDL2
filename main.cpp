#define SDL_MAIN_HANDLED

#include <SDL.h>
#include <SDL_ttf.h>
#include <fmt/format.h>
#include <iostream>
#include <set>
#include <vector>

// Game manager variables
const char *WINDOW_TITLE = "TicTacToe";
bool isRunning = true;
SDL_Window *window;
SDL_Renderer *renderer;
int screenWidth = 600;
int screenHeight = 600;
TTF_Font *globalFont;
SDL_Color defaultFontColor = {0, 0, 0};
SDL_Point mousePosition;

//Scene variables
class Cell {
  SDL_Rect rect;
  char player;
  SDL_Texture *texture;
  SDL_Rect textRect;

public:
  [[nodiscard]] const SDL_Rect &getRect() const {
    return rect;
  }

  void setRect(const SDL_Rect &_rect) {
    rect = _rect;
  }

  [[nodiscard]] char getPlayer() const {
    return player;
  }

  void setPlayer(char _player) {
    player = _player;
    loadTexture();
  }

  void free() {
    if (texture != nullptr) {
      SDL_DestroyTexture(texture);
      texture = nullptr;
      player = NULL;
    }
  }

  void loadTexture() {
    free();

    SDL_Surface *surface = TTF_RenderText_Solid(globalFont, &player, defaultFontColor);
    textRect = {
            .x = rect.x + (rect.w - surface->w) / 2,
            .y = rect.y + (rect.h - surface->h) / 2,
            .w = surface->w,
            .h = surface->h};
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
  }

  void render() {
    SDL_RenderCopy(renderer, texture, nullptr, &textRect);
  }
};

std::vector<Cell> cells(9);
char currentPlayer = 'X';
char winner = NULL;
int filledCells = 0;

bool init();

void update();

void handleKeyboardEvent(SDL_Event event);

void handleMouseEvent(SDL_Event event);

void logMessage(const std::string &message, bool isError = false);

void loadScene();

void updateGame();

void renderGame();

char checkValues(const std::vector<int> &cellIndexes);

bool init() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    logMessage("Failed to init SDL", true);
    return false;
  }

  //Get device display mode
  //  SDL_DisplayMode displayMode;
  //  if (SDL_GetCurrentDisplayMode(0, &displayMode) == 0) {
  //    screenWidth = displayMode.w;
  //    screenHeight = displayMode.h;
  //  }

  if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
    logMessage("Warning: Linear texture filtering not enabled!");
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

  if (TTF_Init() != 0) {
    logMessage("Failed to init TTF", true);
    return false;
  }
  globalFont = TTF_OpenFont("assets/fonts/Roboto-Regular.ttf", 100);

  loadScene();
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
      updateGame();
    }
  }
}

void render() {
  //Clear screen
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

  renderGame();

  //Update screen
  SDL_RenderPresent(renderer);
}

void close() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_CloseFont(globalFont);
  renderer = nullptr;
  window = nullptr;
  globalFont = nullptr;
  for (Cell cell: cells) {
    cell.free();
  }
  TTF_Quit();
  SDL_Quit();
}

void handleKeyboardEvent(SDL_Event event) {
}

void handleMouseEvent(SDL_Event event) {
  mousePosition = {.x = event.button.x, .y = event.button.y};
}

void logMessage(const std::string &message, bool isError) {
  if (isError) {
    std::string msg = fmt::format("{} : {}", message, SDL_GetError());
    SDL_Log("%s", msg.c_str());
    return;
  }
  std::printf("%s\n", message.c_str());
}

void loadScene() {
  int x = 0;
  int y = 0;
  int w = screenWidth / 3;
  int h = screenHeight / 3;
  for (Cell &cell: cells) {
    cell.setRect({x, y, w, h});
    if (x + w < screenWidth) x += w;
    else {
      x = 0;
      y += h;
    }
  }
}

void updateGame() {
  for (Cell &cell: cells) {
    if (SDL_PointInRect(&mousePosition, &cell.getRect())) {
      if (cell.getPlayer()) continue;
      cell.setPlayer(currentPlayer);
      currentPlayer = currentPlayer == 'X' ? 'O' : 'X';
      filledCells++;
      break;
    }
  }

  std::vector<int> valuesToCheck;
  for (int i = 0; i < 9; i += 3) {
    valuesToCheck = {i, i + 1, i + 2};
    char resultRow = checkValues(valuesToCheck);
    if (resultRow) {
      winner = resultRow;
      break;
    }
    int offset = i / 3;
    valuesToCheck = {offset, offset + 3, offset + 6};
    char resultCol = checkValues(valuesToCheck);
    if (resultCol) {
      winner = resultCol;
      break;
    }
  }
  valuesToCheck = {0, 4, 8};
  char resultFirstDiagonal = checkValues(valuesToCheck);
  if (resultFirstDiagonal != 0) winner = resultFirstDiagonal;
  valuesToCheck = {2, 4, 6};
  char resultSecondDiagonal = checkValues(valuesToCheck);
  if (resultSecondDiagonal != 0) winner = resultSecondDiagonal;

  if (winner || filledCells == 9) {
    for (Cell &cell: cells) {
      cell.free();
    }
    if (winner) {
      logMessage(fmt::format("Winner is {}", winner));
    } else {
      logMessage(fmt::format("Tie", winner));
    }
  }
}

char checkValues(const std::vector<int> &cellIndexes) {
  std::vector<char> mappedArray;
  for (int index: cellIndexes) {
    mappedArray.push_back(cells[index].getPlayer());
  }
  return (std::set<char>(mappedArray.begin(), mappedArray.end()).size() == 1) ? mappedArray[0] : '\0';
}

void renderGame() {
  if (winner || filledCells == 9) {
    std::string resultText;
    if (winner) {
      resultText = (fmt::format("Winner is {}", winner));
    } else {
      resultText = (fmt::format("Tie", winner));
    }
    SDL_Surface *surface = TTF_RenderText_Solid(globalFont, resultText.c_str(), defaultFontColor);
    SDL_Rect resultTextRect = {
            .x = (screenWidth - surface->w) / 2,
            .y = (screenHeight - surface->h) / 2,
            .w = surface->w,
            .h = surface->h};
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, texture, nullptr, &resultTextRect);
    SDL_FreeSurface(surface);
    SDL_RenderPresent(renderer);
    SDL_Delay(2000);
    winner = NULL;
    currentPlayer = 'X';
    filledCells = 0;
  }
  SDL_RenderDrawLine(renderer, 0, screenHeight / 3, screenWidth, screenHeight / 3);
  SDL_RenderDrawLine(renderer, 0, (screenHeight * 2) / 3, screenWidth, (screenHeight * 2) / 3);
  SDL_RenderDrawLine(renderer, screenWidth / 3, 0, screenWidth / 3, screenHeight);
  SDL_RenderDrawLine(renderer, (screenWidth * 2) / 3, 0, (screenWidth * 2) / 3, screenHeight);

  for (Cell cell: cells) {
    cell.render();
  }
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
