#ifndef DEBUG_CAMERA_H
#define DEBUG_CAMERA_H

#include <glm/glm.hpp>
#include "camera.h"

// Function to print camera and model info to the console
void DebugCameraInfo(
    const Camera& camera,
    const glm::vec3& cubePos,
    const glm::vec3& qilinPos, float qilinScale, float qilinRotationY,
    const glm::vec3& cottagePos, float cottageScale, float cottageRotationY,
    const glm::vec3& skullPos, float skullScale, float skullRotationY,
    const glm::vec3& towerPos, float towerScale, float towerRotationY
);

// Function to interactively prompt the user to update model positions, scales, and rotations
void PromptForNewPositions(
    glm::vec3& cubePos,
    glm::vec3& qilinPos, float& qilinScale, float& qilinRotationY,
    glm::vec3& cottagePos, float& cottageScale, float& cottageRotationY,
    glm::vec3& skullPos, float& skullScale, float& skullRotationY,
    glm::vec3& towerPos, float& towerScale, float& towerRotationY
);

#endif // DEBUG_CAMERA_H
