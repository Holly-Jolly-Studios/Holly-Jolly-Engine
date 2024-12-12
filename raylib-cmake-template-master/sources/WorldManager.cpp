#include "WorldManager.h"

const int MAX_COMPONENT_AMOUNT = 20;

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
	m_NewTransformPool.SetSize(MAX_COMPONENT_AMOUNT);
	m_CollisionColorChangerPool.SetSize(MAX_COMPONENT_AMOUNT);
	m_PlayerControllerPool.SetSize(MAX_COMPONENT_AMOUNT);
	m_ColliderPool.SetSize(MAX_COMPONENT_AMOUNT);
	m_RendererPool.SetSize(MAX_COMPONENT_AMOUNT);

	// BG
	GameObject* background = new GameObject;
	background->SetObjectID(GetNewGOObjectID());

	background->SetTransform(CreateNewTransformPool(0, 0));
	background->GetTransform()->SetGameObjectID(background->GetObjectID());

	background->SetRenderer(CreateNewRendererPool(13000, 8000, -6000, 0, Color{ 0, 50, 25, 255 }));
	background->GetRenderer()->SetGameObjectID(background->GetObjectID());

	// Player
	GameObject* player = new GameObject;
	player->SetObjectID(GetNewGOObjectID());

	player->SetTransform(CreateNewTransformPool(500, 500));
	player->GetTransform()->SetGameObjectID(player->GetObjectID());

	/*player->SetPlayerController(CreateNewControllerPool());
	player->GetPlayerController()->SetGameObjectID(player->GetObjectID());*/

	player->SetCollider(CreateNewColliderPool());
	player->GetCollider()->SetGameObjectID(player->GetObjectID());

	player->SetRenderer(CreateNewRendererPool(40, 40, 400, 280, Color{ 0, 100, 255, 255 }));
	player->GetRenderer()->SetGameObjectID(player->GetObjectID());

	player->SetColorChanger(CreateNewColorChangerPool(Color{ 255, 0, 0, 255 }, Color{ 0, 100, 255, 255 }));
	player->GetCollisionColorChanger()->SetGameObjectID(player->GetObjectID());

	//m_World.emplace(player->GetObjectID(), player);


	LoadWorld();
}

WorldManager::~WorldManager()
{
	// Do nothing
}


void WorldManager::UpdateWorld()
{
	int i, j;

	// Update player controller
	for (i = 0; i < m_PlayerControllerList.size(); i++)
	{
		
		int players = m_PlayerControllerList[i]->Update(m_World[m_PlayerControllerList[i]->GetGameObjectID()]->GetTransform());
		//int players = m_PlayerControllerList.size();

		// Check player count, if more than 1 delete the extra
		if (players == 0)
		{
			// do nothing
		}
		else if (players == 1)
		{
			int temp1 = m_PlayerControllerList[i]->GetGameObjectID();
			auto temp2 = m_World[temp1];

			SpawnPlayerGameObject(m_World[m_PlayerControllerList[i]->GetGameObjectID()]->GetTransform());
		}
		else
		{
			DeleteFromWorld(m_World[GetClosestGO(m_World[m_PlayerControllerList[i]->GetGameObjectID()]->GetTransform())->GetObjectID()]);
		}
	}

	//Check for collisions
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
		bool colliding = m_CollisionColorChangerList[i]->Update(m_World[m_CollisionColorChangerList[i]->GetGameObjectID()]->GetCollider()->GetIsColliding());

		if (colliding)
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
		RectangleRenderer* renderer = m_RendererList[i];
		if (renderer != nullptr)
		{
			if (m_World[renderer->GetGameObjectID()]->GetTransform() != NULL)
			{
				DrawRectangle(m_World[renderer->GetGameObjectID()]->GetTransform()->GetX(), m_World[renderer->GetGameObjectID()]->GetTransform()->GetY(), renderer->GetWidth(), renderer->GetHeight(), renderer->GetColor());
			}
			else
			{
				DrawRectangle(renderer->GetTopLeftX(), renderer->GetTopLeftY(), renderer->GetWidth(), renderer->GetHeight(), renderer->GetColor());
			}
		}
	}
}

void WorldManager::ClearWorld()
{
	// clear
}

int WorldManager::GetNewGOObjectID()
{
	m_ObjectIDIndex++;
	return m_ObjectIDIndex;
}

void WorldManager::DebugFrames()
{
	if (IsKeyDown(KEY_F))
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
	GameObject* player = new GameObject;
	player->SetObjectID(GetNewGOObjectID());

	player->SetTransform(CreateNewTransformPool(playerTransform->GetX() + 10, playerTransform->GetY() + 10));
	player->GetTransform()->SetGameObjectID(player->GetObjectID());

	player->SetPlayerController(CreateNewControllerPool());
	player->GetPlayerController()->SetGameObjectID(player->GetObjectID());

	player->SetCollider(CreateNewColliderPool());
	player->GetCollider()->SetGameObjectID(player->GetObjectID());

	player->SetRenderer(CreateNewRendererPool(80, 80, 800, 600, Color{ 255, 255, 0, 255 }));
	player->GetRenderer()->SetGameObjectID(player->GetObjectID());

	m_World.emplace(player->GetObjectID(), player);
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

const std::string WORLD_FILE_NAME = "worldTest.txt";
const std::string WORLD_FILE_PATH = "../../../";
const int MAX_COMMENT_LENGTH = 256;
const char COMMENT = '#';


void WorldManager::LoadWorld()
{
	std::fstream file;
	file.open(WORLD_FILE_PATH + WORLD_FILE_NAME);

	if (file.is_open() && file.good())
	{
		char input = ' ';
		std::string currentInput;
		std::string previousInput;

		bool isEditingComponent = false;
		int currentComponentIndex = 0;
		GameObject* currentGameObject = nullptr;

		while (!file.eof())
		{
			file >> input;

			// If input is comment char then ignore whats inside
			if (input == COMMENT)
			{
				file.ignore(MAX_COMMENT_LENGTH, COMMENT);
			}

			// If component exists and your stream isn't empty and if end of value (indicated with ;)
			else if (isEditingComponent && currentInput.size() >= 2 && input == ';')
			{
				switch (currentComponentIndex)
				{
				// Transform
				case 0:
					if (currentInput[0] == 'x')
					{
						int value = GetComponentValue(currentInput);
						currentGameObject->GetTransform()->SetX(value);
						printf("%i Setting x in transform to: ", value);
					}
					else if (currentInput[0] == 'y')
					{
						int value = GetComponentValue(currentInput);
						currentGameObject->GetTransform()->SetY(value);
						printf("%i Setting y in transform to: ", value);
					}
					else
					{
						int value = GetComponentValue(currentInput);
						printf("%i Invalid entry: ", value);
					}
					
					break;

				// Player controller
				case 1:
					// Do nothing, no values needed
					break;

				// Renderer
				case 2: 
					if (currentGameObject->GetRenderer() == nullptr)
					{
						break;
					}

					// Based on value change either width, height, r, g, b, or a
					if (currentInput[0] == 'w')
					{
						int value = GetComponentValue(currentInput);
						currentGameObject->GetRenderer()->SetWidth(value);
						printf("%i Setting width in renderer to: ", value);
					}
					else if (currentInput[0] == 'h')
					{
						int value = GetComponentValue(currentInput);
						currentGameObject->GetRenderer()->SetHeight(value);
						printf("%i Setting height in renderer to: ", value);
					}
					else if (currentInput[0] == 'x')
					{
						int value = GetComponentValue(currentInput);
						currentGameObject->GetRenderer()->SetTopLeftX(value);
						printf("%i Setting x in renderer to: ", value);
					}
					else if (currentInput[0] == 'y')
					{
						int value = GetComponentValue(currentInput);
						currentGameObject->GetRenderer()->SetTopLeftY(value);
						printf("%i Setting y in renderer to: ", value);
					}
					else if (currentInput[0] == 'r')
					{
						int value = GetComponentValue(currentInput);
						currentGameObject->GetRenderer()->SetRValue(value);
						printf("%i Setting r in renderer to: ", value);
					}
					else if (currentInput[0] == 'g')
					{
						int value = GetComponentValue(currentInput);
						currentGameObject->GetRenderer()->SetGValue(value);
						printf("%i Setting g in renderer to: ", value);
					}
					else if (currentInput[0] == 'b')
					{
						int value = GetComponentValue(currentInput);
						currentGameObject->GetRenderer()->SetBValue(value);
						printf("%i Setting b in renderer to: ", value);
					}
					else if (currentInput[0] == 'a')
					{
						int value = GetComponentValue(currentInput);
						currentGameObject->GetRenderer()->SetAValue(value);
						printf("%i Setting a in renderer to: ", value);
					}
					else
					{
						int value = GetComponentValue(currentInput);
						printf("%i Invalid entry: ", value);
					}
					
					break;
			
				// Collision
				case 3:
					// do nothing, no values needed
					break;

				// Color Changer
				case 4: 
					// If no renderer or color changer then break
					if (currentGameObject->GetRenderer() == nullptr || currentGameObject->GetCollisionColorChanger() == nullptr)
					{
						break;
					}

					if (currentInput[0] == 'r')
					{
						int value = GetComponentValue(currentInput);
						currentGameObject->GetCollisionColorChanger()->SetRValue(value);
						printf(" %i Setting r in color changer to: ", value);
					}
					else if (currentInput[0] == 'g')
					{
						int value = GetComponentValue(currentInput);
						currentGameObject->GetCollisionColorChanger()->SetGValue(value);
						printf(" %i Setting g in color changer to: ", value);
					}
					else if (currentInput[0] == 'b')
					{
						int value = GetComponentValue(currentInput);
						currentGameObject->GetCollisionColorChanger()->SetBValue(value);
						printf(" %i Setting b in color changer to: ", value);
					}
					else if (currentInput[0] == 'a')
					{
						int value = GetComponentValue(currentInput);
						currentGameObject->GetCollisionColorChanger()->SetAValue(value);
						printf(" %i Setting a in color changer to: ", value);
					}
					else
					{
						int value = GetComponentValue(currentInput);
						printf(" %i  Invalid entry: ", value);
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
						//currentLayer = 1;
						printf("\nCreating new GameObject\n");
						currentGameObject = CreateEmptyGO();
						previousInput = currentInput;

					}
					else if (currentInput == "component" && !isEditingComponent)
					{
						//currentLayer = 2;
						printf("\nCreating new component\n");

						previousInput = currentInput;
					}
					else
					{
						//if (previousInput == "name")
						//{
						//	// TODO: give this guy a name
						//	std::string NAME = "";
						//	currentGameObject->SetName(NAME);
						//}
						if (previousInput == "component")
						{
							int componentID = std::atoi(currentInput.c_str());

							currentComponentIndex = componentID;

							// Only do if go exists
							if (currentGameObject != nullptr)
							{
								/*
								0 - transform
								1 - player
								2 - rect rend
								3 - rect col
								4 - color col
								*/
								switch (componentID)
								{
								case 0: 
									printf("\nCreating transform component on the given gameobject\n");
									AddNewComponent(currentGameObject, transformComponent);

									break;

								case 1: 
									printf("\nCreating player controller component on the given gameobject\n");
									AddNewComponent(currentGameObject, playerControllerComponent);
									break;
									

								case 2:
									printf("\nCreating rectangle render component on the given gameobject\n");
									AddNewComponent(currentGameObject, rectangleRendererComponent);

									break;
									

								case 3:
									printf("\nCreating rectangle collider component on the given gameobject\n");
									AddNewComponent(currentGameObject, rectangleColliderComponent);
									break;

								case 4: 
									printf("\nCreating color changer component on the given gameobject\n");
									AddNewComponent(currentGameObject, collisionColorChangerComponent);

									break;

								default:
									printf("\nNo component found :( \n");
									break;
								}
							}

							isEditingComponent = true;
						}
						else
						{
							// do nothing otherwise
						}

						previousInput = currentInput;
					}

					currentInput = "";
					continue;
				}

				currentInput.push_back(input);

				printf(" %i Input: ", input);
			}
		}
	}

	file.close();
}


int WorldManager::GetComponentValue(std::string stream)
{
	// Gets rid of first value, which isn't our value, and returns a int converted from our "string"
	stream.erase(0, 1);
	return std::atoi(stream.c_str());
}


#pragma region Game Object Components

GameObject* WorldManager::CreateEmptyGO()
{
	// Create object, give it an ID, put in world, return object
	GameObject* gameobject = new GameObject();
	gameobject->SetObjectID(GetNewGOObjectID());
	m_World.emplace(gameobject->GetObjectID(), gameobject);
	return gameobject;
}

void WorldManager::AddNewComponent(GameObject* gameobject, ComponentTypes type)
{
	switch (type)
	{
	case collisionColorChangerComponent:
		gameobject->SetColorChanger(CreateNewColorChangerPool());
		gameobject->GetCollisionColorChanger()->SetGameObjectID(gameobject->GetObjectID());

		if (gameobject->GetRenderer() != NULL)
		{
			gameobject->GetCollisionColorChanger()->SetDefaultColor(gameobject->GetRenderer()->GetColor());
		}
		break;

	case transformComponent:
		gameobject->SetTransform(CreateNewTransformPool());
		gameobject->GetTransform()->SetGameObjectID(gameobject->GetObjectID());
		break;

	case playerControllerComponent:
		gameobject->SetPlayerController(CreateNewControllerPool());
		gameobject->GetPlayerController()->SetGameObjectID(gameobject->GetObjectID());
		break;
	case rectangleColliderComponent:
		gameobject->SetCollider(CreateNewColliderPool());
		gameobject->GetCollider()->SetGameObjectID(gameobject->GetObjectID());
		break;

	case rectangleRendererComponent:
		gameobject->SetRenderer(CreateNewRendererPool());
		gameobject->GetRenderer()->SetGameObjectID(gameobject->GetObjectID());
		break;

	default:
		printf("no component found you are doing to die in 5 seconds");
		break;
	}
}

#pragma endregion





#pragma region Component pools

NewTransform* WorldManager::CreateNewTransformPool()
{
	NewTransform* t = m_NewTransformPool.New();
	m_NewTransformList.push_back(t);
	return t;
}

NewTransform* WorldManager::CreateNewTransformPool(float x, float y)
{
	NewTransform* t = m_NewTransformPool.New(x, y);
	m_NewTransformList.push_back(t);
	printf(" transform pool pushing back: ", t);
	return t;
}

PlayerController* WorldManager::CreateNewControllerPool()
{
	PlayerController* p = m_PlayerControllerPool.New();
	m_PlayerControllerList.push_back(p);
	return p;
}

RectangleRenderer* WorldManager::CreateNewRendererPool()
{
	RectangleRenderer* r = m_RendererPool.New();
	m_RendererList.push_back(r);
	return r;
}

RectangleRenderer* WorldManager::CreateNewRendererPool(float width, float height, float topLeftX, float topLeftY, Color color)
{
	RectangleRenderer* r = m_RendererPool.New(width, height, topLeftX, topLeftY, color);
	m_RendererList.push_back(r);
	return r;
}

RectangleCollider* WorldManager::CreateNewColliderPool()
{
	RectangleCollider* r = m_ColliderPool.New();
	m_ColliderList.push_back(r);
	return r;
}

CollisionColorChanger* WorldManager::CreateNewColorChangerPool()
{
	CollisionColorChanger* c = m_CollisionColorChangerPool.New();
	m_CollisionColorChangerList.push_back(c);
	return c;
}

CollisionColorChanger* WorldManager::CreateNewColorChangerPool(Color changedColor, Color defaultColor)
{
	CollisionColorChanger* c = m_CollisionColorChangerPool.New(changedColor, defaultColor);
	m_CollisionColorChangerList.push_back(c);
	return c;
}


#pragma endregion

