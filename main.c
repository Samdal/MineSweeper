#include <raylib.h>
#include <stdlib.h>

// Global variables
#define DEFAULT_BOARD_X 30
#define DEFAULT_BOARD_Y 20
#define DEFAULT_BOMBS 70
static Vector2 squareSize;
static int TilesX = DEFAULT_BOARD_X;
static int TilesY = DEFAULT_BOARD_Y;
static int TotalBombs = DEFAULT_BOMBS;
typedef struct Bombs {
  int x, y;
} Bomb;
static Bomb *bombPos; // all bombs position
static bool **shown;  // if a tile is hidden
static bool **flags;  // if a tile is flagged
static bool gameOver = false;

// function declerations
static void ExplodeAllBombs(void);
static void NewGame(void);
static void UpdateGame(void);
static void DrawGame(void);
static void ClearNeighbours(const int *x, const int *y);
static bool HasNeighbours(const int *x, const int *y);
static void DrawTile(const int *x, const int *y);

int main(int argc, char *argv[]) {
  // Set game area and bomb amount with command line args
  switch (argc) {
  case 4:
    TotalBombs = strtol(argv[3], NULL, 0);
  case 3:
    TilesY = strtol(argv[2], NULL, 0);
    if (TilesY <= 0)
      TilesY = DEFAULT_BOARD_Y;
  case 2:
    TilesX = strtol(argv[1], NULL, 0);
    if (TilesX <= 0)
      TilesX = DEFAULT_BOARD_X;
    if ((argc == 4) && (TotalBombs <= 0 || TotalBombs >= TilesX * TilesY))
      TotalBombs = (TilesX * TilesY) / 7;
  }
  shown = malloc(TilesX * sizeof(bool *));
  flags = malloc(TilesX * sizeof(bool *));
  shown[0] = malloc(TilesX * TilesY * sizeof(bool));
  flags[0] = malloc(TilesX * TilesY * sizeof(bool));
  for (int i = 1; i < TilesX; i++) {
    shown[i] = shown[0] + i * TilesY;
    flags[i] = flags[0] + i * TilesY;
  }
  bombPos = malloc(TotalBombs * sizeof(Bomb));
  if (bombPos == NULL || shown == NULL || shown[0] == NULL || flags == NULL ||
      flags[0] == NULL)
    return 255; // memory allocation failed

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
  free(shown[0]);
  free(flags[0]);
  free(shown);
  free(flags);
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
      shown[x][y] = false;
      flags[x][y] = false;
    }
  gameOver = false;

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
    shown[bombPos[i].x][bombPos[i].y] = true;
  gameOver = true;
}

void UpdateGame(void) {
  const Vector2 mouse = GetMousePosition();
  const int mouseX = mouse.x / squareSize.x;
  const int mouseY = mouse.y / squareSize.y;
  if (gameOver) {
    if (IsKeyPressed(KEY_SPACE)) {
      gameOver = false;
      NewGame();
    }
  } else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
             !flags[mouseX][mouseY]) {
    shown[mouseX][mouseY] = true;
    if (!HasNeighbours(&mouseX, &mouseY))
      ClearNeighbours(&mouseX, &mouseY);
    else
      for (int i = 0; i < TotalBombs; i++)
        if (bombPos[i].x == mouseX && bombPos[i].y == mouseY) {
          ExplodeAllBombs();
          break;
        }

    // calulate if game has ended
    int count = 0;
    for (int x = 0; x < TilesX; x++)
      for (int y = 0; y < TilesY; y++)
        if (!shown[x][y])
          count++;
    if (count == TotalBombs)
      gameOver = true;

  } else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
    if (!shown[mouseX][mouseY])
      flags[mouseX][mouseY] = !flags[mouseX][mouseY];
}

void ClearNeighbours(const int *x, const int *y) {
  for (int nX = -1; nX <= 1; nX++) {
    const int xPos = *x + nX;
    if (xPos < 0 || xPos >= TilesX)
      continue;
    for (int nY = -1; nY <= 1; nY++) {
      const int yPos = *y + nY;
      if (yPos < 0 || yPos >= TilesY || shown[xPos][yPos] || flags[xPos][yPos])
        continue;
      shown[xPos][yPos] = true;
      if (!HasNeighbours(&xPos, &yPos))
        ClearNeighbours(&xPos, &yPos); // recurse if tile has no neighbours
    }
  }
}

bool HasNeighbours(const int *x, const int *y) {
  for (int i = 0; i < TotalBombs; i++)
    if ((bombPos[i].x == *x - 1 || bombPos[i].x == *x + 1 ||
         bombPos[i].x == *x) &&
        (bombPos[i].y == *y - 1 || bombPos[i].y == *y + 1 ||
         bombPos[i].y == *y))
      return true;
  return false;
}

void DrawGame(void) {
  squareSize.x = (float)GetScreenWidth() / (float)TilesX;
  squareSize.y = (float)GetScreenHeight() / (float)TilesY;
  ClearBackground(GRAY);
  for (int x = 0; x < TilesX; x++)
    for (int y = 0; y < TilesY; y++)
      if (shown[x][y])
        DrawTile(&x, &y);
      else if (flags[x][y])
        DrawText("?", (x + 0.35f) * squareSize.x, (y + 0.2f) * squareSize.y,
                 squareSize.y * 0.6f, GREEN);

  if (gameOver) {
    const float xsize = TilesX * 0.2f * squareSize.x;
    const float ysize = TilesY * 0.2f * squareSize.y;
    const float fsize = TilesX * 0.1f * squareSize.x;
    DrawText("GAME OVER!", xsize, ysize, fsize, GOLD);
    DrawText("(press SPACE to replay)", xsize, (TilesY * 0.5f) * squareSize.y,
             fsize * 0.5f, GOLD);
  }
  EndDrawing();
}

void DrawTile(const int *x, const int *y) {
  const float xsize = (*x + 0.35f) * squareSize.x;
  const float ysize = (*y + 0.2f) * squareSize.y;
  const float fsize = 0.55f * squareSize.y;
  // find naboring bombs
  int nearby = 0;
  for (int i = 0; i < TotalBombs; i++) {
    if ((bombPos[i].x == *x - 1 || bombPos[i].x == *x + 1 ||
         bombPos[i].x == *x) &&
        (bombPos[i].y == *y - 1 || bombPos[i].y == *y + 1 ||
         bombPos[i].y == *y))
      nearby++;

    if (bombPos[i].x == *x && bombPos[i].y == *y) {
      DrawText("x", xsize, ysize, fsize, RED);
      return;
    }
  }
  const Vector2 rectpos = {squareSize.x * (float)*x, squareSize.y * (float)*y};
  DrawRectangleV(rectpos, squareSize, RAYWHITE);

  if (nearby == 0)
    return;
  const char c[] = {nearby + '0', '\0'};
  DrawText(c, xsize, ysize, fsize, BLACK);
}
