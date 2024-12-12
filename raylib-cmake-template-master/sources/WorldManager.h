#pragma once

#include "ComponentPool.h"
#include "StackAllocator.h"
#include "GameObject.h"

#include <unordered_map>
#include <chrono>
#include <fstream>

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int POOL_SIZE = 20;

class WorldManager
{
public:
	static WorldManager* GetInstance();
	static void CreateInstance();
	static void DestroyInstance();

	void Cleanup();
	void GameLoop();

private:
	WorldManager();
	~WorldManager();

	void UpdateWorld();
	void RenderWorld();
	void ClearWorld();

	static WorldManager* m_WorldManager;
	std::unordered_map<int, GameObject*> m_World;

	// Level Serialization
	void LoadWorld();
	int GetComponentValue(std::string stream);


	// Game Object Components
	GameObject* CreateEmptyGO();

	void AddNewComponent(GameObject* gameobject, ComponentTypes type);

	// Components
	NewTransform* CreateNewTransformPool();
	NewTransform* CreateNewTransformPool(float x, float y);
	CollisionColorChanger* CreateNewColorChangerPool();
	CollisionColorChanger* CreateNewColorChangerPool(Color changedColor, Color defaultColor);
	PlayerController* CreateNewControllerPool();
	RectangleCollider* CreateNewColliderPool();
	RectangleRenderer* CreateNewRendererPool();
	RectangleRenderer* CreateNewRendererPool(float width, float height, float topLeftX, float topLeftY, Color color); //TODO: change this so that there are only 4 variables max passed into the function

	// Helpers
	void SpawnPlayerGameObject(NewTransform* playerTransform);
	void DeleteFromWorld(GameObject* go);
	GameObject* GetClosestGO(NewTransform* transform);

	// GameObject ID
	int m_ObjectIDIndex;
	int GetNewGOObjectID();

	// Debug
	void DebugFrames();
	int* m_Frames;
	RectangleCollider* m_ActiveCollisions;
	size_t m_NumCollisions;

	// Pools
	ComponentPool<NewTransform> m_NewTransformPool;
	std::vector<NewTransform*> m_NewTransformList;

	ComponentPool<PlayerController> m_PlayerControllerPool;
	std::vector<PlayerController*> m_PlayerControllerList;

	ComponentPool<RectangleCollider> m_ColliderPool;
	std::vector<RectangleCollider*> m_ColliderList;

	ComponentPool<RectangleRenderer> m_RendererPool;
	std::vector<RectangleRenderer*> m_RendererList;

	ComponentPool<CollisionColorChanger> m_CollisionColorChangerPool;
	std::vector<CollisionColorChanger*> m_CollisionColorChangerList;
};
