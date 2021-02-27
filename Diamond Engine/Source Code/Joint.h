#pragma once

#include <vector>
#include "MathGeoLib/include/Math/float4x4.h"

class Joint {
public:
	Joint();
	Joint(const char* name, int id, float4x4* offset);
	~Joint();

public:
	const char* m_name;
	std::vector<Joint*> children;
	int m_id;
	float4x4* m_offset;
	float4x4* m_transform;
};