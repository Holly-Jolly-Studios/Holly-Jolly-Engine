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

#pragma region Helpers


void GameObject::SetTransform(NewTransform* NewTransform)
{
	this->m_GOTransform = NewTransform;
}

void GameObject::SetPlayerController(PlayerController* con)
{
	this->m_GOPlayerController = con;
}

void GameObject::SetRenderer(RectangleRenderer* rect)
{
	this->m_GORenderer = rect;
}

void GameObject::SetCollider(RectangleCollider* col)
{
	this->m_GOCollider = col;
}



void GameObject::SetColorChanger(CollisionColorChanger* colChange)
{
	this->m_GOColorChanger = colChange;
}


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



