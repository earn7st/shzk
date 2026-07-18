#pragma once

#include <vector>
#include <memory>

namespace vkR
{
	class Node
	{
	public:
		Node() = default;
		~Node() = default;

		void Tick(float deltaTime);

        template<typename T, typename... Args>
        T* AddComponent(Args&&... args) {
            static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
            auto component = std::make_unique<T>(std::forward<Args>(args)...);
            T* componentPtr = component.get();
            components.push_back(std::move(component));
            return componentPtr;
        }

        template<typename T>
        T* GetComponent() {
            for (auto& component : components) {
                if (T* result = dynamic_cast<T*>(component.get())) {
                    return result;
                }
            }
            return nullptr;
        }

	private:
		std::vector<std::unique_ptr<Component>> components;
	};
}