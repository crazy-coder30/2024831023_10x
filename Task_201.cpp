#include <SDL3/SDL.h>
#include <iostream>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <string>

const int CELL_SIZE     = 20;
const int GRID_W        = 50;
const int GRID_H        = 50;
const int WINDOW_W      = CELL_SIZE * GRID_W;
const int WINDOW_H      = CELL_SIZE * GRID_H + 50; 
const int MOVE_INTERVAL = 100;

enum class Direction { UP, DOWN, LEFT, RIGHT };


struct Point {
    int x, y;
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};


class Snake {
public:
    std::deque<Point> body;
    Direction dir;
    bool grew;

    Snake() {
        body.push_back({ GRID_W / 2,     GRID_H / 2 });
        body.push_back({ GRID_W / 2 - 1, GRID_H / 2 });
        body.push_back({ GRID_W / 2 - 2, GRID_H / 2 });
        dir  = Direction::RIGHT;
        grew = false;
    }

    Point head() const { return body.front(); }

    void move() {
        Point newHead = head();

        switch (dir) {
            case Direction::UP:    newHead.y -= 1; break;
            case Direction::DOWN:  newHead.y += 1; break;
            case Direction::LEFT:  newHead.x -= 1; break;
            case Direction::RIGHT: newHead.x += 1; break;
        }
        newHead.x=(newHead.x + GRID_W) % GRID_W;
        newHead.y=(newHead.y + GRID_H) % GRID_H;
        body.push_front(newHead);
        if (!grew) body.pop_back();
        grew = false;
    }

    
    bool hitWall() const {
        Point h = head();
        return h.x < 0 || h.x >= GRID_W || h.y < 0 || h.y >= GRID_H;
    }

    
    bool hitSelf() const {
        Point h = head();
        for (size_t i = 1; i < body.size(); ++i)
            if (body[i] == h) return true;
        return false;
    }

    void setDirection(Direction newDir) {
        if (newDir == Direction::UP    && dir == Direction::DOWN)  return;
        if (newDir == Direction::DOWN  && dir == Direction::UP)    return;
        if (newDir == Direction::LEFT  && dir == Direction::RIGHT) return;
        if (newDir == Direction::RIGHT && dir == Direction::LEFT)  return;
        dir = newDir;
    }
};


// Each digit: 5 rows of 3 bits (1 = filled, 0 = empty)
static const bool DIGITS[10][5][3] = {
    // 0
    {{1,1,1},{1,0,1},{1,0,1},{1,0,1},{1,1,1}},
    // 1
    {{0,1,0},{1,1,0},{0,1,0},{0,1,0},{1,1,1}},
    // 2
    {{1,1,1},{0,0,1},{1,1,1},{1,0,0},{1,1,1}},
    // 3
    {{1,1,1},{0,0,1},{0,1,1},{0,0,1},{1,1,1}},
    // 4
    {{1,0,1},{1,0,1},{1,1,1},{0,0,1},{0,0,1}},
    // 5
    {{1,1,1},{1,0,0},{1,1,1},{0,0,1},{1,1,1}},
    // 6
    {{1,1,1},{1,0,0},{1,1,1},{1,0,1},{1,1,1}},
    // 7
    {{1,1,1},{0,0,1},{0,1,0},{0,1,0},{0,1,0}},
    // 8
    {{1,1,1},{1,0,1},{1,1,1},{1,0,1},{1,1,1}},
    // 9
    {{1,1,1},{1,0,1},{1,1,1},{0,0,1},{1,1,1}},
};


void drawDigit(SDL_Renderer* renderer, int digit, int px, int py, int dotSize) {
    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 3; ++col) {
            if (DIGITS[digit][row][col]) {
                SDL_FRect r = {
                    (float)(px + col * dotSize),
                    (float)(py + row * dotSize),
                    (float)(dotSize - 1),
                    (float)(dotSize - 1)
                };
                SDL_RenderFillRect(renderer, &r);
            }
        }
    }
}


int drawNumber(SDL_Renderer* renderer, int number, int px, int py, int dotSize) {
    std::string s = std::to_string(number);
    int digitWidth = 3 * dotSize + 4; // 3 cols + 4px gap
    for (char c : s) {
        drawDigit(renderer, c - '0', px, py, dotSize);
        px += digitWidth;
    }
    return px;
}

// Drawing simple letters using rectangles : just the ones needed : S,C,O,R,E,H,I,G,V,T,A,P,W,L,S


static const bool LETTER_S[5][3] = {{1,1,1},{1,0,0},{1,1,1},{0,0,1},{1,1,1}};
static const bool LETTER_C[5][3] = {{1,1,1},{1,0,0},{1,0,0},{1,0,0},{1,1,1}};
static const bool LETTER_O[5][3] = {{1,1,1},{1,0,1},{1,0,1},{1,0,1},{1,1,1}};
static const bool LETTER_R[5][3] = {{1,1,0},{1,0,1},{1,1,0},{1,0,1},{1,0,1}};
static const bool LETTER_E[5][3] = {{1,1,1},{1,0,0},{1,1,0},{1,0,0},{1,1,1}};
static const bool LETTER_H[5][3] = {{1,0,1},{1,0,1},{1,1,1},{1,0,1},{1,0,1}};
static const bool LETTER_I[5][3] = {{1,1,1},{0,1,0},{0,1,0},{0,1,0},{1,1,1}};
static const bool LETTER_G[5][3] = {{1,1,1},{1,0,0},{1,0,1},{1,0,1},{1,1,1}};
static const bool LETTER_A[5][3] = {{0,1,0},{1,0,1},{1,1,1},{1,0,1},{1,0,1}};
static const bool LETTER_M[5][3] = {{1,0,1},{1,1,1},{1,0,1},{1,0,1},{1,0,1}};
static const bool LETTER_V[5][3] = {{1,0,1},{1,0,1},{1,0,1},{1,0,1},{0,1,0}};
static const bool LETTER_P[5][3] = {{1,1,1},{1,0,1},{1,1,1},{1,0,0},{1,0,0}};
static const bool LETTER_T[5][3] = {{1,1,1},{0,1,0},{0,1,0},{0,1,0},{0,1,0}};
static const bool LETTER_W[5][3] = {{1,0,1},{1,0,1},{1,1,1},{1,1,1},{1,0,1}};
static const bool LETTER_N[5][3] = {{1,0,1},{1,1,1},{1,1,1},{1,0,1},{1,0,1}};
static const bool LETTER_COLON[5][1] = {{0},{1},{0},{1},{0}};

void drawBitmap(SDL_Renderer* renderer, const bool bmp[][3], int rows,
                int px, int py, int dotSize) {
    for (int row = 0; row < rows; ++row)
        for (int col = 0; col < 3; ++col)
            if (bmp[row][col]) {
                SDL_FRect r = {
                    (float)(px + col * dotSize),
                    (float)(py + row * dotSize),
                    (float)(dotSize - 1),
                    (float)(dotSize - 1)
                };
                SDL_RenderFillRect(renderer, &r);
            }
}


void drawColon(SDL_Renderer* renderer, int px, int py, int dotSize) {
    SDL_FRect top = { (float)(px + dotSize/2), (float)(py + 1*dotSize), (float)(dotSize-1), (float)(dotSize-1) };
    SDL_FRect bot = { (float)(px + dotSize/2), (float)(py + 3*dotSize), (float)(dotSize-1), (float)(dotSize-1) };
    SDL_RenderFillRect(renderer, &top);
    SDL_RenderFillRect(renderer, &bot);
}


int drawLetter(SDL_Renderer* renderer, char c, int px, int py, int dotSize) {
    int w = 3 * dotSize + 4;
    switch (c) {
        case 'S': drawBitmap(renderer, LETTER_S, 5, px, py, dotSize); break;
        case 'C': drawBitmap(renderer, LETTER_C, 5, px, py, dotSize); break;
        case 'O': drawBitmap(renderer, LETTER_O, 5, px, py, dotSize); break;
        case 'R': drawBitmap(renderer, LETTER_R, 5, px, py, dotSize); break;
        case 'E': drawBitmap(renderer, LETTER_E, 5, px, py, dotSize); break;
        case 'H': drawBitmap(renderer, LETTER_H, 5, px, py, dotSize); break;
        case 'I': drawBitmap(renderer, LETTER_I, 5, px, py, dotSize); break;
        case 'G': drawBitmap(renderer, LETTER_G, 5, px, py, dotSize); break;
        case 'A': drawBitmap(renderer, LETTER_A, 5, px, py, dotSize); break;
        case 'M': drawBitmap(renderer, LETTER_M, 5, px, py, dotSize); break;
        case 'V': drawBitmap(renderer, LETTER_V, 5, px, py, dotSize); break;
        case 'P': drawBitmap(renderer, LETTER_P, 5, px, py, dotSize); break;
        case 'T': drawBitmap(renderer, LETTER_T, 5, px, py, dotSize); break;
        case 'W': drawBitmap(renderer, LETTER_W, 5, px, py, dotSize); break;
        case 'N': drawBitmap(renderer, LETTER_N, 5, px, py, dotSize); break;
        case ':': drawColon(renderer, px, py, dotSize); w = dotSize + 4;  break;
        default: break;
    }
    return px + w;
}


int drawString(SDL_Renderer* renderer, const char* str,
               int px, int py, int dotSize) {
    while (*str) {
        px = drawLetter(renderer, *str, px, py, dotSize);
        ++str;
    }
    return px;
}


//  GAME CLASS

class Game {
public:
    SDL_Window*   window   = nullptr;
    SDL_Renderer* renderer = nullptr;

    Snake  snake;
    Point  food;
    int    score     = 0;
    int    highScore = 0;   
    bool   running   = false;
    bool   gameOver  = false;

    Uint64 lastMoveTime = 0;

    
    bool init() {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
            return false;
        }

        window = SDL_CreateWindow("Snake Game", WINDOW_W, WINDOW_H, 0);
        if (!window) {
            std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
            return false;
        }

        renderer = SDL_CreateRenderer(window, nullptr);
        if (!renderer) {
            std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
            return false;
        }

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        srand(static_cast<unsigned>(time(nullptr)));
        spawnFood();

        running      = true;
        gameOver     = false;
        lastMoveTime = SDL_GetTicks();
        return true;
    }

    // Food
    void spawnFood() {
        Point candidate;
        do {
            candidate.x = rand() % GRID_W;
            candidate.y = rand() % GRID_H;
        } while (isCellOccupied(candidate));
        food = candidate;
    }

    bool isCellOccupied(const Point& p) const {
        for (const auto& seg : snake.body)
            if (seg == p) return true;
        return false;
    }

   
    void handleInput() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }

            if (event.type == SDL_EVENT_KEY_DOWN) {
                SDL_Keycode key = event.key.key;

                if (gameOver) {
                    if (key == SDLK_R) restartGame();
                    if (key == SDLK_ESCAPE) running = false;
                    return;
                }

                switch (key) {
                    case SDLK_UP:     case SDLK_W: snake.setDirection(Direction::UP);    break;
                    case SDLK_DOWN:   case SDLK_S: snake.setDirection(Direction::DOWN);  break;
                    case SDLK_LEFT:   case SDLK_A: snake.setDirection(Direction::LEFT);  break;
                    case SDLK_RIGHT:  case SDLK_D: snake.setDirection(Direction::RIGHT); break;
                    case SDLK_ESCAPE: running = false; break;
                }
            }
        }
    }

    
    void update() {
        if (gameOver) return;

        Uint64 now = SDL_GetTicks();
        if (now - lastMoveTime < MOVE_INTERVAL) return;
        lastMoveTime = now;

        snake.move();


        // SELF COLLISION 
        if (snake.hitSelf()) {
            triggerGameOver();
            return;
        }

        // EAT FOOD
        if (snake.head() == food) {
            snake.grew = true;
            score++;
            spawnFood();
        }
    }

    
    void triggerGameOver() {
        gameOver = true;
        if (score > highScore)
            highScore = score;
        
        std::string t = "Snake | GAME OVER | Score: " + std::to_string(score)
                      + " | Best: " + std::to_string(highScore);
        SDL_SetWindowTitle(window, t.c_str());
    }

   
    void drawCell(int gx, int gy) {
        SDL_FRect rect = {
            (float)(gx * CELL_SIZE + 1),
            (float)(gy * CELL_SIZE + 1),
            (float)(CELL_SIZE - 2),
            (float)(CELL_SIZE - 2)
        };
        SDL_RenderFillRect(renderer, &rect);
    }

    
    void renderScoreBar() {
        int barY = GRID_H * CELL_SIZE;  

        
        SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
        SDL_FRect bg = { 0, (float)barY, (float)WINDOW_W, 50.0f };
        SDL_RenderFillRect(renderer, &bg);

       
        SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
        SDL_RenderLine(renderer, 0, barY, WINDOW_W, barY);

        int dotSize = 5;  
        int textY   = barY + 10;

        // SCORE label + number
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);   // gold
        int x = drawString(renderer, "SCORE:", 10, textY, dotSize);
        x += 6;
        drawNumber(renderer, score, x, textY, dotSize);

        // HI label + number
        SDL_SetRenderDrawColor(renderer, 100, 220, 255, 255);  // cyan
        int hiX = WINDOW_W / 2 + 10;
        int x2  = drawString(renderer, "HI:", hiX, textY, dotSize);
        x2 += 6;
        drawNumber(renderer, highScore, x2, textY, dotSize);
    }

    // Game Over Overlay
    void renderGameOver() {
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 170);
        SDL_FRect overlay = { 0, 0, (float)WINDOW_W, (float)(GRID_H * CELL_SIZE) };
        SDL_RenderFillRect(renderer, &overlay);

        
        float bx = WINDOW_W / 2.0f - 130.0f;
        float by = GRID_H * CELL_SIZE / 2.0f - 80.0f;
        float bw = 260.0f;
        float bh = 160.0f;

        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
        SDL_FRect shadow = { bx + 4, by + 4, bw, bh };
        SDL_RenderFillRect(renderer, &shadow);

       
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_FRect box = { bx, by, bw, bh };
        SDL_RenderFillRect(renderer, &box);

        
        SDL_SetRenderDrawColor(renderer, 220, 50, 50, 255);
        SDL_FRect borders[4] = {
            { bx,          by,          bw,   2 },     // top
            { bx,          by + bh - 2, bw,   2 },     // bottom
            { bx,          by,          2,    bh },    // left
            { bx + bw - 2, by,          2,    bh }     // right
        };
        for (auto& b : borders) SDL_RenderFillRect(renderer, &b);

        int dotSize = 5;

        
        SDL_SetRenderDrawColor(renderer, 220, 50, 50, 255);   // red
        
        int goX = (int)bx + 14;
        int goY = (int)by + 14;
        goX = drawString(renderer, "GAME", goX, goY, dotSize);
        goX += 8;
        drawString(renderer, "OVER", goX, goY, dotSize);

        
        SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
        SDL_RenderLine(renderer,
            (int)bx + 10, (int)by + 46,
            (int)bx + (int)bw - 10, (int)by + 46);

        
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);   // gold
        int sx = (int)bx + 14;
        int sy = (int)by + 55;
        int nx = drawString(renderer, "SCORE:", sx, sy, dotSize);
        nx += 6;
        drawNumber(renderer, score, nx, sy, dotSize);

        
        SDL_SetRenderDrawColor(renderer, 100, 220, 255, 255);  // cyan
        int hy = (int)by + 82;
        int hx = drawString(renderer, "HI:", (int)bx + 14, hy, dotSize);
        hx += 6;
        drawNumber(renderer, highScore, hx, hy, dotSize);

       
        SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
        SDL_RenderLine(renderer,
            (int)bx + 10, (int)by + 108,
            (int)bx + (int)bw - 10, (int)by + 108);

        
        SDL_SetRenderDrawColor(renderer, 160, 160, 160, 255);  // grey
        int px2 = (int)bx + 14;
        int py2 = (int)by + 118;
        px2 = drawString(renderer, "PRESS", px2, py2, dotSize);
        px2 += 8;
        drawString(renderer, "R", px2, py2, dotSize);
    }

    
    void render() {
       
        SDL_SetRenderDrawColor(renderer, 15, 15, 15, 255);
        SDL_RenderClear(renderer);

        
        SDL_SetRenderDrawColor(renderer, 28, 28, 28, 255);
        for (int x = 0; x <= GRID_W; ++x)
            SDL_RenderLine(renderer, x*CELL_SIZE, 0, x*CELL_SIZE, GRID_H*CELL_SIZE);
        for (int y = 0; y <= GRID_H; ++y)
            SDL_RenderLine(renderer, 0, y*CELL_SIZE, WINDOW_W, y*CELL_SIZE);

        // Food
        SDL_SetRenderDrawColor(renderer, 220, 50, 50, 255);
        drawCell(food.x, food.y);

        // Snake : bright head to dark tail
        int len = (int)snake.body.size();
        for (int i = 0; i < len; ++i) {
            float t     = (len <= 1) ? 1.0f : 1.0f - (float)i / (float)(len - 1);
            Uint8 green = (Uint8)(80 + t * 175);
            Uint8 red   = (i == 0) ? 50 : 0;
            Uint8 blue  = (i == 0) ? 50 : 0;
            SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
            drawCell(snake.body[i].x, snake.body[i].y);
        }

        
        renderScoreBar();

       
        if (gameOver) renderGameOver();

        SDL_RenderPresent(renderer);
    }

    
    void restartGame() {
        snake    = Snake();
        score    = 0;
        gameOver = false;
        lastMoveTime = SDL_GetTicks();
        spawnFood();
        SDL_SetWindowTitle(window, "Snake Game");
    }

   
    void run() {
        while (running) {
            handleInput();
            update();
            render();
            SDL_Delay(8);
        }
    }

   
    void cleanup() {
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window)   SDL_DestroyWindow(window);
        SDL_Quit();
    }
};


int main(int argc, char* argv[]) {
    Game game;

    if (!game.init()) {
        game.cleanup();
        return 1;
    }

    game.run();
    game.cleanup();
    return 0;
}