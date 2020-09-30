#include"MaykMath.h"
#include<assert.h>

void MaykMath::Init()
{
	srand(time(NULL));
}

int MaykMath::Random(int minV, int maxV)
{
	assert(minV <= maxV);
	return (rand() % (maxV - minV + 1)) + minV;
}

float MaykMath::Random(float minV, float maxV)
{
	assert(minV <= maxV);
	float random = ((float)rand()) / (float)RAND_MAX;
	return minV + (random * (maxV - minV));
}

std::string MaykMath::VersionToString(int major, int minor, int patch)
{
	std::string ret(std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch));
	return ret;
}

//template <class T>
//void MaykMath::FixedVectorPushBack(std::vector<T>& vec, T& value)
//{
//	//if (vec.size() == vec.capacity())
//	//{
//	//	//Vector is full
//	//	//delete vector's oldest element
//	//	//Move all elements 1 position back
//	//	for (unsigned int i = 0; i < vec.size(); i++)
//	//	{
//	//		if (i + 1 < vec.size())
//	//		{
//	//			float iCopy = vec[i + 1];
//	//			vec[i] = iCopy;
//	//		}
//	//	}
//	//	vec[vec.capacity() - 1] = value;
//	//	//Pushback new element
//	//}
//	//else
//	//{
//	//	//Vector is not full
//	//	vec.push_back(value);
//	//}
//}