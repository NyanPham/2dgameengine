#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Game/Game.h"
#include <SDL2/SDL_render.h>

class MovementSystem: public System {
    public:
        MovementSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<RigidBodyComponent>();
        }
        
        void SubscribeToEvents(const std::unique_ptr<EventBus>& eventBus) {
            eventBus->SubscribeToEvent<CollisionEvent>(this, &MovementSystem::OnCollision);
        }

        void OnCollision(CollisionEvent& event) {
            std::string aId = std::to_string(event.a.GetId());
            std::string bId = std::to_string(event.b.GetId());
            Logger::Log("Collision event emitted: " + aId + " and " + bId);

            Entity a = event.a;
            Entity b = event.b;

            if (a.BelongsToGroup("enemies") && b.BelongsToGroup("obstacles")) {
                OnEnemyHitsObstacle(a, b);
            }

            if (b.BelongsToGroup("enemies") && a.BelongsToGroup("obstacles")) {
                OnEnemyHitsObstacle(b, a);
            }
        }

        void OnEnemyHitsObstacle(Entity enemy, Entity obstacle) {
            if (enemy.HasComponent<RigidBodyComponent>() && enemy.HasComponent<SpriteComponent>()) {
                auto& rigidBody = enemy.GetComponent<RigidBodyComponent>();
                auto& sprite = enemy.GetComponent<SpriteComponent>();

                if (rigidBody.velocity.x != 0) {
                    rigidBody.velocity.x *= -1;
                    sprite.flip = (sprite.flip == SDL_FLIP_NONE) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
                }
                
                if (rigidBody.velocity.y != 0) {
                    rigidBody.velocity.y *= -1;
                    sprite.flip = (sprite.flip == SDL_FLIP_NONE) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
                }
            }
        }

        void Update(double deltaTime) {
            for (auto entity: GetSystemEntities()) {
                // update entity position based on its velocity 
                auto& transform = entity.GetComponent<TransformComponent>();
                auto& rigidBody = entity.GetComponent<RigidBodyComponent>();

                transform.position.x += rigidBody.velocity.x * deltaTime;
                transform.position.y += rigidBody.velocity.y * deltaTime;
              
                // prevent the main player from moving outside the map boundaries 
                if (entity.HasTag("player")) {
                    int paddingLeft = 10;
                    int paddingTop = 10;
                    int paddingRight = 50;
                    int paddingBottom = 50;
                    transform.position.x = transform.position.x < paddingLeft ? paddingLeft : transform.position.x;
                    transform.position.x = transform.position.x > Game::mapWidth - paddingRight ? Game::mapWidth - paddingRight: transform.position.x;
                    transform.position.y = transform.position.y < paddingTop ? paddingTop : transform.position.y;
                    transform.position.y = transform.position.y > Game::mapHeight - paddingBottom ? Game::mapHeight - paddingBottom: transform.position.y;
                }

                // check if entity is outside the map boundaries
                int cullingMargin = 100;

                bool isEntityOutsideMap = (
                    transform.position.x < -cullingMargin ||
                    transform.position.x > Game::mapWidth + cullingMargin || 
                    transform.position.y < -cullingMargin||
                    transform.position.y > Game::mapHeight + cullingMargin
                );

                // kill all entities that move outide the map boundaries
                if (isEntityOutsideMap && !entity.HasTag("player")) {
                    entity.Kill(); 
                }
            }
        }
};

#endif
