#ifndef GAME_H
#define GAME_H

#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <memory>

const int FPS = 60;
const int MILISECS_PER_FRAME = 1000 / FPS;

class Game {
    private:
        bool isRunning; 
        int millisecsPreviousFrame = 0;
        SDL_Window* window;
        SDL_Renderer* renderer;
        
        std::unique_ptr<Registry> registry;
        std::unique_ptr<AssetStore> assetStore;

    public:
        Game();
        ~Game();
        void Initialize();
        void Run();
        void ProcessInput();
        void Setup();
        void LoadLevel(int level);
        void Update();
        void Render();
        void Destroy();

        int windowWidth;
        int windowHeight;
};

#endif
