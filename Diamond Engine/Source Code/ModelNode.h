#pragma once



struct ModelNode
{

	const char* name;

	float* transform;

	unsigned int meshID;
	unsigned int textureID;

	unsigned int parentTreeID;
	unsigned int treeID;
};