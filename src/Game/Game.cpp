#include "Game.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <memory>


Game::Game() {
    isRunning = false;
    registry = std::make_unique<Registry>();
    Logger::Log("Game constructor called!");
}

Game::~Game() {
    Logger::Log("Game destructor called!");
}

void Game::Initialize() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        Logger::Error("Error initializing SDL.");
        return;
    }
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    windowWidth = displayMode.w;
    windowHeight = displayMode.h;
    window = SDL_CreateWindow(
        nullptr, 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_BORDERLESS
    );
    if (!window) {
        Logger::Error("Error creating SDL window.");
        return;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        Logger::Error("Error creating SDL renderer.");
        return;
    }
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN); 
    isRunning = true;
}

void Game::ProcessInput() {
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
        switch (sdlEvent.type) {
            case SDL_QUIT:
                isRunning = false;
                break; 
            case SDL_KEYDOWN:
                if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
                    isRunning = false;
                }
                break;
        } 
    }
    
}

void Game::Setup() {
    // add the systems that need to be processed in our game 
    registry->AddSystem<MovementSystem>();
    registry->AddSystem<RenderSystem>();

    // create some entities
    Entity tank = registry->CreateEntity();
    tank.AddComponent<TransformComponent>(glm::vec2(10.0, 30.0), glm::vec2(1.0, 1.0), 0.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(40.0, 0.0));
    tank.AddComponent<SpriteComponent>(10, 10);
    
    Entity truck = registry->CreateEntity();
    truck.AddComponent<TransformComponent>(glm::vec2(50.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 50.0));
    truck.AddComponent<SpriteComponent>(10, 50);
}

void Game::Update() {
    // if we are too fast, wait until we reach the MILISECS_PER_FRAME
    int timeToWait = MILISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
    if (timeToWait > 0 && timeToWait <= MILISECS_PER_FRAME) 
        SDL_Delay(timeToWait);

    // the difference in ticks since the last frame, converted to seconds
    double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;

    // store the current frame time
    millisecsPreviousFrame = SDL_GetTicks();
   
    // update the registry to process the entities that are waiting to be added/deleted 
    registry->Update();
    
    // invoke all the systems to update 
    registry->GetSystem<MovementSystem>().Update(deltaTime);
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);

    // invoke all the systems to update 
    registry->GetSystem<RenderSystem>().Update(renderer);

    SDL_RenderPresent(renderer);
}

void Game::Run() {
    Setup();
    while (isRunning) {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::Destroy() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
