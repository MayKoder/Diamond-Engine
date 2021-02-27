#include "Joint.h"

Joint::Joint() : m_name("No name joint"), m_id(0), m_offset(nullptr), m_transform(nullptr)
{
}

Joint::Joint(const char* name, int id, float4x4* offset) : m_name(name), m_id(id), m_offset(offset), m_transform(nullptr)
{
}

Joint::~Joint()
{
	for (size_t i = 0; i < children.size(); i++)
	{
		children[i] = nullptr;
	}

	if (m_offset != nullptr)
	{
		delete m_offset;
		m_offset = nullptr;
	}

	if (m_transform != nullptr)
	{
		delete m_transform;
		m_transform = nullptr;
	}
}
