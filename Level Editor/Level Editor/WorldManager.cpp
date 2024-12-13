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
char saveInput[20];
static ImVec4 myColor = { 1, 1, 1, 255 };
static ImVec4 goColor = { 1, 1, 1, 255 };
int x = 0;
int y = 0;
float width = 0;
float height = 0;

bool foo[4] = { false, false, false, false };

std::string tempFilePath = "";

std::string items[] = { "a", "b", "c" }; // defined somewhere
int selectedIndex = 0; // you need to store this state somewher

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
		if(IsKeyPressed(KEY_SPACE))
		{
			SpawnGameObjectOnMouse();
		}

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			if (!m_ShowGameObjectEditor) {
				for (auto i = m_World.begin(); i != m_World.end(); i++) {
					GameObject* go = i->second;
					Rectangle rect;
					rect.x = go->GetTransform()->GetX();
					rect.y = go->GetTransform()->GetY();
					rect.width = go->GetRenderer()->GetWidth();
					rect.height = go->GetRenderer()->GetHeight();

					if (CheckMouseCollision(rect)) {
						m_SelectedGO = i->second;
						x = m_SelectedGO->GetTransform()->GetX();
						y = m_SelectedGO->GetTransform()->GetY();
						width = m_SelectedGO->GetRenderer()->GetWidth();
						height = m_SelectedGO->GetRenderer()->GetHeight();
						Color temp = m_SelectedGO->GetRenderer()->GetColor();
						goColor = { (float)temp.r/255, (float)temp.g/255, (float)temp.b/255, 255};
						foo[0] = m_SelectedGO->HasComponent(playerControllerComponent);
						OpenEditUI();
					}
				}
			}
		}

		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && m_SelectedGO != NULL)
		{
			Vector2 pos = GetMousePosition();
			x = pos.x - (width / 2);
			y = pos.y - (height / 2);
		}

		// render imgui content
		ImGui::Begin("Editor", NULL);

		// World Edits
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "World Edit");
	
		ImGui::TextColored(ImVec4(1, 1, 1, 1), "Background Color");
		ImGui::SameLine();
		ImGui::ColorEdit4("Color", (float*)&myColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

		if (ImGui::Button("Clear World"))
			ClearWorld();


		// Save
		ImGui::TextColored(ImVec4(0, 0, 0, 0), "");
		ImGui::TextColored(ImVec4(1, 1, 1, 1), "Save Level");

		ImGui::InputText("  ", saveInput, IM_ARRAYSIZE(saveInput));
		if (ImGui::Button("Save Game"))
		{
			saveFileName.assign(saveInput);
			SaveButton();
		}
		

		// Load
		ImGui::TextColored(ImVec4(1, 1, 1, 1), "Load Level");

		ImGui::InputText(" ", input, IM_ARRAYSIZE(input));
		if (ImGui::Button("Load Level"))
		{
			levelPath.assign(input);
			LoadLevel(input);
		}


		// Tips
		ImGui::TextColored(ImVec4(0, 0, 0, 0), "");
		ImGui::TextColored(ImVec4(1, 1, 1, 1), "Space to spawn an object");
		ImGui::TextColored(ImVec4(1, 1, 1, 1), "LClick to select");
		ImGui::TextColored(ImVec4(1, 1, 1, 1), "RClick + drag to move selected");

		ImGui::End();



		if (m_ShowGameObjectEditor)
		{
			ImGui::Begin("Object Editor", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

			// Pick GO Color
			ImGui::TextColored(ImVec4(1, 1, 1, 1), "Color");
			ImGui::SameLine();
			ImGui::ColorEdit3("Color", (float*)&goColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

			ImGui::SliderInt("Transform X", &x, 0, 800);
			ImGui::SliderInt("Transform Y", &y, 0, 600);

			ImGui::SliderFloat("Renderer W", &width, 0, 200);
			ImGui::SliderFloat("Renderer H", &height, 0, 200);

			m_SelectedGO->GetTransform()->SetX(x);
			m_SelectedGO->GetTransform()->SetY(y);

			m_SelectedGO->GetRenderer()->SetWidth(width);
			m_SelectedGO->GetRenderer()->SetHeight(height);

			Color tempCol = { goColor.x * 255, goColor.y * 255, goColor.z * 255, 255 };
			m_SelectedGO->GetRenderer()->SetColor(tempCol);


			// Player
			ImGui::Checkbox("Player", &foo[0]);
			if (foo[0] && !m_SelectedGO->HasComponent(playerControllerComponent))
				AddComponent(m_SelectedGO, playerControllerComponent);
			// the object will become a player forever but if you uncomment this the world will end
			//else if (!foo[0] && m_SelectedGO->HasComponent(playerControllerComponent))
				//RemoveComponent(m_SelectedGO, playerControllerComponent);

			// Delete GameObject
			if (ImGui::Button("Delete this object"))
			{
				DeleteSelected();
				CloseEditUI();
			}

			// Close UI
			if (ImGui::Button("Close"))
				CloseEditUI();


			ImGui::End();
		}

		// Change BG Color
		Color col = { myColor.x * 255, myColor.y * 255, myColor.z * 255, 255 };
		ClearBackground(col);

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
	m_Saving = false;

	LoadLevel(levelPath + fileExtention);
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

void WorldManager::LoadLevel(std::string fileToOpen)
{
	ClearWorld();

	std::fstream fin;
	fin.open(fileToOpen + fileExtention);

	if (fin.is_open() && fin.good())
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

		while (!fin.eof())
		{
			fin >> input;
			if (input == COMMENT_SYMBOL)
			{
				fin.ignore(MAX_COMMENT_LENGTH, COMMENT_SYMBOL);
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
					// RectangleCollider
					break;

				case 3:
					// PlayerController
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
				if (input == '[' || input == ']' || input == '{' || input == '}')
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
						if (previousInput == "component")
						{
							int componentNum = std::atoi(currentInput.c_str());

							currentComponent = componentNum;

							if (tempGameObject != nullptr)
							{
								if (componentNum == 0)
								{
									AddComponent(tempGameObject, transformComponent);
								}
								else if (componentNum == 1)
								{
									AddComponent(tempGameObject, rectangleRendererComponent);
								}
								else if (componentNum == 2)
								{
									AddComponent(tempGameObject, rectangleColliderComponent);
								}
								else if (componentNum == 3)
								{
									AddComponent(tempGameObject, playerControllerComponent);
								}
								else if (componentNum == 4)
								{
									AddComponent(tempGameObject, collisionColorChangerComponent);
									tempGameObject->GetCollisionColorChanger()->SetDefaultColor(tempGameObject->GetRenderer()->GetColor());
								}
							}

							isEditingComponent = true;
						}
						else
						{
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
	fin.close();
}

void WorldManager::SaveWorld(std::string fileToSave)
{
	// If nothing is in scene don't save anything
	if (m_World.size() == 0)
		return;


	std::ofstream fout(fileToSave + fileExtention);

	if (fout.is_open() && fout.good())
	{
		for (auto i = m_World.begin(); i != m_World.end(); i++)
		{
			fout << "gameobject" << std::endl << "{" << std::endl;

			if (i->second->GetTransform() != nullptr)
			{
				fout << "component[0] {";

				fout << " x" << i->second->GetTransform()->GetX() << "; ";
				fout << " y" << i->second->GetTransform()->GetY() << "; }" << std::endl;
			}

			if (i->second->GetRenderer() != nullptr)
			{
				bool newColor = false;

				if (i->second->GetCollisionColorChanger() && i->second->GetCollider()->GetIsColliding())
				{
					i->second->GetRenderer()->SetColor(i->second->GetCollisionColorChanger()->GetDefaultColor());
					newColor = true;
				}

				Color color = i->second->GetRenderer()->GetColor();

				if (newColor)
					i->second->GetRenderer()->SetColor(i->second->GetCollisionColorChanger()->GetNewColor());

				fout << "component[1] {";

				fout << " w" << i->second->GetRenderer()->GetWidth() << "; ";
				fout << " h" << i->second->GetRenderer()->GetHeight() << "; ";
				fout << " x" << i->second->GetRenderer()->GetTopX() << "; ";
				fout << " y" << i->second->GetRenderer()->GetTopY() << "; ";
				fout << " r" << int(color.r) << "; ";
				fout << " g" << int(color.g) << "; ";
				fout << " b" << int(color.b) << "; ";
				fout << " a" << int(color.a) << "; }" << std::endl;
			}

			if (i->second->GetCollider() != nullptr)
			{
				fout << "component[2] {}" << std::endl;
			}

			if (i->second->GetPlayerController() != nullptr)
			{
				fout << "component[3] {}" << std::endl;
			}

			if (i->second->GetCollisionColorChanger() != nullptr)
			{
				Color color = i->second->GetCollisionColorChanger()->GetNewColor();

				fout << "component[4] { ";

				fout << " r" << int(color.r) << "; ";
				fout << " g" << int(color.g) << "; ";
				fout << " b" << int(color.b) << "; ";
				fout << " a" << int(color.a) << "; }" << std::endl;
			}

			fout << "}" << std::endl << std::endl;
		}
		fout.close();
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
			gameobject->RemoveColorChanger(m_ColliderColorChangerPool, m_ColliderColorChangerList);
			RemoveFromCollisionColorChangerPool(gameobject);
		}
		break;

	case transformComponent:
		if (gameobject->HasComponent(type))
		{
			gameobject->RemoveTransform(m_TransformComponentPool, m_TransformComponentList);
			RemoveFromTransformPool(gameobject);
		}
		break;

	case playerControllerComponent:
		if (gameobject->HasComponent(type))
		{
			//RemoveFromPlayerControllerPool(gameobject);
			gameobject->RemovePlayerController(m_PlayerControllerPool, m_PlayerControllerList);
		}
		break;

	case rectangleColliderComponent:
		if (gameobject->HasComponent(type))
		{
			gameobject->RemoveCollider(m_ColliderPool, m_ColliderList);
			RemoveFromColliderPool(gameobject);
		}
		break;

	case rectangleRendererComponent:
		if (gameobject->HasComponent(type))
		{
			gameobject->RemoveRenderer(m_RendererPool, m_RendererList);
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

void WorldManager::DeleteSelected() 
{
	int key;
	for (auto i = m_World.begin(); i != m_World.end(); i++) {
		if (i->second == m_SelectedGO)
		{
			key = i->first;
			break;
		}
	}
	
	if (m_SelectedGO->GetTransform() != NULL) {
		for (int i = 0; i < m_TransformComponentList.size(); i++) {
			if (m_TransformComponentList[i] == m_SelectedGO->GetTransform())
			{
				m_TransformComponentList.erase(m_TransformComponentList.begin() + i);
			}
		}
		m_TransformComponentPool.Delete(m_SelectedGO->GetTransform());
	}

	if (m_SelectedGO->GetRenderer() != NULL)
	{
		for (int i = 0; i < m_RendererList.size(); i++) {
			if (m_RendererList[i] == m_SelectedGO->GetRenderer())
			{
				m_RendererList.erase(m_RendererList.begin() + i);
			}
		}
		m_RendererPool.Delete(m_SelectedGO->GetRenderer());
	}

	if (m_SelectedGO->GetPlayerController() != NULL)
	{
		for (int i = 0; i < m_PlayerControllerList.size(); i++) {
			if (m_PlayerControllerList[i] == m_SelectedGO->GetPlayerController())
			{
				m_PlayerControllerList.erase(m_PlayerControllerList.begin() + i);
			}
		}
		m_PlayerControllerPool.Delete(m_SelectedGO->GetPlayerController());
	}

	if (m_SelectedGO->GetCollisionColorChanger() != NULL)
	{
		for (int i = 0; i < m_ColliderColorChangerList.size(); i++) {
			if (m_ColliderColorChangerList[i] == m_SelectedGO->GetCollisionColorChanger())
			{
				m_ColliderColorChangerList.erase(m_ColliderColorChangerList.begin() + i);
			}
		}
		m_ColliderColorChangerPool.Delete(m_SelectedGO->GetCollisionColorChanger());
	}

	if (m_SelectedGO->GetCollider() != NULL)
	{
		for (int i = 0; i < m_ColliderList.size(); i++) {
			if (m_ColliderList[i] == m_SelectedGO->GetCollider())
			{
				m_ColliderList.erase(m_ColliderList.begin() + i);
			}
		}
		m_ColliderPool.Delete(m_SelectedGO->GetCollider());
	}

	m_World.erase(key);
}

void WorldManager::ClearWorld()
{
	/*
	to delete an object we need to:
	delete the gameobject itself
	remove from pool and list
	*/

	m_World.clear();

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

	ClearObjectIDs();
	CloseEditUI();

	printf("done clearing\n");
}


// Object ID
int WorldManager::GetNewObjectID()
{
	return m_ObjectIDIndex++;
}

void WorldManager::ClearObjectIDs()
{
	m_ObjectIDIndex = 0;
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
	SaveWorld(saveFileName);
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
	tempGO->GetTransform()->SetGameObjectID(tempGO->GetObjectID());

	int x = GetRandomValue(30, 100);
	int y = GetRandomValue(30, 100);

	tempGO->SetRenderer(AddToRendererPool(x, y, x, y, GetRandomColor()));
	tempGO->GetRenderer()->SetGameObjectID(tempGO->GetObjectID());
}




bool WorldManager::CheckMouseCollision(Rectangle rect)
{
	Vector2 pos = GetMousePosition();
	return CheckCollisionPointRec(pos, rect);
}

void WorldManager::ToggleUI()
{
	m_ShowGameObjectEditor = !m_ShowGameObjectEditor;
}

void WorldManager::OpenEditUI()
{
	if(!m_ShowGameObjectEditor)
		m_ShowGameObjectEditor = true;
}

void WorldManager::CloseEditUI()
{
	if(m_ShowGameObjectEditor)
		m_ShowGameObjectEditor = false;

	m_SelectedGO = NULL;
}

Color WorldManager::GetRandomColor()
{
	int rand = GetRandomValue(0, 5);
	Color color;

	switch (rand)
	{
		case 0:
			color = BLUE;
			break;
		case 1:
			color = RED;
			break;
		case 2:
			color = YELLOW;
			break;
		case 3:
			color = PURPLE;
			break;
		case 4:
			color = BLACK;
			break;
		default:
			color = PINK;
			break;
	}

	return color;
}





