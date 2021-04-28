#include <raylib.h>
#include <stdlib.h>

// Global variables
static bool shouldRedraw = false;
static int screenWidth = 800;
static int screenHeight = 800;
static Vector2 squareSize;
static int TilesX = 20;
static int TilesY = 20;
static int TotalBombs = 20;
typedef struct Bombs {
  int x, y;
} Bomb;
static Bomb *bombPos; // all bombs position
static bool *shown;   // if a tile is hidden or not

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
  switch (argc) {
  case 4:
    TotalBombs = strtol(argv[3], NULL, 0);
  case 3:
    TilesY = strtol(argv[2], NULL, 0);
  case 2:
    TilesX = strtol(argv[1], NULL, 0);
    break;
  }

  // enable Vsync and resize window
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  SetConfigFlags(FLAG_VSYNC_HINT);
  InitWindow(screenWidth, screenHeight, "Minesweeper in C99");

  InitGame();
  if (bombPos == NULL || shown == NULL) // if memory allocation failed
    return 255;

  while (!WindowShouldClose())
    UpdateDrawFrame();

  CloseWindow();
  return 0;
}

void InitGame(void) {
  // do not allow illegal values
  if (TilesX <= 0)
    TilesX = 20;
  if (TilesY <= 0)
    TilesY = 20;
  if (TotalBombs <= 0 || TotalBombs >= TilesX * TilesY)
    TotalBombs = 20;
  bombPos = malloc(20 * sizeof(Bomb));
  shown = calloc(TilesX * TilesY, sizeof(bool));

  // square size
  squareSize.x = (float)screenWidth / (float)TilesX;
  squareSize.y = (float)screenHeight / (float)TilesY;

  NewGame();
  shouldRedraw = true;
}

void NewGame(void) {
  for (int i = 0; i < TotalBombs; i++) {
    int randomx = rand() % TilesX;
    int randomy = rand() % TilesY;
  }
  RedrawAllTiles();
}

void ExplodeAllBombs(void) {}

void RedrawAllTiles(void) {
  ClearBackground(RAYWHITE);
  shouldRedraw = false;
  EndDrawing();
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

void DrawGame(void) {
  if (shouldRedraw) {
    RedrawAllTiles();
    return;
  }
  EndDrawing();
}
