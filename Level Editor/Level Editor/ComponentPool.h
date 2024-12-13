#pragma once

#include <vector>
#include <cassert>

template <typename T>
class ComponentPool
{
public:

	ComponentPool(int size) : m_Pool(size), m_InUse(size, false)
	{}

	ComponentPool() = default;

	template <typename ...Us>
	T* New(Us ...args)
	{
		int i;
		for (i = 0; i < m_Pool.size(); i++)
		{

			if (!m_InUse[i])
			{
				m_InUse[i] = true;

				new (&m_Pool[i]) T(args...);
				return &m_Pool[i];
			}
		}

		assert(false); //should only reach this section if the object pool is empty
	}

	void SetSize(int size)
	{
		m_Pool.resize(size);
		m_InUse.resize(size);
	}

	int GetSize()
	{
		return m_Pool.size;
	}

	void Delete(T* component)
	{
		int i;
		for (i = 0; i < m_Pool.size(); i++) //loops through the entire pool until it find the gameobject to delete
		{
			T* currentCom = &m_Pool[i];
			if (currentCom = component)
			{
				assert(m_InUse[i]); //checks to see if the component exists
				m_InUse[i] = false;

				currentCom->~T();
				return;

			}
		}

		assert(false); //should only reach this section if a component was not inside of the pool
	}

	void ClearPool()
	{
		for (int i = 0; i < m_Pool.size(); i++) //loops through the entire pool until it find the gameobject to delete
		{
			T* currentCom = &m_Pool[i];

			m_InUse[i] = false;

			currentCom->~T();
		}

		return;
	}
private:
	std::vector<T> m_Pool;
	std::vector<bool> m_InUse;
};