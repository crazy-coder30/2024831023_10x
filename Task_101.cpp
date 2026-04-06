#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <bits/stdc++.h>
using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int CIRCLE_RADIUS = 200;

void drawFilledCircle(SDL_Renderer *renderer, int centerX, int centerY, int radius)
{

    for (int angle = 0; angle <= 360; angle++)
    {

        float angleInRadians = angle * 3.14159f / 180.0f;

        float edgeX = centerX + radius * cosf(angleInRadians);
        float edgeY = centerY + radius * sinf(angleInRadians);

        SDL_RenderLine(renderer,
                      (float)centerX, (float)centerY,
                       edgeX, edgeY);
    }
}

int main(int argc, char *argv[])
{

    SDL_Window *window = SDL_CreateWindow(
        "Task 101 - Circle",                    // title shown at top of window
        SCREEN_WIDTH,                           // window width
        SCREEN_HEIGHT,                          // window height
        0                                       // no special options
    );

    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);

    int centerX = SCREEN_WIDTH / 2;  
    int centerY = SCREEN_HEIGHT / 2; 

    bool running = true;
    SDL_Event event; 

    while (running)
    {

        while (SDL_PollEvent(&event))
        {

            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }

            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (event.key.key == SDLK_ESCAPE)
                {
                    running = false;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 20, 20, 50, 255);
        SDL_RenderClear(renderer); // fill entire screen with the color above

        SDL_SetRenderDrawColor(renderer, 0, 200, 220, 255); // cyan color
        drawFilledCircle(renderer, centerX, centerY, CIRCLE_RADIUS);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
