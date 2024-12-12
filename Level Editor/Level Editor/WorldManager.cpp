#include "WorldManager.h"

// Global Instance
WorldManager* WorldManager::gs_WorldManager = nullptr;

// Instance Helpers
void WorldManager::CreateInstance()
{
	if (gs_WorldManager == nullptr)
	{
		gs_WorldManager = new WorldManager;
	}
}

void WorldManager::DestroyInstance()
{
	delete gs_WorldManager;
	gs_WorldManager = nullptr;
}

WorldManager* WorldManager::GetInstance()
{
	if (gs_WorldManager != nullptr)
	{
		return gs_WorldManager;
	}
}


// Loop
void WorldManager::GameLoop()
{
	int frame = 0;
	float targetTime = 16.67;

	while (!WindowShouldClose())
	{
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

		BeginDrawing();

		ClearBackground(RAYWHITE);

		// Put this somewhere else pls
		rlImGuiBegin();
		// render imgui content

		// frame 1
		ImGui::Begin("Hello!", NULL);
		ImGui::Text("Hello World!");
		ImGui::End();


		// Frame allocators
		StackAllocator frameAllocator;
		m_Frames = frameAllocator.Alloc<int>();

		m_ActiveCollisions = (RectangleCollider*)frameAllocator.GetCurrentHead();
		m_Collisions = 0;

		int i;
		for (i = 0; i < m_ColliderList.size(); i++)
		{
			if (m_ColliderList[i]->GetIsColliding())
			{
				frameAllocator.Alloc<RectangleCollider*>(m_ColliderList[i]);
				m_Collisions++;
			}
		}

		UpdateGameObjects();
		Render();

		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		*m_Frames = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
		frameAllocator.Reset();

		//Debug();

		m_Frames = nullptr;
		m_ActiveCollisions = nullptr;

		rlImGuiEnd();
		EndDrawing();
	}
}


// Constructor & Destructor
WorldManager::WorldManager()
{
	// ImGui setup
	rlImGuiSetup(true);

	// Init object IDs
	m_ObjectIDIndex = 0;

	// Init component pool sizes
	m_TransformComponentPool.SetSize(MAX_COMPONENTS);
	m_ColliderColorChangerPool.SetSize(MAX_COMPONENTS);
	m_PlayerControllerPool.SetSize(MAX_COMPONENTS);
	m_ColliderPool.SetSize(MAX_COMPONENTS);
	m_RendererPool.SetSize(MAX_COMPONENTS);

	// BG gameobject
	GameObject* background = NewGameObject();

	background->SetTransform(AddToTransformPool(0, 0));
	background->GetTransform()->SetGameObjectID(background->GetObjectID());

	background->SetRenderer(AddToRendererPool(Vector2{ 1000, 1000 }, Vector2{ -1000, 0 }, Color{ 0, 0, 0, 255 }));
	background->GetRenderer()->SetGameObjectID(background->GetObjectID());

	// Player gameobject
	GameObject* player = NewGameObject();

	player->SetTransform(AddToTransformPool(400, 280));
	player->GetTransform()->SetGameObjectID(player->GetObjectID());

	player->SetPlayerController(AddToPlayerControllerPool());
	player->GetPlayerController()->SetGameObjectID(player->GetObjectID());

	player->SetCollider(AddToColliderPool());
	player->GetCollider()->SetGameObjectID(player->GetObjectID());

	player->SetRenderer(AddToRendererPool(Vector2{ 40, 40 }, Vector2{ 400, 280 }, Color{ 0, 100, 255, 255 }));
	player->GetRenderer()->SetGameObjectID(player->GetObjectID());

	player->SetColorChanger(AddToCollisionColorChangerPool(Color{ 255, 0, 0, 255 }, Color{ 0, 100, 255, 255 }));
	player->GetCollisionColorChanger()->SetGameObjectID(player->GetObjectID());

	LoadLevel();
}

WorldManager::~WorldManager() = default;


// Loop Other
void WorldManager::UpdateGameObjects()
{
	int i, k;

	// Player Controller
	for (i = 0; i < m_PlayerControllerList.size(); i++)
	{
		// Should only have one player to update
		if (m_PlayerControllerList[i]->Update(m_World[m_PlayerControllerList[i]->GetGameObjectID()]->GetTransform()) == 1)
		{
			SpawnPlayer(m_World[m_PlayerControllerList[i]->GetGameObjectID()]->GetTransform());
		}
		//TODO: Check for multiple players, if there is more than 1 destroy others
	}

	// Collisions
	bool isColliding = false;
	for (i = 0; i < m_ColliderList.size(); i++)
	{
		isColliding = false;

		for (k = 0; k < m_ColliderList.size(); k++)
		{
			if (m_ColliderList[i] != m_ColliderList[k])
			{
				GameObject* gameobject1 = m_World[m_ColliderList[i]->GetGameObjectID()];
				GameObject* gameobject2 = m_World[m_ColliderList[k]->GetGameObjectID()];

				bool collision1 = m_ColliderList[i]->CheckCollision(Rectangle{ gameobject1->GetTransform()->GetX(),
					gameobject1->GetTransform()->GetY(),
					gameobject1->GetRenderer()->GetWidth(),
					gameobject1->GetRenderer()->GetHeight() },
					Rectangle{ gameobject2->GetTransform()->GetX(),
					gameobject2->GetTransform()->GetY(),
					gameobject2->GetRenderer()->GetWidth(),
					gameobject2->GetRenderer()->GetHeight() });

				bool collision2 = m_ColliderList[k]->CheckCollision(Rectangle{ gameobject1->GetTransform()->GetX(),
					gameobject1->GetTransform()->GetY(), gameobject1->GetRenderer()->GetWidth(),
					gameobject1->GetRenderer()->GetHeight() }, Rectangle{ gameobject2->GetTransform()->GetX(),
					gameobject2->GetTransform()->GetY(), gameobject2->GetRenderer()->GetWidth(),
					gameobject2->GetRenderer()->GetHeight() });

				if (collision1 || collision2)
				{
					isColliding = true;
				}
			}
		}
		if (!isColliding)
		{
			// Reset all colliders even if not collider so none of the bools are the wrong value
			m_ColliderList[i]->ResetCollision();
		}
	}

	// Collision Color Changer
	for (i = 0; i < m_ColliderColorChangerList.size(); i++)
	{
		bool hasCollided = m_ColliderColorChangerList[i]->Update(m_World[m_ColliderColorChangerList[i]->GetGameObjectID()]->GetCollider()->GetIsColliding());

		if (hasCollided)
		{
			// If you havent changed colors, switch. Otherwiese switch back
			if (m_ColliderColorChangerList[i]->GetIsDefaultColor())
			{
				m_World[m_ColliderColorChangerList[i]->GetGameObjectID()]->GetRenderer()->SetColor(m_ColliderColorChangerList[i]->GetDefaultColor());
			}
			else
			{
				m_World[m_ColliderColorChangerList[i]->GetGameObjectID()]->GetRenderer()->SetColor(m_ColliderColorChangerList[i]->GetNewColor());
			}
		}
	}
}

void WorldManager::Render()
{
	for (int i = 0; i < m_RendererList.size(); i++)
	{
		RectangleRenderer* renderer = m_RendererList[i];
		if (renderer == nullptr)
		{
			return;
		}
		else
		{
			if (m_World[renderer->GetGameObjectID()]->GetTransform() == NULL)
			{
				DrawRectangle(renderer->GetTopX(), renderer->GetTopY(), renderer->GetWidth(), renderer->GetHeight(), renderer->GetColor());
			}
			else
			{
				DrawRectangle(m_World[renderer->GetGameObjectID()]->GetTransform()->GetX(), m_World[renderer->GetGameObjectID()]->GetTransform()->GetY(), renderer->GetWidth(), renderer->GetHeight(), renderer->GetColor());
			}
		}
	}
}


// Level Serialization
int WorldManager::GetComponentValuesFromStream(std::string stream)
{
	// Gets rid of first value, which isn't our value, and returns an int converted from the string
	stream.erase(0, 1);
	return std::atoi(stream.c_str());
}

void WorldManager::LoadLevel()
{
	std::fstream file;
	file.open(WORLD_FILE_PATH + WORLD_FILE_NAME);

	if (file.is_open() && file.good())
	{
		char input = ' ';
		std::string currentInput;
		std::string previousInput;

		bool isEditingComponent = false;
		GameObject* tempGameObject = nullptr;
		int currentComponent = 0;

		/*
		0 - transform (x, y)
		1 - renderer (x, y, w, h, rgba)
		2 - collider
		3 - player
		4 - color collider (rgba)
		*/

		while (!file.eof())
		{
			file >> input;
			if (input == COMMENT_SYMBOL)
			{
				file.ignore(MAX_COMMENT_LENGTH, COMMENT_SYMBOL);
			}
			else if (isEditingComponent && currentInput.size() >= 2 && input == ';')
			{
				switch (currentComponent)
				{
				case 0:
					// Transform
					if (currentInput[0] == 'x')
					{
						int value = GetComponentValuesFromStream(currentInput);
						tempGameObject->GetTransform()->SetX(value);
					}
					else if (currentInput[0] == 'y')
					{
						int value = GetComponentValuesFromStream(currentInput);
						tempGameObject->GetTransform()->SetY(value);
					}
					break;

				case 1:
					// Renderer
					if (tempGameObject->GetRenderer() == nullptr)
					{
						break;
					}
					if (currentInput[0] == 'w')
					{
						int value = GetComponentValuesFromStream(currentInput);
						tempGameObject->GetRenderer()->SetWidth(value);
					}
					else if (currentInput[0] == 'h')
					{
						int value = GetComponentValuesFromStream(currentInput);
						tempGameObject->GetRenderer()->SetHeight(value);
					}
					else if (currentInput[0] == 'x')
					{
						int value = GetComponentValuesFromStream(currentInput);
						tempGameObject->GetRenderer()->SetTopLeftX(value);
					}
					else if (currentInput[0] == 'y')
					{
						int value = GetComponentValuesFromStream(currentInput);
						tempGameObject->GetRenderer()->SetTopLeftY(value);
					}
					else if (currentInput[0] == 'r')
					{
						int value = GetComponentValuesFromStream(currentInput);
						tempGameObject->GetRenderer()->SetRValue(value);
					}
					else if (currentInput[0] == 'g')
					{
						int value = GetComponentValuesFromStream(currentInput);
						tempGameObject->GetRenderer()->SetGValue(value);
					}
					else if (currentInput[0] == 'b')
					{
						int value = GetComponentValuesFromStream(currentInput);
						tempGameObject->GetRenderer()->SetBValue(value);
					}
					else if (currentInput[0] == 'a')
					{
						int value = GetComponentValuesFromStream(currentInput);
						tempGameObject->GetRenderer()->SetAValue(value);
					}

					break;

				case 2:
					// RectangleCollider has no values
					break;

				case 3:
					// PlayerController has no values
					break;

				case 4:
					// CollisionColorChanger
					if (tempGameObject->GetRenderer() == nullptr || tempGameObject->GetCollisionColorChanger() == nullptr) //NOTE: Ensures that the color changer is not null when trying to add values to it
					{
						break;
					}

					if (currentInput[0] = 'r')
					{
						int value = GetComponentValuesFromStream(currentInput);
						tempGameObject->GetCollisionColorChanger()->SetRValue(value);
					}
					else if (currentInput[0] = 'g')
					{
						int value = GetComponentValuesFromStream(currentInput);
						tempGameObject->GetCollisionColorChanger()->SetGValue(value);
					}
					else if (currentInput[0] = 'b')
					{
						int value = GetComponentValuesFromStream(currentInput);
						tempGameObject->GetCollisionColorChanger()->SetBValue(value);

					}
					else if (currentInput[0] = 'a')
					{
						int value = GetComponentValuesFromStream(currentInput);
						tempGameObject->GetCollisionColorChanger()->SetAValue(value);
					}

					break;
				}

				currentInput = "";
				continue;
			}
			else
			{
				if (input == '(' || input == ')' || input == '{' || input == '}')
				{
					if (input == '}')
					{
						isEditingComponent = false;
					}

					if (currentInput == "gameobject" && !isEditingComponent)
					{
						tempGameObject = NewGameObject();
						previousInput = currentInput;

					}
					else if (currentInput == "component" && !isEditingComponent)
					{
						previousInput = currentInput;
					}
					else
					{
						//NOTE: Using a switch for now to get it working:

						if (previousInput == "component")
						{
							int componentIndex = std::atoi(currentInput.c_str());

							currentComponent = componentIndex;

							if (tempGameObject != nullptr)
							{
								if (componentIndex == 0)
								{
									AddComponent(tempGameObject, transformComponent);
								}
								else if (componentIndex == 1)
								{
									AddComponent(tempGameObject, rectangleRendererComponent);
								}
								else if (componentIndex == 2)
								{
									AddComponent(tempGameObject, rectangleColliderComponent);
								}
								else if (componentIndex == 3)
								{
									AddComponent(tempGameObject, playerControllerComponent);
								}
								else if (componentIndex == 4)
								{
									AddComponent(tempGameObject, collisionColorChangerComponent);
									tempGameObject->GetCollisionColorChanger()->SetDefaultColor(tempGameObject->GetRenderer()->GetColor());
								}
							}

							isEditingComponent = true;
						}
						else
						{
							// Do nothing 
						}

						previousInput = currentInput;
					}

					currentInput = "";
					continue;
				}

				currentInput.push_back(input);
			}
		}
	}

	file.close();
}


// GameObject Helpers
void WorldManager::AddComponent(GameObject* gameobject, ComponentTypes type)
{
	switch (type)
	{
	case collisionColorChangerComponent:
		gameobject->SetColorChanger(AddToCollisionColorChangerPool());
		gameobject->GetCollisionColorChanger()->SetGameObjectID(gameobject->GetObjectID());

		if (gameobject->GetRenderer() != NULL)
		{
			gameobject->GetCollisionColorChanger()->SetDefaultColor(gameobject->GetRenderer()->GetColor());
		}
		break;

	case transformComponent:
		gameobject->SetTransform(AddToTransformPool());
		gameobject->GetTransform()->SetGameObjectID(gameobject->GetObjectID());
		break;

	case playerControllerComponent:
		gameobject->SetPlayerController(AddToPlayerControllerPool());
		gameobject->GetPlayerController()->SetGameObjectID(gameobject->GetObjectID());
		break;
	case rectangleColliderComponent:
		gameobject->SetCollider(AddToColliderPool());
		gameobject->GetCollider()->SetGameObjectID(gameobject->GetObjectID());
		break;

	case rectangleRendererComponent:
		gameobject->SetRenderer(AddToRendererPool());
		gameobject->GetRenderer()->SetGameObjectID(gameobject->GetObjectID());
		break;

	default:
		printf("no component found you are doing to die in 5 seconds");
		break;
	}
}

GameObject* WorldManager::NewGameObject()
{
	// Create new gameobject, give it an id, put in map, return
	GameObject* tempGameObject = new GameObject();
	tempGameObject->SetObjectID(GetNewObjectID());

	m_World.emplace(tempGameObject->GetObjectID(), tempGameObject);
	return tempGameObject;
}

void WorldManager::SpawnPlayer(NewTransform* transform)
{
	GameObject* player = NewGameObject();

	player->SetTransform(AddToTransformPool(transform->GetX() + 10, transform->GetY() + 10));
	player->GetTransform()->SetGameObjectID(player->GetObjectID());

	player->SetCollider(AddToColliderPool());
	player->GetCollider()->SetGameObjectID(player->GetObjectID());

	player->SetRenderer(AddToRendererPool(Vector2{ 80, 80 }, Vector2{ 800, 600 }, Color{ 255, 255, 0, 255 }));
	player->GetRenderer()->SetGameObjectID(player->GetObjectID());
}


// Object ID
int WorldManager::GetNewObjectID()
{
	m_ObjectIDIndex++;
	return m_ObjectIDIndex;
}


// Component Pool 
NewTransform* WorldManager::AddToTransformPool()
{
	NewTransform* temp = m_TransformComponentPool.New();
	m_TransformComponentList.push_back(temp);
	return temp;
}

NewTransform* WorldManager::AddToTransformPool(float x, float y)
{
	NewTransform* temp = m_TransformComponentPool.New(x, y);
	m_TransformComponentList.push_back(temp);
	return temp;
}

CollisionColorChanger* WorldManager::AddToCollisionColorChangerPool()
{
	CollisionColorChanger* temp = m_ColliderColorChangerPool.New();
	m_ColliderColorChangerList.push_back(temp);
	return temp;
}

CollisionColorChanger* WorldManager::AddToCollisionColorChangerPool(Color newColor, Color defaultColor)
{
	CollisionColorChanger* temp = m_ColliderColorChangerPool.New(newColor, defaultColor);
	m_ColliderColorChangerList.push_back(temp);
	return temp;
}

PlayerController* WorldManager::AddToPlayerControllerPool()
{
	PlayerController* temp = m_PlayerControllerPool.New();
	m_PlayerControllerList.push_back(temp);
	return temp;
}

RectangleCollider* WorldManager::AddToColliderPool()
{
	RectangleCollider* temp = m_ColliderPool.New();
	m_ColliderList.push_back(temp);
	return temp;
}

RectangleRenderer* WorldManager::AddToRendererPool()
{
	RectangleRenderer* temp = m_RendererPool.New();
	m_RendererList.push_back(temp);
	return temp;
}

RectangleRenderer* WorldManager::AddToRendererPool(float width, float height, float leftX, float leftY, Color color)
{
	RectangleRenderer* temp = m_RendererPool.New(width, height, leftX, leftY, color);
	m_RendererList.push_back(temp);
	return temp;
}

RectangleRenderer* WorldManager::AddToRendererPool(Vector2 size, Vector2 left, Color color) // Only for background
{
	RectangleRenderer* temp = m_RendererPool.New(size.x, size.y, left.x, left.y, color);
	m_RendererList.push_back(temp);
	return temp;
} 