#ifndef __PARTICLE_EFFECTS_H__
#define __PARTICLE_EFFECTS_H__

//ERROR: Remove this include from here
#include"DEJsonSupport.h"

struct Particle;

enum class PARTICLE_EFFECT_TYPE : int
{
    NONE = -1,
    FORCE_OVER_LIFETIME,
    ROTATE_OVER_LIFETIME,
    COLOR_OVER_LIFETIME,
	SIZE_OVER_LIFETIME,
	VELOCITY_OVER_LIFETIME,
	SHAPE,
	MAX    
};

class ParticleEffect 
{
public:
    ParticleEffect(PARTICLE_EFFECT_TYPE type);
    virtual ~ParticleEffect();

    virtual void Spawn(Particle& particle);//executes once per particle
    virtual void Update(Particle& particle, float dt);//executes once per particle
	virtual void PrepareEffect();//executes once per frame

#ifndef STANDALONE
    virtual void OnEditor(int emitterIndex);
#endif // !STANDALONE

    virtual void SaveData(JSON_Object* nObj);
    virtual void LoadData(DEConfig& nObj);

public:
    PARTICLE_EFFECT_TYPE type;
    bool toDelete;
};

#endif // !__PARTICLE_EFFECTS