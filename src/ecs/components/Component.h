#pragma once

#include <glm/gtc/quaternion.hpp>

#include "resource/ResourceManager.h"

class Entity;

class ComponentTypeIDSystem {
private:
    static size_t nextTypeID;

public:
    template<typename T>
    static size_t GetTypeID() {
        static size_t typeID = nextTypeID++;
        return typeID;
    }
};

class Component
{
private:
	Entity* owner;

public:
	virtual ~Component() = default;

	virtual void Initialize() {}
	virtual void Update(float deltaTime) {}
	virtual void Render() {}

	void SetOwner(Entity* entity) { owner = entity; }
	Entity* GetOwner() const { return owner; }

    template<typename T>
    static size_t GetTypeID() {
        return ComponentTypeIDSystem::GetTypeID<T>();
    }
};




