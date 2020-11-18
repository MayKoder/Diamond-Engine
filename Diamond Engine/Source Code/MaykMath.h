#pragma once

#ifndef __MAYKMATH_H__
#define __MAYKMATH_H__

#include<stdlib.h>
#include<time.h>
#include<vector>
#include<string>

namespace MaykMath 
{

	void Init();
	int Random(int minV, int maxV);
	float Random(float minV, float maxV);
	std::string VersionToString(int major, int minor, int patch);

	void FindCentroid(float* A, float* B, float* C, float* r);

	void GeneralDataSet(float* dest, float* src, size_t vecSize);

	template <class T>
	void FixedVectorPushBack(std::vector<T>& vec, T& value) {
		if (vec.size() == vec.capacity())
		{
			//Vector is full
			//delete vector's oldest element
			//Move all elements 1 position back
			for (unsigned int i = 0; i < vec.size(); i++)
			{
				if (i + 1 < vec.size())
				{
					float iCopy = vec[i + 1];
					vec[i] = iCopy;
				}
			}
			vec[vec.capacity() - 1] = value;
			//Pushback new element
		}
		else
		{
			//Vector is not full
			vec.push_back(value);
		}
	}


}


#endif //__MAYKMATH_H__