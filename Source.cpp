#include <SDL.h>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

#include "vec2.h"
#include "Element.h"
#include "Solver.h"

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 800;

SDL_Color get_random_color()
{
    const float t = SDL_GetTicks();

    const float r = sin(t);
    const float g = sin(t + 0.33f * 2.0f * M_PI);
    const float b = sin(t + 0.66f * 2.0f * M_PI);

    return { static_cast<uint8_t>(255.0f * r * r),
            static_cast<uint8_t>(255.0f * g * g),
            static_cast<uint8_t>(255.0f * b * b) };
}


int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("What a magnificent physics engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        SDL_Log("Window could not be created: %s", SDL_GetError());
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        SDL_Log("Renderer could not be created: %s", SDL_GetError());
        return 1;
    }

    Solver solver = Solver(renderer);
    solver.set_sub_steps(1);
    solver.set_gravity({ 0,800 });
    solver.add_constraint(
        Circle({400, 400 }, 300 )
    );
    solver.add_constraint(
        Circle({ 600, 400 }, 300)
    );
    
    bool quit = false;
    SDL_Event event;

    Uint32 startTime = SDL_GetTicks();
    Uint32 elapsedTime = 0;
    const Uint32 interval = 200; 

    int mousex = (float)WINDOW_WIDTH / 2;
    int mousey = (float)WINDOW_HEIGHT / 2;
	bool mousepressed = false;
     
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {

            case SDL_QUIT:
                quit = true;
                break;

            case SDL_MOUSEMOTION:
                mousex = event.button.x;
                mousey = event.button.y;
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    //solver.add_object({ (float)mousex, (float)mousey }, 10, get_random_color());
                    mousepressed = true;
                }
                break;

            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    mousepressed = false;
                }
                break;
            }
        }

        if (mousepressed) {
            elapsedTime = SDL_GetTicks() - startTime;
            if (elapsedTime >= interval) {
                for (int i = -20; i <= 20; i+=10) {
                    solver.add_object({ (float)mousex+i, (float)mousey }, 5, get_random_color());
                }
                solver.add_object({ (float)mousex, (float)mousey }, 5, get_random_color());
                startTime = SDL_GetTicks();
            }
        }

        // Update //
		solver.update();

        // Render //
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        solver.render();

        SDL_RenderPresent(renderer);
    }   

    // Quit SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}