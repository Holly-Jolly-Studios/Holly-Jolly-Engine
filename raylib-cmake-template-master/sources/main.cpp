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

    // Transforms
    NewTransform playerPos{ 400, 400 };
    NewTransform wallPos{ 300, 200 };
    NewTransform backroundPos{ 0, 0 };

    // GameObjects for assignment
    GameObject player;
    player.CreatePlayerController(2, playerPos);
    player.CreateRenderer(50, 50, playerPos, MAROON);
    player.CreateCollider(50, 50, playerPos);
    player.CreateColliderColorChanger(DARKBLUE);

    GameObject wall;
    wall.CreateRenderer(100, 100, wallPos, BLUE);
    wall.CreateCollider(100, 100, wallPos);

    GameObject background;
    background.CreateRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, backroundPos, GRAY);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        
        // Draw background before everything else
        background.Update();

        // Update player GameObject, handles player input (WASD or Arrow Keys)
        player.Update();

        // Check for player and wall collision
        player.CheckCollision(wall);

        // Update wall
        wall.Update();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
