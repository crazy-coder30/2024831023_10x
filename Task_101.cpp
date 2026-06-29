#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <string>

const int CELL_SIZE     = 20;
const int GRID_W        = 40;
const int GRID_H        = 40;
const int WINDOW_W      = CELL_SIZE * GRID_W;
const int WINDOW_H      = CELL_SIZE * GRID_H + 50; // +50 for score bar
const int MOVE_INTERVAL = 100;

enum class Direction { UP, DOWN, LEFT, RIGHT };

// ============================================================
//  POINT STRUCT
// ============================================================
struct Point {
    int x, y;
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

// ============================================================
//  SNAKE CLASS
// ============================================================
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

// ============================================================
//  GAME CLASS
// ============================================================
class Game {
public:
    SDL_Window*   window   = nullptr;
    SDL_Renderer* renderer = nullptr;
    TTF_Font*     font     = nullptr;  // one font for everything

    Snake  snake;
    Point  food;
    int    score     = 0;
    int    highScore = 0;
    bool   running   = false;
    bool   gameOver  = false;

    Uint64 lastMoveTime = 0;

    // ----------------------------------------------------------
    //  INIT
    // ----------------------------------------------------------
    bool init() {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
            return false;
        }

        // Initialize SDL3_ttf
        if (!TTF_Init()) {
            std::cerr << "TTF_Init failed: " << SDL_GetError() << "\n";
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

        // Load font — adjust path for your system
        // Ubuntu/Debian: /usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf
        // Windows:       C:/Windows/Fonts/arialbd.ttf
        font = TTF_OpenFont("C:/Windows/Fonts/arialbd.ttf", 24);
        if (!font) {
            std::cerr << "TTF_OpenFont failed: " << SDL_GetError() << "\n";
            return false;
        }

        srand(static_cast<unsigned>(time(nullptr)));
        spawnFood();

        running      = true;
        gameOver     = false;
        lastMoveTime = SDL_GetTicks();
        return true;
    }

    // ----------------------------------------------------------
    //  DRAW TEXT HELPER
    //  This ONE function replaces all 120 lines of digit/letter renderer
    // ----------------------------------------------------------
    void drawText(const std::string& text, int x, int y, SDL_Color color) {
        // Render text to a surface
        SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), 0, color);
        if (!surface) return;

        // Convert surface to texture
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);   // SDL3: SDL_FreeSurface → SDL_DestroySurface
        if (!texture) return;

        // Get size and draw
        float w, h;
        SDL_GetTextureSize(texture, &w, &h);
        SDL_FRect dst = { (float)x, (float)y, w, h };
        SDL_RenderTexture(renderer, texture, nullptr, &dst);  // SDL3: SDL_RenderCopy → SDL_RenderTexture
        SDL_DestroyTexture(texture);
    }

    // ----------------------------------------------------------
    //  FOOD
    // ----------------------------------------------------------
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

    // ----------------------------------------------------------
    //  INPUT
    // ----------------------------------------------------------
    void handleInput() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }

            if (event.type == SDL_EVENT_KEY_DOWN) {
                SDL_Keycode key = event.key.key;

                if (gameOver) {
                    if (key == SDLK_R)      restartGame();
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

    // ----------------------------------------------------------
    //  UPDATE
    // ----------------------------------------------------------
    void update() {
        if (gameOver) return;

        Uint64 now = SDL_GetTicks();
        if (now - lastMoveTime < MOVE_INTERVAL) return;
        lastMoveTime = now;

        snake.move();

        if (snake.hitWall()) { triggerGameOver(); return; }
        if (snake.hitSelf()) { triggerGameOver(); return; }

        if (snake.head() == food) {
            snake.grew = true;
            score++;
            spawnFood();
        }
    }

    // ----------------------------------------------------------
    //  TRIGGER GAME OVER
    // ----------------------------------------------------------
    void triggerGameOver() {
        gameOver = true;
        if (score > highScore)
            highScore = score;
    }

    // ----------------------------------------------------------
    //  DRAW CELL
    // ----------------------------------------------------------
    void drawCell(int gx, int gy) {
        SDL_FRect rect = {
            (float)(gx * CELL_SIZE + 1),
            (float)(gy * CELL_SIZE + 1),
            (float)(CELL_SIZE - 2),
            (float)(CELL_SIZE - 2)
        };
        SDL_RenderFillRect(renderer, &rect);
    }

    // ----------------------------------------------------------
    //  SCORE BAR
    // ----------------------------------------------------------
    void renderScoreBar() {
        int barY = GRID_H * CELL_SIZE;

        // Background
        SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
        SDL_FRect bg = { 0, (float)barY, (float)WINDOW_W, 50.0f };
        SDL_RenderFillRect(renderer, &bg);

        // Separator line
        SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
        SDL_RenderLine(renderer, 0, barY, WINDOW_W, barY);

        // Score text — gold color
        SDL_Color gold = { 255, 215, 0, 255 };
        drawText("SCORE: " + std::to_string(score), 10, barY + 12, gold);

        // High score text — cyan color
        SDL_Color cyan = { 100, 220, 255, 255 };
        drawText("BEST: " + std::to_string(highScore), WINDOW_W / 2 + 10, barY + 12, cyan);
    }

    // ----------------------------------------------------------
    //  GAME OVER SCREEN
    // ----------------------------------------------------------
    void renderGameOver() {
        // Dark overlay
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 170);
        SDL_FRect overlay = { 0, 0, (float)WINDOW_W, (float)(GRID_H * CELL_SIZE) };
        SDL_RenderFillRect(renderer, &overlay);

        // Centre box
        float bx = WINDOW_W / 2.0f - 130.0f;
        float by = GRID_H * CELL_SIZE / 2.0f - 80.0f;

        // Box shadow
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
        SDL_FRect shadow = { bx + 4, by + 4, 260.0f, 160.0f };
        SDL_RenderFillRect(renderer, &shadow);

        // Box fill
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_FRect box = { bx, by, 260.0f, 160.0f };
        SDL_RenderFillRect(renderer, &box);

        // Box border — red
        SDL_SetRenderDrawColor(renderer, 220, 50, 50, 255);
        SDL_FRect borders[4] = {
            { bx,           by,            260.0f, 2.0f },
            { bx,           by + 158.0f,   260.0f, 2.0f },
            { bx,           by,            2.0f,   160.0f },
            { bx + 258.0f,  by,            2.0f,   160.0f }
        };
        for (auto& b : borders) SDL_RenderFillRect(renderer, &b);

        // Text — all using drawText() now
        SDL_Color red    = { 220,  50,  50, 255 };
        SDL_Color gold   = { 255, 215,   0, 255 };
        SDL_Color cyan   = { 100, 220, 255, 255 };
        SDL_Color grey   = { 160, 160, 160, 255 };

        drawText("GAME OVER",                        (int)bx + 50, (int)by + 14,  red);
        drawText("SCORE: " + std::to_string(score),  (int)bx + 14, (int)by + 58,  gold);
        drawText("BEST:  " + std::to_string(highScore), (int)bx + 14, (int)by + 88,  cyan);
        drawText("PRESS R TO RESTART",               (int)bx + 14, (int)by + 124, grey);
    }

    // ----------------------------------------------------------
    //  RENDER
    // ----------------------------------------------------------
    void render() {
        // Background
        SDL_SetRenderDrawColor(renderer, 15, 15, 15, 255);
        SDL_RenderClear(renderer);

        // Grid lines
        SDL_SetRenderDrawColor(renderer, 28, 28, 28, 255);
        for (int x = 0; x <= GRID_W; ++x)
            SDL_RenderLine(renderer, x*CELL_SIZE, 0, x*CELL_SIZE, GRID_H*CELL_SIZE);
        for (int y = 0; y <= GRID_H; ++y)
            SDL_RenderLine(renderer, 0, y*CELL_SIZE, WINDOW_W, y*CELL_SIZE);

        // Food — red
        SDL_SetRenderDrawColor(renderer, 220, 50, 50, 255);
        drawCell(food.x, food.y);

        // Snake — gradient from bright head to dark tail
        int len = (int)snake.body.size();
        for (int i = 0; i < len; ++i) {
            float t     = (len <= 1) ? 1.0f : 1.0f - (float)i / (float)(len - 1);
            Uint8 green = (Uint8)(80 + t * 175);
            Uint8 red   = (i == 0) ? 50 : 0;
            Uint8 blue  = (i == 0) ? 50 : 0;
            SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
            drawCell(snake.body[i].x, snake.body[i].y);
        }

        // Score bar at bottom
        renderScoreBar();

        // Game over overlay on top
        if (gameOver) renderGameOver();

        SDL_RenderPresent(renderer);
    }

    // ----------------------------------------------------------
    //  RESTART
    // ----------------------------------------------------------
    void restartGame() {
        snake        = Snake();
        score        = 0;
        gameOver     = false;
        lastMoveTime = SDL_GetTicks();
        spawnFood();
    }

    // ----------------------------------------------------------
    //  MAIN LOOP
    // ----------------------------------------------------------
    void run() {
        while (running) {
            handleInput();
            update();
            render();
            SDL_Delay(8);
        }
    }

    // ----------------------------------------------------------
    //  CLEANUP
    // ----------------------------------------------------------
    void cleanup() {
        if (font)     TTF_CloseFont(font);
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window)   SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
    }
};

// ============================================================
//  MAIN
// ============================================================
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