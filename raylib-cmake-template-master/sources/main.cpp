#include "raylib.h"
#include "WorldManager.h"

#include <iostream>

#define SCREEN_WIDTH (1920)
#define SCREEN_HEIGHT (1080)

const int TARGET_FPS = 60;

#define WINDOW_TITLE "GPR-460 Assignment That Def Isn't Late"

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(TARGET_FPS);

    WorldManager::CreateInstance();
    WorldManager* worldManager = WorldManager::GetInstance();

    // Loop until game is done
    worldManager->GameLoop();

    // Cleanup
    worldManager->DestroyInstance();
    worldManager = nullptr;

    CloseWindow();

    return 0;
}
