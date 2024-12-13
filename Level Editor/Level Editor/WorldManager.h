#pragma once

#include "ComponentPool.h"
#include "StackAllocator.h"
#include "GameObject.h"

#include <chrono>
#include <fstream>
#include <filesystem>
#include <stdlib.h>
#include <unordered_map>
#include <random>

#include "imgui.h"
#include "rlImGui.h"

const int MAX_COMMENT_LENGTH = 256;
const char COMMENT_SYMBOL = '#';
const int POOL_SIZE = 100;
const int MAX_COMPONENTS = 100;


class WorldManager
{
public:
	// Instance Helpers
	static void CreateInstance();
	static void DestroyInstance();
	static WorldManager* GetInstance();

	// Loop
	void GameLoop();

private:

	#pragma region Functions

	// Constructor and Destructor
	WorldManager();
	~WorldManager();

	// Loop Other
	void UpdateGameObjects();
	void Render();

	// Level Serialization
	std::unordered_map<int, GameObject*> m_World;
	int GetComponentValuesFromStream(std::string stream);
	void LoadLevel(std::string fileToOpen);
	void SaveWorld(std::string fileToSave);

	// GameObject Helpers
	void AddComponent(GameObject* gameobject, ComponentTypes type);
	void RemoveComponent(GameObject* gameobject, ComponentTypes type);
	GameObject* NewGameObject();
	void SpawnPlayer(NewTransform* transform);
	void ClearWorld();

	// GameObject ID
	int GetNewObjectID();
	void ClearObjectIDs();

	// Component Pool
	NewTransform* AddToTransformPool();
	NewTransform* AddToTransformPool(float x, float y);

	CollisionColorChanger* AddToCollisionColorChangerPool();
	CollisionColorChanger* AddToCollisionColorChangerPool(Color newColor, Color defaultColor);

	PlayerController* AddToPlayerControllerPool();

	RectangleCollider* AddToColliderPool();

	RectangleRenderer* AddToRendererPool();
	RectangleRenderer* AddToRendererPool(float width, float height, float leftX, float leftY, Color color);
	RectangleRenderer* AddToRendererPool(Vector2 size, Vector2 left, Color color);

	void RemoveFromTransformPool(GameObject* gameobject);
	void RemoveFromCollisionColorChangerPool(GameObject* gameobject);
	void RemoveFromPlayerControllerPool(GameObject* gameobject);
	void RemoveFromColliderPool(GameObject* gameobject);
	void RemoveFromRendererPool(GameObject* gameobject);

	// Debug 
	void SaveButton();
	void DeleteAllGOOfType(ComponentTypes type);
	void SpawnGameObjectOnMouse();
	bool CheckMouseCollision(Rectangle rect);
	void ToggleUI();
	void OpenEditUI();
	void CloseEditUI();
	Color GetRandomColor();

	#pragma endregion

	#pragma region VARIABLES

	// Static Instance
	static WorldManager* gs_WorldManager;

	// Level Serialization Path
	int numFiles = 0;
	std::string levelPath = "level";
	std::string fileExtention = ".txt";
	std::string saveFileName = "";

	// Collision Vars
	RectangleCollider* m_ActiveCollisions;
	size_t m_Collisions;

	// GameObject ID
	int m_ObjectIDIndex;
	GameObject* m_SelectedGO;
	void DeleteSelected();

	// Component Pools & Lists
	ComponentPool<NewTransform> m_TransformComponentPool;
	std::vector<NewTransform*> m_TransformComponentList;

	ComponentPool<PlayerController> m_PlayerControllerPool;
	std::vector<PlayerController*> m_PlayerControllerList;

	ComponentPool<RectangleCollider> m_ColliderPool;
	std::vector<RectangleCollider*> m_ColliderList;

	ComponentPool<RectangleRenderer> m_RendererPool;
	std::vector<RectangleRenderer*> m_RendererList;

	ComponentPool<CollisionColorChanger> m_ColliderColorChangerPool;
	std::vector<CollisionColorChanger*> m_ColliderColorChangerList;

	// Debug
	int* m_Frames;
	bool m_ShowGameObjectEditor;
	bool m_Saving;

	#pragma endregion
};