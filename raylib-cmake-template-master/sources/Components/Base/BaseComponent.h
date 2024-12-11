#pragma once

class BaseComponent
{
public:
	int GetGameObjectID();

	void SetGameObjectID(int id);

protected:
	int m_gameObjectID;
};