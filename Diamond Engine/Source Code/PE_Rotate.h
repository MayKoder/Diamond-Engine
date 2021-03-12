
#include "ParticleEffects.h"
#include "MathGeoLib/include/Math/Quat.h"

class PE_Rotate:ParticleEffect
{
public:
	PE_Rotate();
	~PE_Rotate() override;

	void Spawn(Particle& particle) override;
	void Update(Particle& particle, float dt) override;

#ifndef STANDALONE
	void OnEditor(int emitterIndex) override;
#endif //!STANDALONE

private:
	Quat rotationQuat;
	float rotationSpeed;
};