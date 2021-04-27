#include "raylib.h"
#include <stdlib.h>

// Global variables
static bool shouldRedraw = false;
static int screenWidth = 800;
static int screenHeight = 450;
static int TilesX = 20;
static int TilesY = 20;
static int Bombs = 20;
static Vector2 squareSize;

// function declerations
static void InitGame(void);
static void RedrawAllTiles(void);
static void ExplodeAllBombs(void);
static void NewGame(void);
static void UpdateDrawFrame(void);
static void UpdateGame(void);
static void DrawGame(void);

int main(int argc, char *argv[]) {
  // Set game area and bomb amount with command line args
  //
  // TODO: bytt ut atoi med
  // https://stackoverflow.com/questions/2729460/why-do-i-get-this-unexpected-result-using-atoi-in-c
  switch (argc) {
  case 4:
    Bombs = atoi(argv[3]);
  case 3:
    TilesY = atoi(argv[2]);
  case 2:
    TilesX = atoi(argv[1]);
  }

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  SetConfigFlags(FLAG_VSYNC_HINT);

  InitWindow(screenWidth, screenHeight, "Minesweeper in C99");

  InitGame();

  while (!WindowShouldClose()) {
    UpdateDrawFrame();
  }

  CloseWindow();

  return 0;
}

void InitGame(void) {

  // do not allow illegal values
  if (TilesX <= 0) {
    TilesX = 20;
  }
  if (TilesY <= 0) {
    TilesY = 20;
  }
  if (Bombs <= 0 || Bombs >= TilesX * TilesY) {
    Bombs = 20;
  }

  // square size
  squareSize.x = (float)screenWidth / (float)TilesX;
  squareSize.y = (float)screenHeight / (float)TilesY;

  NewGame();
  shouldRedraw = true;
}

void NewGame(void) { RedrawAllTiles(); }

void RedrawAllTiles(void) {
  ClearBackground(RAYWHITE);
  shouldRedraw = false;
}

void UpdateDrawFrame(void) {
  // if screen is rezised
  const int newScreenWidth = GetScreenWidth();
  const int newScreenHeight = GetScreenHeight();
  if (screenWidth != newScreenWidth) {
    screenWidth = newScreenWidth;
    shouldRedraw = true;
  }
  if (screenHeight != newScreenHeight) {
    screenHeight = newScreenHeight;
    shouldRedraw = true;
  }

  UpdateGame();
  DrawGame();
}
void UpdateGame(void) {}
void DrawGame(void) { EndDrawing(); }
