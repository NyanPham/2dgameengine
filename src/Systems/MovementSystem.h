#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

class MovementSystem: public System {
    public:
        MovementSystem() {
            // RequireComponent<TransformComponent>();
            // RequireComponent<VelocityComponent>();
        }

        void Update() {
            for (auto entity: GetEntities()) {

            }
        }
};

#endif
