#include "raylib.h"
#include "WorldManager.h"

#include <iostream>

#define SCREEN_WIDTH (1800)
#define SCREEN_HEIGHT (1000)

const int TARGET_FPS = 60;

#define WINDOW_TITLE "Level Editor"

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(TARGET_FPS);

    WorldManager::CreateInstance();
    WorldManager* gameManager = WorldManager::GetInstance();

    // Loop until game is done
    gameManager->GameLoop();

    // Cleanup
    gameManager->DestroyInstance();
    gameManager = nullptr;

	rlImGuiShutdown();
    CloseWindow();

    return 0;
}
