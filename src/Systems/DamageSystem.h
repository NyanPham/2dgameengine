#ifndef DAMAGESYSTEM_H
#define DAMAGESYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/HealthComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"
#include "../Logger/Logger.h"

class DamageSystem: public System {
    public:
        DamageSystem() {
            RequireComponent<BoxColliderComponent>();
        }

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
            eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::OnCollision);
        }

        void OnCollision(CollisionEvent& event) {
            std::string aId = std::to_string(event.a.GetId());
            std::string bId = std::to_string(event.b.GetId());
            Logger::Log("Collision event emitted: " + aId + " and " + bId);

            Entity a = event.a;
            Entity b = event.b;

            if (a.BelongsToGroup("projectiles") && b.HasTag("player")) {
                OnProjectileHitsPlayer(a, b);
            }
            
            if (b.BelongsToGroup("projectiles") && a.HasTag("player")) {
                OnProjectileHitsPlayer(b, a);
            }
            
            if (a.BelongsToGroup("projectiles") && b.BelongsToGroup("enemies")) {
                OnProjectileHitsEnemy(a, b);
            } 

            if (b.BelongsToGroup("projectiles") && a.BelongsToGroup("enemies")) {
                OnProjectileHitsEnemy(b, a);
            }

            // event.a.Kill();
            // event.b.Kill();
        }

        void OnProjectileHitsPlayer(Entity projectile, Entity player) {
            auto projectileComponent = projectile.GetComponent<ProjectileComponent>();

            if (!projectileComponent.isFriendly) {
                // reduce the health of the player by the projectile hitPercentDamage 
                auto& health = player.GetComponent<HealthComponent>();
                health.healthPercentage -= projectileComponent.hitPercentDamage;

                if (health.healthPercentage <= 0) {
                    player.Kill();
                }

                projectile.Kill();
            }
        }

        void OnProjectileHitsEnemy(Entity projectile, Entity enemy) {
            auto projectileComponent = projectile.GetComponent<ProjectileComponent>();

            // only damage the enemy if projectile is friendly
            if (projectileComponent.isFriendly) {
                auto& health = enemy.GetComponent<HealthComponent>();

                health.healthPercentage -= projectileComponent.hitPercentDamage;

                if (health.healthPercentage <= 0) {
                    enemy.Kill();
                }

                projectile.Kill();
            }
        }

        void Update() {

        }
};

#endif
