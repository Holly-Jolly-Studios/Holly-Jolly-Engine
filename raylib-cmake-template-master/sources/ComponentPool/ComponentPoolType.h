#pragma once
#include <vector>
#include "ComponentPool/Component.h"  
#include "GameObject/GameObject.h"      

class NewTransformPool
{
public:
    NewTransformPool(int size);
    ~NewTransformPool();

    void Delete(NewTransform* component);
    NewTransform* New();
    NewTransform* GetComponent(int i);

private:
    std::vector<NewTransform> m_NewTransformComponentPool;
    std::vector<bool> m_IsActive;
};


class RendererPool
{
public:
    RendererPool(int size);
    ~RendererPool();

    void Delete(RectangleRenderer* component);
    RectangleRenderer* New();
    RectangleRenderer* GetComponent(int i);

private:
    std::vector<RectangleRenderer> m_RendererComponentPool;
    std::vector<bool> m_IsActive;
};


class PlayerPool
{
public:
    PlayerPool(int size);
    ~PlayerPool();

    void Delete(PlayerController* component);
    PlayerController* New();
    PlayerController* GetComponent(int i);

private:
    std::vector<PlayerController> PlayerControllerComponentPool;
    std::vector<bool> m_IsActive;
};
