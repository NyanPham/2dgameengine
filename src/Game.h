#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>

const int FPS = 60;
const int MILISECS_PER_FRAME = 1000 / FPS;

class Game {
    private:
        bool isRunning; 
        int millisecsPreviousFrame;
        SDL_Window* window;
        SDL_Renderer* renderer;

    public:
        Game();
        ~Game();
        void Initialize();
        void Run();
        void ProcessInput();
        void Setup();
        void Update();
        void Render();
        void Destroy();

        int windowWidth;
        int windowHeight;
};

#endif
