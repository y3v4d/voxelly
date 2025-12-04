#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "game.hpp"

struct AppState {
    Game* game;
};

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
    if (!SDL_Init(SDL_INIT_VIDEO)){
        return SDL_APP_FAILURE;
    }

    Game* game = new Game();
    game->init();

    AppState* state = new AppState();
    state->game = game;

    *appstate = state;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    AppState* state = static_cast<AppState*>(appstate);

    state->game->loop();
    state->game->window->swapBuffers();

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    AppState* state = static_cast<AppState*>(appstate);
    auto& window = state->game->window;

    window->processEvent(*event);
    if(window->shouldClose()) {
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    AppState* state = static_cast<AppState*>(appstate);

    state->game->shutdown();
    delete state->game;

    SDL_Quit();
}

/*int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    Game game;
    game.init();

    while (game.isRunning()) {
        game.window->pollEvents();
        game.loop();
        game.window->swapBuffers();
    }

    game.shutdown();
    SDL_Quit();

    return 0;
}*/