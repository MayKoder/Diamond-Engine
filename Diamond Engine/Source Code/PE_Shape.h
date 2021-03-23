#pragma once

#include "ParticleEffects.h";
class PE_SpawnShapeBase;
enum class PE_SPAWN_SHAPE_TYPE;

class PE_Shape : public ParticleEffect //DEPRECATED CLASS TODO DELETE
{
public:
	PE_Shape();
	~PE_Shape() override;

	void Spawn(Particle& particle) override; 

#ifndef STANDALONE
	void OnEditor(int emitterIndex) override;
#endif // !STANDALONE

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;

	PE_SpawnShapeBase* ChangeSpawnShape(PE_SPAWN_SHAPE_TYPE newType);

	//DO NOT USE THIS ONE, USE ChangeSpawnShape() instead
	PE_SpawnShapeBase* CreateSpawnShape(PE_SPAWN_SHAPE_TYPE newType);
	void GetShapeTypeString(PE_SPAWN_SHAPE_TYPE newType,std::string& out);

private:
	bool hasInitialSpeed;
	//Min - Max Start particles speed 
	float particlesSpeed; //speed in the spawn direction
	float randomSpeedMultiplier;
	PE_SpawnShapeBase* mySpawnShape;
};