#include "WorldManager.h"

const int MAX_COMPONENTS = 20;

WorldManager* WorldManager::m_WorldManager = nullptr;

WorldManager* WorldManager::GetInstance()
{
	if (m_WorldManager != nullptr)
	{
		return m_WorldManager;
	}
}

void WorldManager::CreateInstance()
{
	if (m_WorldManager == nullptr)
	{
		m_WorldManager = new WorldManager;
	}
}

void WorldManager::DestroyInstance()
{
	delete m_WorldManager;
	m_WorldManager = nullptr;
}

void WorldManager::Cleanup()
{
	ClearWorld();
}

void WorldManager::GameLoop()
{
	int frameNumber = 0;

	float targetTime = 16.67;

	//TODO: Put Display fps using the frame allocator

	while (!WindowShouldClose())
	{
		std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

		BeginDrawing();

		ClearBackground(RAYWHITE);

		//Create frame allocators

		StackAllocator frameAllocator;

		m_Frames = frameAllocator.Alloc<int>();

		//TODO: Loop through the collider components
				// check if each collider is colliding
				// if colliding, allocate that collider into the stack allocator
				// numCollisions++; 

		m_ActiveCollisions = (RectangleCollider*)frameAllocator.GetCurrentHead();
		m_NumCollisions = 0;

		int i;
		for (i = 0; i < m_ColliderList.size(); i++)
		{
			if (m_ColliderList[i]->GetIsColliding()) //if that collider is currently colliding
			{
				frameAllocator.Alloc<RectangleCollider*>(m_ColliderList[i]); //TODO: Fix this so that it can allocate properly
				m_NumCollisions++;
			}
		}

		//std::cout << "NUM Collisions during this frame: " << numCollisions << std::endl;

		UpdateWorld();
		RenderWorld();

		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		*m_Frames = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();

		DebugFrames();
		frameAllocator.Reset();

		m_Frames = nullptr;
		m_ActiveCollisions = nullptr;

		EndDrawing();
	}
}


// Private Functions

WorldManager::WorldManager()
{
	// First GO index
	m_ObjectIDIndex = 0;

	// Pool setup
	m_NewTransformPool.SetSize(MAX_COMPONENTS);
	m_CollisionColorChangerPool.SetSize(MAX_COMPONENTS);
	m_PlayerControllerPool.SetSize(MAX_COMPONENTS);
	m_ColliderPool.SetSize(MAX_COMPONENTS);
	m_RendererPool.SetSize(MAX_COMPONENTS);

	// BG
	GameObject* background = new GameObject;
	background->SetObjectID(GetNewGOObjectID());

	background->SetTransform(CreateNewTransform(0, 0));
	background->GetTransform()->SetGameObjectID(background->GetObjectID());

	background->SetRenderer(CreateNewRenderer(13000, 8000, -6000, 0, Color{ 0, 50, 25, 255 }));
	background->GetRenderer()->SetGameObjectID(background->GetObjectID());

	m_World.emplace(background->GetObjectID(), background);


	// Player
	GameObject* player = new GameObject;
	player->SetObjectID(GetNewGOObjectID());

	player->SetTransform(CreateNewTransform(500, 500));
	player->GetTransform()->SetGameObjectID(player->GetObjectID());

	player->SetPlayerController(CreateNewController());
	player->GetPlayerController()->SetGameObjectID(player->GetObjectID());

	player->SetCollider(CreateNewCollider());
	player->GetCollider()->SetGameObjectID(player->GetObjectID());

	player->SetRenderer(CreateNewRenderer(40, 40, 400, 280, Color{ 0, 100, 255, 255 }));
	player->GetRenderer()->SetGameObjectID(player->GetObjectID());

	player->SetColorChanger(CreateNewColorChanger(Color{ 255, 0, 0, 255 }, Color{ 0, 100, 255, 255 }));
	player->GetCollisionColorChanger()->SetGameObjectID(player->GetObjectID());

	m_World.emplace(player->GetObjectID(), player);
}

WorldManager::~WorldManager()
{
	// Do nothing
}


void WorldManager::UpdateWorld()
{
	int i;

	// Update player controller
	for (i = 0; i < m_PlayerControllerList.size(); i++)
	{
		int players = m_PlayerControllerList[i]->Update(m_World[m_PlayerControllerList[i]->GetGameObjectID()]->GetTransform());

		if (players == 0)
		{
			// do nothing
		}
		else if (players == 1)
		{
			SpawnPlayerGameObject(m_World[m_PlayerControllerList[i]->GetGameObjectID()]->GetTransform());
		}
		else
		{
			DeleteFromWorld(m_World[GetClosestGO(m_World[m_PlayerControllerList[i]->GetGameObjectID()]->GetTransform())->GetObjectID()]);
		}
	}

	//Check for collisions
	int j;
	for (i = 0; i < m_ColliderList.size(); i++)
	{
		for (j = 0; j < m_ColliderList.size(); j++)
		{
			if (m_ColliderList[i] != m_ColliderList[j])
			{
				GameObject* go1 = m_World[m_ColliderList[i]->GetGameObjectID()];
				GameObject* go2 = m_World[m_ColliderList[j]->GetGameObjectID()];
				m_ColliderList[i]->CheckCollision(Rectangle{ go1->GetTransform()->GetX(), go1->GetTransform()->GetY(), go1->GetRenderer()->GetWidth(), go1->GetRenderer()->GetHeight() }, Rectangle{ go2->GetTransform()->GetX(), go2->GetTransform()->GetY(), go2->GetRenderer()->GetWidth(), go2->GetRenderer()->GetHeight() });
				m_ColliderList[j]->CheckCollision(Rectangle{ go1->GetTransform()->GetX(), go1->GetTransform()->GetY(), go1->GetRenderer()->GetWidth(), go1->GetRenderer()->GetHeight() }, Rectangle{ go2->GetTransform()->GetX(), go2->GetTransform()->GetY(), go2->GetRenderer()->GetWidth(), go2->GetRenderer()->GetHeight() });
			}
		}
	}

	//Update collision collor changer
	for (i = 0; i < m_CollisionColorChangerList.size(); i++)
	{
		bool stateChanged = m_CollisionColorChangerList[i]->Update(m_World[m_CollisionColorChangerList[i]->GetGameObjectID()]->GetCollider()->GetIsColliding());

		if (stateChanged)
		{
			if (m_CollisionColorChangerList[i]->GetIsDefaultColor())
			{
				m_World[m_CollisionColorChangerList[i]->GetGameObjectID()]->GetRenderer()->SetColor(m_CollisionColorChangerList[i]->GetDefaultColor());
			}
			else
			{
				m_World[m_CollisionColorChangerList[i]->GetGameObjectID()]->GetRenderer()->SetColor(m_CollisionColorChangerList[i]->GetNewColor());
			}
		}
	}
}

void WorldManager::RenderWorld()
{
	int i;

	for (i = 0; i < m_RendererList.size(); i++)
	{
		RectangleRenderer* ren = m_RendererList[i];
		if (ren != nullptr)
		{
			if (m_World[ren->GetGameObjectID()]->GetTransform() != NULL)
			{
				DrawRectangle(m_World[ren->GetGameObjectID()]->GetTransform()->GetX(), m_World[ren->GetGameObjectID()]->GetTransform()->GetY(), ren->GetWidth(), ren->GetHeight(), ren->GetColor());
			}
			else
			{
				DrawRectangle(ren->GetTopLeftX(), ren->GetTopLeftY(), ren->GetWidth(), ren->GetHeight(), ren->GetColor());
			}
		}
	}
}

void WorldManager::ClearWorld()
{
	// do something
}

int WorldManager::GetNewGOObjectID()
{
	m_ObjectIDIndex++;
	return m_ObjectIDIndex;
}

void WorldManager::DebugFrames()
{
	if (IsKeyDown(KEY_P))
	{
		printf("Time for frame (ms): %i \n", *m_Frames);
	}
	else if (IsKeyDown(KEY_C))
	{
		printf("Number of collisions this frame: %i \n", m_NumCollisions);
	}
}

void WorldManager::SpawnPlayerGameObject(NewTransform* playerTransform)
{
	GameObject* go = new GameObject;
	go->SetObjectID(GetNewGOObjectID());

	go->SetTransform(CreateNewTransform(playerTransform->GetX() + 10, playerTransform->GetY() + 10));
	go->GetTransform()->SetGameObjectID(go->GetObjectID());

	go->SetCollider(CreateNewCollider());
	go->GetCollider()->SetGameObjectID(go->GetObjectID());

	go->SetRenderer(CreateNewRenderer(80, 80, 800, 600, Color{ 255, 255, 0, 255 }));
	go->GetRenderer()->SetGameObjectID(go->GetObjectID());

	m_World.emplace(go->GetObjectID(), go);
}

void WorldManager::DeleteFromWorld(GameObject* go)
{
	// Delete each component then delete based off of game object id
	if (go->GetCollider() != NULL)
	{
		auto found = std::find(m_ColliderList.begin(), m_ColliderList.end(), (go->GetCollider()));
		if (found != m_ColliderList.end()) {
			m_ColliderList.erase(found);
		}

		m_ColliderPool.Delete(go->GetCollider());
	}

	if (go->GetCollisionColorChanger() != NULL)
	{
		auto found = std::find(m_CollisionColorChangerList.begin(), m_CollisionColorChangerList.end(), (go->GetCollisionColorChanger()));
		if (found != m_CollisionColorChangerList.end()) {
			m_CollisionColorChangerList.erase(found);
		}

		m_CollisionColorChangerPool.Delete(go->GetCollisionColorChanger());
	}

	if (go->GetPlayerController() != NULL)
	{
		auto found = std::find(m_PlayerControllerList.begin(), m_PlayerControllerList.end(), (go->GetPlayerController()));
		if (found != m_PlayerControllerList.end()) {
			m_PlayerControllerList.erase(found);
		}

		m_PlayerControllerPool.Delete(go->GetPlayerController());
	}

	if (go->GetRenderer() != NULL)
	{
		auto found = std::find(m_RendererList.begin(), m_RendererList.end(), (go->GetRenderer()));
		if (found != m_RendererList.end()) {
			m_RendererList.erase(found);
		}

		m_RendererPool.Delete(go->GetRenderer());
	}

	if (go->GetTransform() != NULL)
	{
		auto found = std::find(m_NewTransformList.begin(), m_NewTransformList.end(), (go->GetTransform()));
		if (found != m_NewTransformList.end()) {
			m_NewTransformList.erase(found);
		}

		m_NewTransformPool.Delete(go->GetTransform());
	}

	m_World.erase(go->GetObjectID());
}

GameObject* WorldManager::GetClosestGO(NewTransform* transform)
{
	NewTransform* closest = NULL;
	float closestDistance = 99999999.9f;

	for (NewTransform* temp : m_NewTransformList)
	{
		if (temp != transform)
		{
			float x = transform->GetX() - temp->GetX();
			float y = transform->GetY() - temp->GetY();

			float distance = sqrt((x * x) + (y * y));

			if (distance < closestDistance)
			{
				closestDistance = distance;
				closest = temp;
			}
		}
	}
	return m_World[closest->GetGameObjectID()];
}

#pragma region Component

NewTransform* WorldManager::CreateNewTransform()
{
	NewTransform* t = m_NewTransformPool.New();
	m_NewTransformList.push_back(t);
	return t;
}

NewTransform* WorldManager::CreateNewTransform(float x, float y)
{
	NewTransform* t = m_NewTransformPool.New(x, y);
	m_NewTransformList.push_back(t);
	return t;
}

CollisionColorChanger* WorldManager::CreateNewColorChanger()
{
	CollisionColorChanger* c = m_CollisionColorChangerPool.New();
	m_CollisionColorChangerList.push_back(c);
	return c;
}

CollisionColorChanger* WorldManager::CreateNewColorChanger(Color changedColor, Color defaultColor)
{
	CollisionColorChanger* c = m_CollisionColorChangerPool.New(changedColor, defaultColor);
	m_CollisionColorChangerList.push_back(c);
	return c;
}

PlayerController* WorldManager::CreateNewController()
{
	PlayerController* p = m_PlayerControllerPool.New();
	m_PlayerControllerList.push_back(p);
	return p;
}

RectangleCollider* WorldManager::CreateNewCollider()
{
	RectangleCollider* r = m_ColliderPool.New();
	m_ColliderList.push_back(r);
	return r;
}

RectangleRenderer* WorldManager::CreateNewRenderer()
{
	RectangleRenderer* r = m_RendererPool.New();
	m_RendererList.push_back(r);
	return r;
}

RectangleRenderer* WorldManager::CreateNewRenderer(float width, float height, float topLeftX, float topLeftY, Color color)
{
	RectangleRenderer* r = m_RendererPool.New(width, height, topLeftX, topLeftY, color);
	m_RendererList.push_back(r);
	return r;
}

#pragma endregion

