#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <bits/stdc++.h>
using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int RADIUS = 40;
const float MOVE_SPEED = 4.0f;

void drawFilledCircle(SDL_Renderer *renderer, float cx, float cy, float r)
{
    for (int angle = 0; angle <= 360; angle++)
    {
        float rad = angle * 3.14159f / 180.0f;
        float ex = cx + r * cosf(rad);
        float ey = cy + r * sinf(rad);
        SDL_RenderLine(renderer, cx, cy, ex, ey);
    }
}

void drawCircleOutline(SDL_Renderer *renderer, float cx, float cy, float r)
{
    for (int angle = 0; angle <= 360; angle++)
    {
        float rad = angle * 3.14159f / 180.0f;

        SDL_RenderPoint(renderer, cx + r * cosf(rad), cy + r * sinf(rad));
    }
}

bool areCirclesColliding(float x1, float y1, float x2, float y2, float r1, float r2)
{

    float dx = x2 - x1;
    float dy = y2 - y1;
    float distance = sqrtf(dx * dx + dy * dy);

    return distance <= (r1 + r2);
}

int main(int argc, char *argv[])
{

    SDL_Window *window = SDL_CreateWindow("Task 103 - Two Circles",
                                          SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);

    // --- Circle 1: AUTO (moves by itself) ---
    float autoX = (float)RADIUS;
    float autoY = SCREEN_HEIGHT / 2.0f;
    float autoSpeedX = 3.0f;

    // --- Circle 2: PLAYER ---
    float playerX = SCREEN_WIDTH / 2.0f;
    float playerY = (float)RADIUS;

    bool colliding = false;
    int flashTimer = 0;
    float ringSize = 0.0f;

    bool running = true;
    SDL_Event event;

    while (running)
    {

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
            if (event.type == SDL_EVENT_KEY_DOWN)
                if (event.key.key == SDLK_ESCAPE)
                    running = false;
        }

        const bool *keys = SDL_GetKeyboardState(nullptr);

        if (keys[SDL_SCANCODE_UP])
            playerY -= MOVE_SPEED;
        if (keys[SDL_SCANCODE_DOWN])
            playerY += MOVE_SPEED;
        if (keys[SDL_SCANCODE_LEFT])
            playerX -= MOVE_SPEED;
        if (keys[SDL_SCANCODE_RIGHT])
            playerX += MOVE_SPEED;

        if (playerX - RADIUS < 0)
            playerX = RADIUS;
        if (playerX + RADIUS > SCREEN_WIDTH)
            playerX = SCREEN_WIDTH - RADIUS;
        if (playerY - RADIUS < 0)
            playerY = RADIUS;
        if (playerY + RADIUS > SCREEN_HEIGHT)
            playerY = SCREEN_HEIGHT - RADIUS;

        autoX += autoSpeedX;

        if (autoX - RADIUS <= 0)
        {
            autoX = RADIUS;
            autoSpeedX = 3.0f;
        }

        if (autoX + RADIUS >= SCREEN_WIDTH)
        {
            autoX = SCREEN_WIDTH - RADIUS;
            autoSpeedX = -3.0f;
        }

        colliding = areCirclesColliding(autoX, autoY, playerX, playerY,
                                        RADIUS, RADIUS);

        if (colliding)
        {
            flashTimer = 20;
            ringSize = 0.0f;
        }

        if (flashTimer > 0)
        {
            flashTimer--;
            ringSize += 3.0f;
        }

        SDL_SetRenderDrawColor(renderer, 10, 10, 25, 255);
        SDL_RenderClear(renderer);

        if (flashTimer > 0)
        {

            float fadeFraction = flashTimer / 20.0f;
            Uint8 alpha = (Uint8)(fadeFraction * 150.0f);

            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 255, 40, 40, alpha);

            SDL_FRect fullScreen = {0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT};
            SDL_RenderFillRect(renderer, &fullScreen);

            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }

        if (colliding)
            SDL_SetRenderDrawColor(renderer, 255, 80, 80, 255);
        else
            SDL_SetRenderDrawColor(renderer, 60, 140, 255, 255);

        drawFilledCircle(renderer, autoX, autoY, RADIUS);

        if (colliding)
            SDL_SetRenderDrawColor(renderer, 255, 220, 50, 255);
        else
            SDL_SetRenderDrawColor(renderer, 60, 220, 90, 255);

        drawFilledCircle(renderer, playerX, playerY, RADIUS);

        if (flashTimer > 0)
        {
            float midX = (autoX + playerX) / 2.0f;
            float midY = (autoY + playerY) / 2.0f;

            SDL_SetRenderDrawColor(renderer, 255, 255, 100, 255);
            drawCircleOutline(renderer, midX, midY, RADIUS + ringSize);
            drawCircleOutline(renderer, midX, midY, RADIUS + ringSize + 15);
            drawCircleOutline(renderer, midX, midY, RADIUS + ringSize + 30);
        }

        SDL_FRect dot = {12, 12, 18, 18};
        if (colliding)
            SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
        else
            SDL_SetRenderDrawColor(renderer, 50, 220, 80, 255);
        SDL_RenderFillRect(renderer, &dot);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
