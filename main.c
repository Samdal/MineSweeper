#include <raylib.h>
#include <stdlib.h>

// Global variables
static bool mouseInput;
static Vector2 squareSize;
static int TilesX = 20;
static int TilesY = 20;
static int TotalBombs = 40;
typedef struct Bombs {
  int x, y;
} Bomb;
static Bomb *bombPos; // all bombs position
static bool *shown;   // if a tile is hidden
static bool *flags;   // if a tile is flagged

// function declerations
static void ExplodeAllBombs(void);
static void NewGame(void);
static void UpdateGame(void);
static void DrawGame(void);
static void ClearNeighbours(int x, int y);
static bool HasNeighbours(int x, int y);
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
  int boardsize = TilesX * TilesY * sizeof(bool);
  shown = malloc(boardsize);
  flags = malloc(boardsize);
  bombPos = malloc(TotalBombs * sizeof(Bomb));
  if (bombPos == NULL || shown == NULL || flags == NULL)
    return 255; // if memory allocation failed

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  SetConfigFlags(FLAG_VSYNC_HINT);
  InitWindow(800, 800, "Minesweeper in C99");
  NewGame();

  while (!WindowShouldClose()) { // game loop
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
    for (int y = 0; y < TilesY; y++) {
      shown[x * TilesX + y] = false;
      flags[x * TilesX + y] = false;
    }

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

void ExplodeAllBombs(void) {
  for (int i = 0; i < TotalBombs; i++)
    shown[bombPos[i].x * TilesX + bombPos[i].y] = true;
}

void UpdateGame(void) {
  Vector2 mouse = GetMousePosition();
  int mouseX = mouse.x / squareSize.x;
  int mouseY = mouse.y / squareSize.y;
  int mouseTile = mouseX * TilesX + mouseY;
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !flags[mouseTile]) {
    shown[mouseX * TilesX + mouseY] = true;
    if (!HasNeighbours(mouseX, mouseY))
      ClearNeighbours(mouseX, mouseY);
    else
      for (int i = 0; i < TotalBombs; i++)
        if (bombPos[i].x == mouseX && bombPos[i].y == mouseY) {
          ExplodeAllBombs();
          break;
        }
  } else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
    if (!shown[mouseTile])
      flags[mouseTile] = !flags[mouseTile];
}

void ClearNeighbours(int x, int y) {
  for (int nX = -1; nX <= 1; nX++) {
    int xPos = x + nX;
    if (xPos < 0 || xPos >= TilesX)
      continue;
    for (int nY = -1; nY <= 1; nY++) {
      int yPos = y + nY;
      int tile = xPos * TilesX + yPos;
      if (yPos < 0 || yPos >= TilesY || shown[tile] || flags[tile])
        continue;
      shown[tile] = true;
      if (!HasNeighbours(xPos, yPos))
        ClearNeighbours(xPos, yPos); // recurse if tile has no neighbours
    }
  }
}

bool HasNeighbours(int x, int y) {
  for (int i = 0; i < TotalBombs; i++)
    if ((bombPos[i].x == x - 1 || bombPos[i].x == x + 1 || bombPos[i].x == x) &&
        (bombPos[i].y == y - 1 || bombPos[i].y == y + 1 || bombPos[i].y == y))
      return true;
  return false;
}

void DrawGame(void) {
  squareSize.x = (float)GetScreenWidth() / (float)TilesX;
  squareSize.y = (float)GetScreenHeight() / (float)TilesY;
  ClearBackground(GRAY);
  for (int x = 0; x < TilesX; x++)
    for (int y = 0; y < TilesY; y++)
      if (shown[x * TilesX + y])
        DrawTile(x, y);
      else if (flags[x * TilesX + y])
        DrawText("?", (x + 0.35f) * squareSize.x, (y + 0.2f) * squareSize.y,
                 squareSize.y * 0.6f, GREEN);
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
      DrawText("x", (x + 0.35f) * squareSize.x, (y + 0.2f) * squareSize.y,
               squareSize.y * 0.6f, RED);
      return;
    }
  }
  Vector2 rectpos = {squareSize.x * (float)x, squareSize.y * (float)y};
  DrawRectangleV(rectpos, squareSize, RAYWHITE);

  if (nearby == 0)
    return;
  const char c[] = {nearby + '0', '\0'};
  DrawText(c, (x + 0.35f) * squareSize.x, (y + 0.2f) * squareSize.y,
           squareSize.y * 0.6f, BLACK);
}
