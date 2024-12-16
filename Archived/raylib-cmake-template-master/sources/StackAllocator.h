#pragma once

#include <iostream>
#include <vector>

class StackAllocator
{
public:

    StackAllocator(size_t size = 4096) :
        m_Stack(size, 0),
        m_StackSize(m_Stack.size()),
        m_Head(m_Stack.data())
    {}

    template <typename T, typename ...Us>
    T* Alloc(Us... args)
    {
        const size_t size = sizeof(T);
        const size_t bytesUsed = m_Head - m_Stack.data();
        const size_t bytesRemaining = m_StackSize - bytesUsed;
        if (bytesRemaining < size)
        {
            std::cerr << "Stack failed to allocate!\n";
            return nullptr;
        }

        char* ptrToObject = m_Head;
        new (ptrToObject) T(args...);
        m_Head += size;
        return (T*)ptrToObject;
    }

    template <typename T>
    T* AllocArray(size_t arrayCount = 4096)
    {
        const size_t size = sizeof(T) * arrayCount;
        const size_t bytesUsed = m_Head - m_Stack.data();
        const size_t bytesRemaining = m_StackSize - bytesUsed;
        if (bytesRemaining < size)
        {
            std::cerr << "Stack failed to allocate!\n";
            return nullptr;
        }

        char* ptrToObject = m_Head;
        m_Head += size;
        return (T*)ptrToObject;
    }

    void Reset();

    char* GetCurrentHead();

    void ClearToFrame(char* frame);

    size_t Capacity() { return m_Stack.size(); }

    size_t BytesInUse()
    {
        return m_Head - m_Stack.data();
    }

    float PctUsed();

private:
    std::vector<char> m_Stack;
    size_t m_StackSize;
    char* m_Head;
};
