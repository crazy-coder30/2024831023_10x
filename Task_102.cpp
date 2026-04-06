#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <bits/stdc++.h>
using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const float STARTING_RADIUS = 20.0f;
const float GROW_AMOUNT = 0.5f;

void drawFilledCircle(SDL_Renderer *renderer, float centerX, float centerY, float radius)
{

    for (int angle = 0; angle <= 360; angle++)
    {
        float angleRad = angle * 3.14159f / 180.0f;
        float edgeX = centerX + radius * cosf(angleRad);
        float edgeY = centerY + radius * sinf(angleRad);
        SDL_RenderLine(renderer, centerX, centerY, edgeX, edgeY);
    }
}

bool circleHitsWall(float centerX, float centerY, float radius)
{

    if (centerX - radius <= 0)
        return true; // hit left wall
    if (centerX + radius >= SCREEN_WIDTH)
        return true; // hit right wall
    if (centerY - radius <= 0)
        return true; // hit top wall
    if (centerY + radius >= SCREEN_HEIGHT)
        return true; // hit bottom wall

    return false; // no collision
}

int main(int argc, char *argv[])
{

    SDL_Window *window = SDL_CreateWindow("Task 102 - Growing Circle",
                                          SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);

    // Circle starts at the center of the window
    float centerX = SCREEN_WIDTH / 2.0f;  // 400
    float centerY = SCREEN_HEIGHT / 2.0f; // 300
    float radius = STARTING_RADIUS;

    bool running = true;
    SDL_Event event;

    while (running)
    {

        // Handle events
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
            if (event.type == SDL_EVENT_KEY_DOWN)
                if (event.key.key == SDLK_ESCAPE)
                    running = false;
        }

        radius += GROW_AMOUNT;

        if (circleHitsWall(centerX, centerY, radius))
        {
            radius = STARTING_RADIUS; // reset back to small
            cout << "Hit the wall! Resetting radius." << endl;
        }

        SDL_SetRenderDrawColor(renderer, 15, 15, 35, 255);
        SDL_RenderClear(renderer);

        float maxRadius = centerX;
        float progress = radius / maxRadius;

        Uint8 red = (Uint8)(progress * 220.0f);
        Uint8 green = (Uint8)(220.0f - progress * 170.0f);
        Uint8 blue = 50;

        SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
        drawFilledCircle(renderer, centerX, centerY, radius);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
