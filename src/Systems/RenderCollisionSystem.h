#ifndef RENDERCOLLISIONSYTEM_H
#define RENDERCOLLISIONSYTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/BoxColliderComponent.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

class RenderCollisionSystem: public System {
    public:
        RenderCollisionSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<BoxColliderComponent>();
        }

        void Update(SDL_Renderer* renderer) {
            for (auto entity: GetSystemEntities()) {
                const auto transform = entity.GetComponent<TransformComponent>();
                const auto collider = entity.GetComponent<BoxColliderComponent>();

                SDL_Rect colliderRect = {
                    static_cast<int>(transform.position.x + collider.offset.x),
                    static_cast<int>(transform.position.y + collider.offset.y),
                    static_cast<int>(collider.width),
                    static_cast<int>(collider.height)
                };
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderDrawRect(renderer, &colliderRect);
            }
        }     
};

#endif 
