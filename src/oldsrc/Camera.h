
#include <vkR_types.h>
#include <SDL3/SDL_events.h>

class Camera {
public:
    glm::vec3 velocity = glm::vec3(0.f);
    glm::vec3 position = glm::vec3(0, 0, 5);
    // vertical rotation
    float pitch{ 0.f };
    // horizontal rotation
    float yaw{ 0.f };

    float sens = 1.0f;
    float speed = 0.1f;

    glm::mat4 getViewMatrix();
    glm::mat4 getRotationMatrix();

    void processSDLEvent(const SDL_Event& e);

    void update();
};