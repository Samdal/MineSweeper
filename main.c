#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

// Global variables
static int screenWidth = 800;
static int screenHeight = 800;
static Vector2 squareSize;
static int TilesX = 20;
static int TilesY = 20;
static int TotalBombs = 40;
typedef struct Bombs {
  int x, y;
} Bomb;
static Bomb *bombPos; // all bombs position
static bool *shown;   // if a tile is hidden or not

// function declerations
static void ExplodeAllBombs(void);
static void NewGame(void);
static void UpdateGame(void);
static void DrawGame(void);
static void DrawTile(int x, int y);

int main(int argc, char *argv[]) {
  // Set game area and bomb amount with command line args
  switch (argc) {
  case 4:
    TotalBombs = strtol(argv[3], NULL, 0);
  case 3:
    TilesY = strtol(argv[2], NULL, 0);
    if (TilesY <= 0)
      TilesY = 20;
  case 2:
    TilesX = strtol(argv[1], NULL, 0);
    if (TilesX <= 0)
      TilesX = 20;
    if ((argc == 4) && (TotalBombs <= 0 || TotalBombs >= TilesX * TilesY))
      TotalBombs = (TilesX * TilesY) / 7;
  }
  shown = malloc(TilesX * TilesY * sizeof(bool));
  bombPos = malloc(TotalBombs * sizeof(Bomb));
  if (bombPos == NULL || shown == NULL)
    return 255; // if memory allocation failed

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  SetConfigFlags(FLAG_VSYNC_HINT);
  InitWindow(screenWidth, screenHeight, "Minesweeper in C99");
  NewGame();

  while (!WindowShouldClose()) {
    UpdateGame();
    DrawGame();
  }

  // close down and free memory
  free(bombPos);
  free(shown);
  CloseWindow();
  return 0;
}

void NewGame(void) {
  // reset board
  for (int i = 0; i < TotalBombs; i++) {
    bombPos[i].x = 0;
    bombPos[i].y = 0;
  }
  for (int x = 0; x < TilesX; x++)
    for (int y = 0; y < TilesY; y++)
      shown[x * TilesX + y] = false;

  // place bombs (with no duplicates)
  for (int i = 0; i < TotalBombs; i++) {
    bombPos[i].x = rand() % TilesX;
    bombPos[i].y = rand() % TilesY;
    for (int j = 0; j < TotalBombs; j++)
      if ((j != i) &&
          (bombPos[i].x == bombPos[j].x && bombPos[i].y == bombPos[j].y))
        i--; // repeat bomb
  }
}

void ExplodeAllBombs(void) {}

void UpdateGame(void) {}

void DrawGame(void) {
  squareSize.x = GetScreenWidth() / TilesX;
  squareSize.y = GetScreenHeight() / TilesY;
  ClearBackground(RAYWHITE);
  for (int x = 0; x < TilesX; x++)
    for (int y = 0; y < TilesY; y++) {
      // if (shown[x * TilesX + y])
      DrawTile(x, y);
    }
  EndDrawing();
}

void DrawTile(int x, int y) {
  // find naboring bombs
  int nearby = 0;
  for (int i = 0; i < TotalBombs; i++) {
    if ((bombPos[i].x == x - 1 || bombPos[i].x == x + 1 || bombPos[i].x == x) &&
        (bombPos[i].y == y - 1 || bombPos[i].y == y + 1 || bombPos[i].y == y))
      nearby++;

    if (bombPos[i].x == x && bombPos[i].y == y) {
      DrawText("x", (x + 0.4f) * squareSize.x, (y + 0.3f) * squareSize.y,
               squareSize.y * 0.6f, RED);
      return;
    }
  }

  if (nearby == 0)
    return; // TODO: Draw empty square
  const char c[] = {nearby + '0', '\0'};
  DrawText(c, (x + 0.4f) * squareSize.x, (y + 0.3f) * squareSize.y,
           squareSize.y * 0.6f, BLACK);
}
