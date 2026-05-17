#include "ECS.h"
#include "../Logger/Logger.h"

#include <algorithm>

int IComponent::nextId = 0;

int Entity::GetId() const {
    return id;
}

void Entity::Kill() {
    registry->KillEntity(*this);
}

std::vector<Entity> System::GetSystemEntities() const {
    return entities;
}

const Signature& System::GetComponentSignature() const {
    return componentSignature;
}

Entity Registry::CreateEntity() {
    int entityId;

    if (freeIds.empty()) {
        // if there are no free ids waiting to be reused
        entityId = numEntities++;
        if (entityId >= (int)entityComponentSignatures.size()) {
            entityComponentSignatures.resize(entityId + 1);
        }
    } else {
        // reuse an id from the list of previously removed entities 
        entityId = freeIds.front();
        freeIds.pop_front();
    }

    Entity entity(entityId);
    entity.registry = this;
    entitiesToBeAdded.insert(entity);
    
    Logger::Log("Entity created with id = " + std::to_string(entityId));

    return entity;
}

void Registry::KillEntity(Entity entity) {
    entitiesToBeKilled.insert(entity);
}

void Registry::AddEntityToSystem(Entity entity) {
    const auto entityId = entity.GetId();
        
    const auto& entityCompnonentSignature = entityComponentSignatures[entityId];
    
    for (auto& system: systems) {
        const auto& systemComponentSignature = system.second->GetComponentSignature();
        
        bool isInterested = (entityCompnonentSignature & systemComponentSignature) == systemComponentSignature;
        
        if (isInterested) {
            system.second->AddEntityToSystem(entity);
        }
    }
}

void Registry::RemoveEntityFromSystem(Entity entity) {
    for (auto system: systems) {
        system.second->RemoveEntityFromSystem(entity);
    } 
}

void Registry::Update() {
    // add the entities that are waiting to be created to the active Systems 
    for (auto entity: entitiesToBeAdded) {
        AddEntityToSystem(entity);
    }
    entitiesToBeAdded.clear();

    // remove the entities that are waiting to be killed form the active Systems
    for (auto entity: entitiesToBeKilled) {
        RemoveEntityFromSystem(entity);
        entityComponentSignatures[entity.GetId()].reset();

        // make the entity id available to be reused 
        freeIds.push_back(entity.GetId());
    }
    entitiesToBeKilled.clear();
}

void System::AddEntityToSystem(Entity entity) {
    entities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity) {
    entities.erase(std::remove_if(entities.begin(), entities.end(), [&entity](Entity other) {
        return entity == other;
    }), entities.end());
}


