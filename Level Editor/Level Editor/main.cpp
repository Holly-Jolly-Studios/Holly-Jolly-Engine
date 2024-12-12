//#include "raylib.h"
//#include "imgui.h"
//#include "rlImGui.h"
//
//int main()
//{
//	const int w = 700;
//	const int h = 700;
//
//	InitWindow(w, h, "test");
//	SetTargetFPS(60);
//
//	rlImGuiSetup(true);
//
//	while (!WindowShouldClose())
//	{
//		BeginDrawing();
//		ClearBackground(RAYWHITE);
//
//		rlImGuiBegin();
//		// render imgui content
//
//		// frame 1
//		ImGui::Begin("Hello!", NULL);
//		ImGui::Text("Hello World!");
//		ImGui::End();
//
//		rlImGuiEnd();
//
//		EndDrawing();
//	}
//
//	rlImGuiShutdown();
//	CloseWindow();
//	return 0;
//}



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

	rlImGuiShutdown();
    CloseWindow();

    return 0;
}
