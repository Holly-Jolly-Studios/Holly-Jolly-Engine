#include "GameObject.h"

GameObject::GameObject()
{
	m_GORenderer = NULL;
	m_GOCollider = NULL;
	m_GOPlayerController = NULL;
	m_GOColorChanger = NULL;
	m_GOTransform = NULL;
}

GameObject::~GameObject()
{
	if (m_GORenderer != nullptr)
	{
		delete m_GORenderer;
		m_GORenderer = nullptr;
	}

	if (m_GOCollider != nullptr)
	{
		delete m_GOCollider;
		m_GOCollider = nullptr;
	}

	if (m_GOPlayerController != nullptr)
	{
		delete m_GOPlayerController;
		m_GOPlayerController = nullptr;
	}

	if (m_GOColorChanger != nullptr)
	{
		delete m_GOColorChanger;
		m_GOColorChanger = nullptr;
	}
}

void GameObject::Update()
{
	// Component specific logic
	if (m_GOColorChanger != nullptr && m_GORenderer != nullptr)
	{
		bool colorChanged = m_GOColorChanger->Update(m_GOCollider->GetIsColliding());

		if (colorChanged)
		{
			if (m_GOColorChanger->GetIsDefaultColor())
			{
				m_GORenderer->SetColor(m_GOColorChanger->GetDefaultColor());
			}
			else
			{
				m_GORenderer->SetColor(m_GOColorChanger->GetNewColor());
			}
		}
	}

	// If this GO is a player, do this
	if (m_GOPlayerController != nullptr && m_GOTransform != nullptr)
	{
		m_GOPlayerController->Update(m_GOTransform);
	}
}

#pragma region Object ID

int GameObject::GetObjectID()
{
	return m_ObjectID;
}

void GameObject::SetObjectID(int NewObjectID)
{
	this->m_ObjectID = NewObjectID;

	if (m_GORenderer != NULL)
	{
		m_GORenderer->SetGameObjectID(NewObjectID);
	}

	if (m_GOCollider != NULL)
	{
		m_GOCollider->SetGameObjectID(NewObjectID);
	}

	if (m_GOPlayerController != NULL)
	{
		m_GOPlayerController->SetGameObjectID(NewObjectID);
	}

	if (m_GOColorChanger != NULL)
	{
		m_GOColorChanger->SetGameObjectID(NewObjectID);
	}

	if (m_GOTransform != NULL)
	{
		m_GOTransform->SetGameObjectID(NewObjectID);
	}
}

#pragma endregion

#pragma region Add Components

bool GameObject::ContainsComponentInList(ComponentTypes type)
{
	if (std::find(m_ComponentTypes.begin(), m_ComponentTypes.end(), type) != m_ComponentTypes.end()) 
	{
		return true;
	}
	else 
	{
		return false;
	}
}

void GameObject::SetTransform(NewTransform* NewTransform)
{
	this->m_GOTransform = NewTransform;

	if (!ContainsComponentInList(transformComponent))
	{
		m_ComponentTypes.push_back(transformComponent);
	}
}

void GameObject::SetPlayerController(PlayerController* NewController)
{
	this->m_GOPlayerController = NewController;

	if (!ContainsComponentInList(playerControllerComponent))
	{
		m_ComponentTypes.push_back(playerControllerComponent);
	}
}

void GameObject::SetCollider(RectangleCollider* NewCollider)
{
	this->m_GOCollider = NewCollider;

	if (!ContainsComponentInList(rectangleColliderComponent))
	{
		m_ComponentTypes.push_back(rectangleColliderComponent);
	}
}

void GameObject::SetRenderer(RectangleRenderer* NewRenderer)
{
	this->m_GORenderer = NewRenderer;

	if (!ContainsComponentInList(rectangleRendererComponent))
	{
		m_ComponentTypes.push_back(rectangleRendererComponent);
	}
}

void GameObject::SetColorChanger(CollisionColorChanger* NewColor)
{
	this->m_GOColorChanger = NewColor;

	if (!ContainsComponentInList(collisionColorChangerComponent))
	{
		m_ComponentTypes.push_back(collisionColorChangerComponent);
	}
}

#pragma endregion

#pragma region Remove Components

void GameObject::RemoveFromComponentList(ComponentTypes type)
{
	// may cause errors, try to loop through instead?
	if (ContainsComponentInList(type))
		m_ComponentTypes.erase(std::remove(m_ComponentTypes.begin(), m_ComponentTypes.end(), type), m_ComponentTypes.end());
}

void GameObject::RemoveTransform()
{
	if (m_GOTransform != NULL)
	{
		m_GOTransform = NULL;
		RemoveFromComponentList(transformComponent);
	}
}

void GameObject::RemovePlayerController()
{
	if (m_GOPlayerController != NULL)
	{
		m_GOPlayerController = NULL;
		RemoveFromComponentList(playerControllerComponent);
	}
}

void GameObject::RemoveCollider()
{
	if (m_GOCollider != NULL)
	{
		m_GOCollider = NULL;
		RemoveFromComponentList(rectangleColliderComponent);
	}
}

void GameObject::RemoveRenderer()
{
	if (m_GORenderer != NULL)
	{
		m_GORenderer = NULL;
		RemoveFromComponentList(rectangleRendererComponent);
	}
}

void GameObject::RemoveColorChanger()
{
	if (m_GOColorChanger != NULL)
	{
		m_GOColorChanger = NULL;
		RemoveFromComponentList(collisionColorChangerComponent);
	}
}

void GameObject::RemoveAllComponents()
{
	RemoveTransform();
	RemovePlayerController();
	RemoveCollider();
	RemoveRenderer();
	RemoveColorChanger();
}


#pragma endregion

#pragma region Getters

NewTransform* GameObject::GetTransform()
{
	if (m_GOTransform == nullptr)
	{
		return NULL;
	}
	return m_GOTransform;
}

PlayerController* GameObject::GetPlayerController()
{
	if (m_GOPlayerController == nullptr)
	{
		return NULL;
	}

	return m_GOPlayerController;
}

RectangleRenderer* GameObject::GetRenderer()
{
	if (m_GORenderer == nullptr)
	{
		return NULL;
	}

	return m_GORenderer;
}

RectangleCollider* GameObject::GetCollider()
{
	if (m_GOCollider == nullptr)
	{
		return NULL;
	}

	return m_GOCollider;
}



CollisionColorChanger* GameObject::GetCollisionColorChanger()
{
	if (m_GOColorChanger == nullptr)
	{
		return NULL;
	}

	return m_GOColorChanger;
}

#pragma endregion

void GameObject::SetName(std::string name)
{
	m_Name = name;
}
