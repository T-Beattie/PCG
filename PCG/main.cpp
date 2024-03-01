#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

using namespace std;

void init_sdl(SDL_Window* window,  SDL_Renderer* renderer) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        std::cout << "Error SDL2 Initialization : " << SDL_GetError();
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        std::cout << "Error SDL2_image Initialization";
    }

    
    if (window == NULL) {
        std::cout << "Error window creation";
    }

    
    if (renderer == NULL) {
        std::cout << "Error renderer creation";
    }
}

SDL_Texture* load_texture(const char* image_path, SDL_Renderer* renderer) {
    SDL_Surface* img = IMG_Load(image_path);

    if (img == NULL) {
        std::cout << "Error loading image: " << IMG_GetError();
    }

    SDL_Texture* image_tex = SDL_CreateTextureFromSurface(renderer, img);
    if (image_tex == NULL) {
        std::cout << "Error creating texture";
    }

    SDL_FreeSurface(img);

    return image_tex;
}


int main(int argc, char* args[])
{
    SDL_Window* window = SDL_CreateWindow("First program", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);    

    init_sdl(window, renderer);

    SDL_Texture* image_tex = load_texture("lettuce.png", renderer);  

    while (true) {
        SDL_Event e;
        if (SDL_WaitEvent(&e)) {
            if (e.type == SDL_QUIT) {
                break;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, image_tex, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(image_tex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}