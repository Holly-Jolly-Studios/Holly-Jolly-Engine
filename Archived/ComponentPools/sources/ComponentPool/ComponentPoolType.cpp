#include "ComponentPool/ComponentPoolType.h"

#pragma region NewTransformPool

NewTransformPool::NewTransformPool(int size)
{
    m_NewTransformComponentPool.resize(size);
    m_IsActive.resize(size, false);
}

NewTransformPool::~NewTransformPool()
{
}

void NewTransformPool::Delete(NewTransform* component)
{
    for (int i = 0; i < m_NewTransformComponentPool.size(); i++)
    {
        NewTransform* comparer = &m_NewTransformComponentPool[i];
        if (component == comparer)
        {
            m_IsActive[i] = false;
            comparer->~NewTransform();
        }
    }
}

NewTransform* NewTransformPool::New()
{
    for (int i = 0; i < m_NewTransformComponentPool.size(); i++)
    {
        if (!m_IsActive[i])
        {
            new (&m_NewTransformComponentPool[i]) NewTransform(10, 10);
            m_IsActive[i] = true;
            return (&m_NewTransformComponentPool[i]);
        }
    }
    return nullptr;
}

NewTransform* NewTransformPool::GetComponent(int i)
{
    return &m_NewTransformComponentPool[i];
}

#pragma endregion


#pragma region RendererPool

RendererPool::RendererPool(int size)
{
    m_RendererComponentPool.resize(size);
    m_IsActive.resize(size, false);
}

RendererPool::~RendererPool()
{}

void RendererPool::Delete(RectangleRenderer* component)
{
    for (int i = 0; i < m_RendererComponentPool.size(); i++)
    {
        RectangleRenderer* comparer = &m_RendererComponentPool[i];
        if (component == comparer)
        {
            m_IsActive[i] = false;
            comparer->~RectangleRenderer();
        }
    }
}

RectangleRenderer* RendererPool::New()
{
    for (int i = 0; i < m_RendererComponentPool.size(); i++)
    {
        if (!m_IsActive[i])
        {
            new (&m_RendererComponentPool[i]) RectangleRenderer(50, 50, BLACK);
            m_IsActive[i] = true;
            return (&m_RendererComponentPool[i]);
        }
    }
    return nullptr;
}

RectangleRenderer* RendererPool::GetComponent(int i)
{
    return &m_RendererComponentPool[i];
}

#pragma endregion


#pragma region PlayerPool

PlayerPool::PlayerPool(int size)
{
    PlayerControllerComponentPool.resize(size);
    m_IsActive.resize(size, false);
}

PlayerPool::~PlayerPool()
{
}

void PlayerPool::Delete(PlayerController* component)
{
    for (int i = 0; i < PlayerControllerComponentPool.size(); i++)
    {
        PlayerController* comparer = &PlayerControllerComponentPool[i];
        if (component == comparer)
        {
            m_IsActive[i] = false;
            comparer->~PlayerController();
        }
    }
}

PlayerController* PlayerPool::New()
{
    for (int i = 0; i < PlayerControllerComponentPool.size(); i++)
    {
        if (!m_IsActive[i])
        {
            new (&PlayerControllerComponentPool[i]) PlayerController();
            m_IsActive[i] = true;
            return (&PlayerControllerComponentPool[i]);
        }
    }
    return nullptr;
}

PlayerController* PlayerPool::GetComponent(int i)
{
    return &PlayerControllerComponentPool[i];
}

#pragma endregion


