#include "Entity.h"

void Entity::Initialize()
{
    for (auto& component : components) 
    {
        component->Initialize();
    }
}	

void Entity::Update(float deltaTime)
{
    for (auto& component : components)
    {
        component->Update(deltaTime);
    }
}

void Entity::Render()
{
    if (!_bIsActive) return;

    for (auto& component : components) {
        component->Render();
    }
}