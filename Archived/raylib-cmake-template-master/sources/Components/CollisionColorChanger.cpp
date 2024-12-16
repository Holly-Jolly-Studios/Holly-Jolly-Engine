#include "CollisionColorChanger.h"

CollisionColorChanger::CollisionColorChanger()
{
	m_NewColor = Color{ 0, 0, 0, 0 };
	m_DefaultColor = Color{ 0, 0, 0, 0 };
	m_IsDefaultColor = true;
	m_SwitchedColor = false;
}

CollisionColorChanger::CollisionColorChanger(Color changedColor, Color defaultColor)
{
	this->m_NewColor = changedColor;
	this->m_DefaultColor = defaultColor;
	m_IsDefaultColor = true;
	m_SwitchedColor = false;
}

CollisionColorChanger::~CollisionColorChanger()
{
	// Do nothing
}

bool CollisionColorChanger::Update(bool isColliding)
{
	// If you are colliding and you aren't a different color, switch colors.
	// If you arent colliding and you aren't your original color switch back

	if (isColliding)
	{
		if (!m_SwitchedColor)
		{
			m_SwitchedColor = true;
			m_IsDefaultColor = false;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (m_SwitchedColor)
		{
			m_SwitchedColor = false;
			m_IsDefaultColor = true;
			return true;
		}
	}
}

#pragma region Helpers

void CollisionColorChanger::SetChangedColor(Color color)
{
	m_NewColor = color;
}

void CollisionColorChanger::SetRValue(float r)
{
	m_NewColor.r = r;
}

void CollisionColorChanger::SetGValue(float g)
{
	m_NewColor.g = g;
}

void CollisionColorChanger::SetBValue(float b)
{
	m_NewColor.b = b;
}

void CollisionColorChanger::SetAValue(float a)
{
	m_NewColor.a = a;
}

Color CollisionColorChanger::GetNewColor()
{
	return m_NewColor;
}

bool CollisionColorChanger::GetIsDefaultColor()
{
	return m_IsDefaultColor;
}

void CollisionColorChanger::SetDefaultColor(Color color)
{
	m_DefaultColor = color;
}

Color CollisionColorChanger::GetDefaultColor()
{
	return m_DefaultColor;
}

#pragma endregion

