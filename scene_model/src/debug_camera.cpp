#include <glm/glm.hpp>
#include "camera.h"

#include <iostream>

void DebugCameraInfo(
    const Camera& camera,
    const glm::vec3& cubePos,
    const glm::vec3& qilinPos, float qilinScale, float qilinRotationY,
    const glm::vec3& cottagePos, float cottageScale, float cottageRotationY,
    const glm::vec3& skullPos, float skullScale, float skullRotationY,
    const glm::vec3& towerPos, float towerScale, float towerRotationY
)
{
    glm::vec3 pos = camera.Position;
    glm::vec3 front = camera.Front;

    std::cout << "\n--- Camera Info ---\n";
    std::cout << "Position (X, Y, Z): "
        << pos.x << ", " << pos.y << ", " << pos.z
        << "\nFront Vector (Dir): "
        << front.x << ", " << front.y << ", " << front.z
        << "\n";

    std::cout << "--- Model Positions ---\n";

    std::cout << "Cube Pos:        ("
        << cubePos.x << ", " << cubePos.y << ", " << cubePos.z << ")\n";

    std::cout << "Qilin Pos/Scale/Rot:   ("
        << qilinPos.x << ", " << qilinPos.y << ", " << qilinPos.z << ") | Scale: "
        << qilinScale << " | Rot Y: " << qilinRotationY << "\n";

    std::cout << "Cottage Pos/Scale/Rot: ("
        << cottagePos.x << ", " << cottagePos.y << ", " << cottagePos.z << ") | Scale: "
        << cottageScale << " | Rot Y: " << cottageRotationY << "\n";

    std::cout << "Skull Pos/Scale/Rot:   ("
        << skullPos.x << ", " << skullPos.y << ", " << skullPos.z << ") | Scale: "
        << skullScale << " | Rot Y: " << skullRotationY << "\n";

    std::cout << "Tower Pos/Scale/Rot:   ("
        << towerPos.x << ", " << towerPos.y << ", " << towerPos.z << ") | Scale: "
        << towerScale << " | Rot Y: " << towerRotationY << "\n";
    std::cout << "-----------------------\n";
}

void PromptForNewPositions(
    glm::vec3& cubePos,
    glm::vec3& qilinPos, float& qilinScale, float& qilinRotationY,
    glm::vec3& cottagePos, float& cottageScale, float& cottageRotationY,
    glm::vec3& skullPos, float& skullScale, float& skullRotationY,
    glm::vec3& towerPos, float& towerScale, float& towerRotationY
)
{
    auto updateModel = [](const std::string& modelName, glm::vec3& pos, float& scale, float& rotY) {
        std::cout << "\n--- Editing " << modelName << " ---\n";

        std::cout << "Current Pos (X Y Z): " << pos.x << " " << pos.y << " " << pos.z << "\n";
        std::cout << "Enter NEW Position (X Y Z): ";
        float x, y, z;
        if (!(std::cin >> x >> y >> z)) {
            std::cerr << "Invalid input for " << modelName << " position. Skipping update.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return;
        }
        pos = glm::vec3(x, y, z);

        std::cout << "Current Scale: " << scale << "\n";
        std::cout << "Enter NEW Scale: ";
        float s;
        if (!(std::cin >> s)) {
            std::cerr << "Invalid input for " << modelName << " scale. Skipping update.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return;
        }
        scale = s;

        std::cout << "Current Rotation Y (degrees): " << rotY << "\n";
        std::cout << "Enter NEW Rotation Y (degrees): ";
        float r;
        if (!(std::cin >> r)) {
            std::cerr << "Invalid input for " << modelName << " Y-rotation. Skipping update.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return;
        }
        rotY = r;
        };

    auto updateCube = [](glm::vec3& pos) {
        std::cout << "\n--- Editing Cube ---\n";
        std::cout << "Current Pos (X Y Z): " << pos.x << " " << pos.y << " " << pos.z << "\n";
        std::cout << "Enter NEW Position (X Y Z): ";
        float x, y, z;
        if (!(std::cin >> x >> y >> z)) {
            std::cerr << "Invalid input for Cube position. Skipping update.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return;
        }
        pos = glm::vec3(x, y, z);
        };

    std::cout << "\n=========================================\n";
    std::cout << "!!! INTERACTIVE EDIT MODE ACTIVATED !!!\n";
    std::cout << "The rendering window will be BLOCKED until input is complete.\n";

    updateCube(cubePos);
    updateModel("Qilin", qilinPos, qilinScale, qilinRotationY);
    updateModel("Cottage", cottagePos, cottageScale, cottageRotationY);
    updateModel("Skull", skullPos, skullScale, skullRotationY);
    updateModel("Tower", towerPos, towerScale, towerRotationY);

    std::cout << "!!! EDITING COMPLETE. Resuming rendering. !!!\n";
    std::cout << "=========================================\n";
}