#pragma once

#include "vector"
#include <iostream>

template <typename T>
class ComponentPool
{
public:

    ComponentPool(int size)
    {
        m_ComponentPool.resize(size);
        m_IsActive.resize(size, false);
    }

    void Delete(T* component)
    {
        for (int i = 0; i < m_ComponentPool.size(); i++)
        {
            T* comparer = &m_ComponentPool[i];
            if (component == comparer)
            {
                m_IsActive[i] = false;
                comparer->~T();
            }
        }
    }

    template <typename... Us>
    T* New(Us... argument) {
        for (int i = 0; i < m_ComponentPool.size(); i++)
        {
            if (!m_IsActive[i])
            {
                new (&m_ComponentPool[i]) T(argument...);
                m_IsActive[i] = true;
                return (&m_ComponentPool[i]);
            }
        }
    }

    T* getComponent(int i)
    {
        return &m_ComponentPool[i];
    }

    void Update(float dt)
    {
        for (int i = 0; i < m_ComponentPool.size(); i++)
        {
            if (m_IsActive[i])
            {
                m_ComponentPool[i].Update(dt);
            }
        }
    }

    void Update(float x, float y, float width, float height)
    {
        for (int i = 0; i < m_ComponentPool.size(); i++)
        {
            if (m_IsActive[i])
            {
                m_ComponentPool[i].Update(x, y, width, height);
            }
        }
    }

    void Render()
    {
        for (int i = 0; i < m_ComponentPool.size(); i++)
        {
            if (m_IsActive[i])
            {
                m_ComponentPool[i].Render();
            }
        }
    }

private:
    std::vector<T> m_ComponentPool;
    std::vector<bool> m_IsActive;
};
