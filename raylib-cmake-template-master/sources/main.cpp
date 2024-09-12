#include "raylib.h"
#include "GameObject.cpp"
#include <iostream>

#define SCREEN_WIDTH (800)
#define SCREEN_HEIGHT (450)

#define WINDOW_TITLE "Component-Based Game Objects"

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

    // GameObjects for assignment
    GameObject player;
    player.CreateRenderer(50, 50, NewTransform{ 10, 10 }, MAROON);
    player.CreateCollider(50, 50);
    player.CreatePlayerController(2, NewTransform{10, 10});
    player.CreateColliderColorChanger(GREEN);

    GameObject wall;
    wall.CreateRenderer(100, 100, NewTransform{ 50, 50 }, BLUE);
    wall.CreateCollider(100, 100);

    GameObject background;
    wall.CreateRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, NewTransform{ 0, 0 }, GRAY);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(WHITE);

        // Update player GO, handles input
        player.Update();

        // Check for player and wall collision
        player.CheckCollision(wall);

        // Update everything else
        wall.Update();
        background.Update();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
