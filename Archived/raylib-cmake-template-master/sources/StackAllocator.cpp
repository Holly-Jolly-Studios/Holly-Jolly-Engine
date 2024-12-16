#include "StackAllocator.h"

void StackAllocator::Reset()
{
	m_Head = m_Stack.data();
}

char* StackAllocator::GetCurrentHead()
{
	return m_Head;
}

void StackAllocator::ClearToFrame(char* frame)
{

	if (frame < m_Stack.data() || frame >(m_Stack.data() + m_Stack.size()))
	{
		std::cerr << "Tried to clear to frame pointer " << frame << ", but it's not"
			<< " within stack " << m_Stack.data() << "!\n";
		return;
	}

	m_Head = frame;
}

float StackAllocator::PctUsed()
{
	return 100.f * ((float)BytesInUse() / m_Stack.size());
}

