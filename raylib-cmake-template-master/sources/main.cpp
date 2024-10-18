#include "raylib.h"
#include <vector>
#include <iostream>

#include "GameObject/GameObject.h"
#include "ComponentPool/Component.h"
#include "ComponentPool/ComponentPool.h"
#include "ComponentPool/ComponentPoolType.h"


#define SCREEN_WIDTH (800)
#define SCREEN_HEIGHT (450)

#define WINDOW_TITLE "Component-Based Game Objects w/ ObjectPooling"

int size = 20;
std::vector<GameObject*> objects;

#pragma region Helpers

void AddToPool()
{
    if (objects.size() < size) {
        GameObject* newGO = new GameObject();

        // Random positions
        float xDir = rand() % 2;
        float yDir = rand() % 2;
        float randX = rand() % 100;
        float randY = rand() % 100;
        float randWidth = rand() % 100 + 1;
        float randHeight = rand() % 100 + 1;
        int randColor = rand() % 2;

        if (xDir == 0)
        {
            randX = -randX;
        }

        if (yDir == 0)
        {
            randY = -randY;
        }

        newGO->GetTransform()->SetNewTransform(objects[0]->GetTransform()->GetX() + randX, objects[0]->GetTransform()->GetY() + randY);


        if (randColor == 0) {
            newGO->CreateRenderer(randWidth, randHeight, BLUE);
            newGO->CreatePlayerController();
        }
        else
        {
            newGO->CreateRenderer(randWidth, randHeight, PURPLE);
        }

        newGO->CreateCollider(randWidth, randHeight);
        newGO->CreateColliderColorChanger(ORANGE);

        objects.push_back(newGO);
    }
}

void RemoveFromPool(GameObject* object)
{
    if (objects.size() > 1)
    {
        RectangleRenderer::rectangleRendererPool.Delete(object->GetRender());
        RectangleCollider::rectangleColliderPool.Delete(object->GetCollider());
        PlayerController::playerControllerPool.Delete(object->GetPlayer());
        ColliderColorChanger::colliderColorChangerPool.Delete(object->GetChanger());

        auto find = std::find(objects.begin(), objects.end(), object);
        objects.erase(find);
    }
}

GameObject* GetClosestToObject(GameObject* object)
{
    float x = object->GetTransform()->GetX();
    float y = object->GetTransform()->GetY();
    float distance = 0, closest;
    GameObject* currentCloset;

    for (int i = 1; i < objects.size(); i++)
    {
        distance = sqrt(pow(objects[i]->GetTransform()->GetX() - x, 2) + pow(objects[i]->GetTransform()->GetY() - y, 2));

        if (i == 1)
        {
            closest = distance;
            currentCloset = objects[i];
        }

        if (distance < closest)
        {
            closest = distance;
            currentCloset = objects[i];
        }
    }

    return currentCloset;
}

#pragma endregion


int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

    bool randomOn = false;

    GameObject* player = new GameObject();
    player->CreateRenderer(60, 60, RED);
    player->CreatePlayerController();
    objects.push_back(player);

    float previousTime = GetTime();

    while (!WindowShouldClose())
    {
        float currentTime = GetTime();
        float deltaTime = currentTime - previousTime;

        BeginDrawing();

        ClearBackground(WHITE);

        //Delete closest
        if (IsKeyPressed(KEY_D)) 
        {
            if (objects.size() > 1)
                RemoveFromPool(GetClosestToObject(objects[0]));
        }

        //Add object
        if (IsKeyPressed(KEY_SPACE)) 
        {
            AddToPool();
        }

        //Add until full
        if (IsKeyPressed(KEY_F)) 
        {
            for (int i = 0; i < size; i++)
            {
                AddToPool();
            }
        }

        // Delete all
        if (IsKeyPressed(KEY_C)) 
        {
            for (int i = objects.size(); i > 1; i--)
                RemoveFromPool(objects[objects.size() - 1]);
        }

        // Random spawn
        if (IsKeyPressed(KEY_R)) 
        {
            randomOn = !randomOn;
        }

        if (randomOn)
        {
            int createDestroy = rand() % 2;

            if (createDestroy == 0)
            {
                AddToPool();
            }
            else 
            {
                int randomObject = (rand() % objects.size()) + 1;
                if (randomObject == objects.size())
                {
                    randomObject--;
                }
                RemoveFromPool(objects[randomObject]);
            }
        }


        PlayerController::playerControllerPool.Update(deltaTime);
        ColliderColorChanger::colliderColorChangerPool.Update(player->GetTransform()->GetX(), player->GetTransform()->GetY(), player->GetRender()->GetWidth(), player->GetRender()->GetHeight());

        RectangleRenderer::rectangleRendererPool.Render();

        EndDrawing();

        previousTime = currentTime;
    }

    CloseWindow();

    return 0;
}
