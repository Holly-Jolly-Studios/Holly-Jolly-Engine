#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

int main()
{
	const int w = 700;
	const int h = 700;

	InitWindow(w, h, "test");
	SetTargetFPS(60);

	rlImGuiSetup(true);

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(RAYWHITE);

		rlImGuiBegin();
		// render imgui content

		// frame 1
		ImGui::Begin("Hello!", NULL);
		ImGui::Text("Hello World!");
		ImGui::End();

		rlImGuiEnd();

		EndDrawing();
	}

	rlImGuiShutdown();
	CloseWindow();
	return 0;
}
