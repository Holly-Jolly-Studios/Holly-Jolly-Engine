#include "BaseComponent.h"

void BaseComponent::SetGameObjectID(int id)
{
	m_gameObjectID = id;
}

int BaseComponent::GetGameObjectID()
{
	return m_gameObjectID;
}
