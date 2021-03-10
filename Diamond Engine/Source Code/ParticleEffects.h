#pragma once

enum class PARTICLEEFFECTTYPES {
    NONE
};

class ParticleEffect {
public:
    ParticleEffect(PARTICLEEFFECTTYPES type);
    ~ParticleEffect();

#ifndef STANDALONE
    virtual void OnEditor();
#endif // !STANDALONE

    virtual void Update();

public:
    PARTICLEEFFECTTYPES type;
};