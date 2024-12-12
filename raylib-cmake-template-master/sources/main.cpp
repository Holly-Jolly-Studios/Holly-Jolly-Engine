#include "raylib.h"
#include "WorldManager.h"

#include <iostream>

#define SCREEN_WIDTH (800)
#define SCREEN_HEIGHT (600)

const int TARGET_FPS = 60;

#define WINDOW_TITLE "GPR-460 Assignment That Def Isn't Late"

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

    CloseWindow();

    return 0;
}
