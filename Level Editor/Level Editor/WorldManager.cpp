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


char input[20];
static ImVec4 myColor = { 1, 1, 1, 255 };
static ImVec4 goColor = { 1, 1, 1, 255 };
int x = 0;
int y = 0;

bool foo[4] = { false, false, false, false };


// Loop
void WorldManager::GameLoop()
{
	int frame = 0;
	float targetTime = 16.67;

	Color col = { myColor.x * 255, myColor.y * 255, myColor.z * 255, 255 };

	ClearBackground(col);


	while (!WindowShouldClose())
	{
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

		BeginDrawing();


		// Put this somewhere else pls
		rlImGuiBegin();

		// User input
		if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
		{
			printf("hello!!!!!!");
			SpawnGameObjectOnMouse();
		}

		// render imgui content
		ImGui::Begin("Editor", NULL);

		// Pick background color
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "Background Color");
		ImGui::SameLine();
		ImGui::ColorEdit4("Color", (float*)&myColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

		// Save button
		if (ImGui::Button("Save Game"))
			SaveButton();

		// Clear world button
		if (ImGui::Button("Clear World"))
			ClearWorld();
		
		// Remove All Transforms
		/*if (ImGui::Button("Kill Transforms"))
			DeleteAllGOOfType(transformComponent);*/

		


		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Input File Name To Load");

		ImGui::InputText("Enter Text", input, IM_ARRAYSIZE(input));
		//ImGui::SameLine();
		if (ImGui::Button("Load Level"))
		{
			levelPath.assign(input);
			LoadLevel();

		}


		// Show other UI
		if (ImGui::Button("Show UI"))
			ToggleUI();

		ImGui::End();



		if (m_ShowGameObjectEditor)
		{
			// TODO: Edit specific gameobject
			GameObject* clone = new GameObject();
			Color temp;

			ImGui::Begin("Object Editor", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

			// Pick GO Color
			ImGui::TextColored(ImVec4(1, 1, 1, 1), "Color");
			ImGui::SameLine();
			ImGui::ColorEdit4("Color", (float*)&goColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

			ImGui::InputInt("Transform X", &x);
			ImGui::InputInt("Transform Y", &y);

			NewTransform* tempPos = new NewTransform(x, y);
			clone->SetTransform(tempPos);


			// Player
			ImGui::Checkbox("Player", &foo[0]);
			if (foo[0] == true)
				AddComponent(clone, playerControllerComponent);
			else
				RemoveComponent(clone, playerControllerComponent);
				
			// Renderer
			ImGui::Checkbox("Renderer", &foo[1]);
			if (foo[1] == true)
				AddComponent(clone, rectangleRendererComponent);
			else
				RemoveComponent(clone, rectangleRendererComponent);

			// Collider
			ImGui::Checkbox("Collider", &foo[2]);
			if (foo[2] == true)
				AddComponent(clone, rectangleColliderComponent);
			else
				RemoveComponent(clone, rectangleColliderComponent);

			// Color Changer
			ImGui::Checkbox("Color Changer", &foo[3]);
			if (foo[3] == true)
				AddComponent(clone, collisionColorChangerComponent);
			else
				RemoveComponent(clone, collisionColorChangerComponent);

			// Delete GameObject
			if (ImGui::Button("Delete this object"))
				ToggleUI();


			ImGui::End();
		}





		// Change BG Color
		Color col = { myColor.x * 255, myColor.y * 255, myColor.z * 255, 255 };
		ClearBackground(col);




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

	// Debug
	m_ShowGameObjectEditor = false;

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
		m_PlayerControllerList[i]->Update(m_World[m_PlayerControllerList[i]->GetGameObjectID()]->GetTransform());
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
	ClearWorld();

	std::fstream file;
	file.open(levelPath);

	if (file.fail())
	{
		printf("", levelPath, " does not exist!\n");
	}

	if (file.is_open() && file.good())
	{
		printf("Loading level: %i", levelPath, "\n");

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


void WorldManager::SaveWorld()
{
	// If nothing was loaded, don't save
	if (m_World.size() <= 1)
	{
		return;
	}

	std::ofstream file(levelPath);

	if (file.is_open() && file.good())
	{
		// Puts gameobjects in file
		int i;
		for (i = 3; i <= m_World.size(); i++) //Loops through the world, and generate a file with the data
		{
			file << "gameobject\n";
			file << "{";

			if (m_World[i]->GetTransform() != nullptr)
			{
				file << "\tcomponent(";
				file << "0)\n";
				file << "\t{\n";

				//Component values:
				file << "\tx";
				file << m_World[i]->GetTransform()->GetX();
				file << ";\n";

				file << "\ty";
				file << m_World[i]->GetTransform()->GetY();
				file << ";\n";

				file << "\t}\n";

			}

			if (m_World[i]->GetRenderer() != nullptr)
			{
				bool changedColor = false;

				if (m_World[i]->GetCollisionColorChanger() && m_World[i]->GetCollider()->GetIsColliding()) //Checks to see if a color changing object is currently changed
				{
					m_World[i]->GetRenderer()->SetColor(m_World[i]->GetCollisionColorChanger()->GetDefaultColor()); //Ensures that the default color of the renderer will be saved
					changedColor = true;
				}

				file << "\tcomponent(";
				file << "1)\n";
				file << "\t{\n";

				//Component values:

				file << "\tw";
				file << m_World[i]->GetRenderer()->GetWidth();
				file << ";\n";

				file << "\th";
				file << m_World[i]->GetRenderer()->GetHeight();
				file << ";\n";

				file << "\tx";
				file << m_World[i]->GetRenderer()->GetTopX();
				file << ";\n";

				file << "\ty";
				file << m_World[i]->GetRenderer()->GetTopY();
				file << ";\n";

				Color color = m_World[i]->GetRenderer()->GetColor();

				file << "\tr";
				file << int(color.r);
				file << ";\n";

				file << "\tg";
				file << int(color.g);
				file << ";\n";

				file << "\tb";
				file << int(color.b);
				file << ";\n";

				file << "\ta";
				file << int(color.a);
				file << ";\n";

				file << "\t}\n";

				if (changedColor)
				{
					m_World[i]->GetRenderer()->SetColor(m_World[i]->GetCollisionColorChanger()->GetNewColor()); //Resets the color back to what it's suppost to display on the screen. 
				}
			}

			if (m_World[i]->GetCollider() != nullptr)
			{
				file << "\tcomponent(";
				file << "2)\n";
				file << "\t{\n";

				file << "\t}\n";
			}

			if (m_World[i]->GetPlayerController() != nullptr)
			{
				file << "\tcomponent(";
				file << "3)\n";
				file << "\t{\n";

				file << "\t}\n";
			}

			if (m_World[i]->GetCollisionColorChanger() != nullptr)
			{
				file << "\tcomponent(";
				file << "4)\n";
				file << "\t{\n";

				//Component values:

				Color color = m_World[i]->GetCollisionColorChanger()->GetNewColor();

				file << "\tr";
				file << int(color.r);
				file << ";\n";

				file << "\tg";
				file << int(color.g);
				file << ";\n";

				file << "\tb";
				file << int(color.b);
				file << ";\n";

				file << "\ta";
				file << int(color.a);
				file << ";\n";

				file << "\t}\n";
			}

			file << "}\n";
		}
		file.close();
	}
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

void WorldManager::RemoveComponent(GameObject* gameobject, ComponentTypes type)
{
	if (gameobject == NULL)
	{
		return;
	}

	switch (type)
	{
	case collisionColorChangerComponent:
		if (gameobject->HasComponent(type))
		{
			gameobject->RemoveColorChanger();
			RemoveFromCollisionColorChangerPool(gameobject);
		}
		break;

	case transformComponent:
		if (gameobject->HasComponent(type))
		{
			gameobject->RemoveTransform();
			RemoveFromTransformPool(gameobject);
		}
		break;

	case playerControllerComponent:
		if (gameobject->HasComponent(type))
		{
			gameobject->RemovePlayerController();
			RemoveFromPlayerControllerPool(gameobject);
		}
		break;

	case rectangleColliderComponent:
		if (gameobject->HasComponent(type))
		{
			gameobject->RemoveCollider();
			RemoveFromColliderPool(gameobject);
		}
		break;

	case rectangleRendererComponent:
		if (gameobject->HasComponent(type))
		{
			gameobject->RemoveRenderer();
			RemoveFromRendererPool(gameobject);
		}
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

void WorldManager::ClearWorld()
{
	/*
	to delete an object we need to:
	delete the gameobject itself
	remove from pool and list
	*/

	m_TransformComponentList.clear();
	m_TransformComponentPool.ClearPool();

	m_PlayerControllerList.clear();
	m_PlayerControllerPool.ClearPool();

	m_RendererList.clear();
	m_RendererPool.ClearPool();

	m_ColliderList.clear();
	m_ColliderPool.ClearPool();

	m_ColliderColorChangerList.clear();
	m_ColliderColorChangerPool.ClearPool();

	m_World.clear();
	printf("done clearing\n");
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


void WorldManager::RemoveFromTransformPool(GameObject* gameobject)
{
	m_TransformComponentPool.Delete(gameobject->GetTransform());
}

void WorldManager::RemoveFromCollisionColorChangerPool(GameObject* gameobject)
{
	m_ColliderColorChangerPool.Delete(gameobject->GetCollisionColorChanger());
}

void WorldManager::RemoveFromPlayerControllerPool(GameObject* gameobject)
{
	m_PlayerControllerPool.Delete(gameobject->GetPlayerController());
}

void WorldManager::RemoveFromColliderPool(GameObject* gameobject)
{
	m_ColliderPool.Delete(gameobject->GetCollider());
}

void WorldManager::RemoveFromRendererPool(GameObject* gameobject)
{
	m_RendererPool.Delete(gameobject->GetRenderer());
	//m_RendererList.erase(gameobject->GetRenderer());
}



// Debug
void WorldManager::SaveButton()
{
	SaveWorld();
	printf("testing 123 \n");
}

void WorldManager::DeleteAllGOOfType(ComponentTypes type)
{
	//m_TransformComponentList.clear();
	//m_TransformComponentPool.ClearPool();

	//for (auto it = m_World.begin(); it != m_World.end(); )
	//{
	//	if (it->second->ContainsComponentInList(type))
	//	{
	//		it = m_World.erase(it); // erase returns an iterator to the next element
	//	}
	//	else
	//	{
	//		++it; // only increment if no element is erased
	//	}
	//}

}

void WorldManager::SpawnGameObjectOnMouse()
{
	Vector2 pos = GetMousePosition();

	GameObject* tempGO = NewGameObject();
	tempGO->SetTransform(AddToTransformPool(pos.x, pos.y));
	tempGO->SetRenderer(AddToRendererPool());
}



//
//bool WorldManager::CheckMouseCollision()
//{
//	if (CheckCollisionPointRec(GetMousePosition(), (Rectangle) { 0, 0, GetScreenWidth(), GetScreenHeight() })
//	{
//		// Mouse inside the screen bounds
//	}
//}

void WorldManager::ToggleUI()
{
	m_ShowGameObjectEditor = !m_ShowGameObjectEditor;
}



