using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace DiamondEngine
{
    public class ParticleSystem : DiamondComponent
    {
        public ParticleSystem()
        {
            type = ComponentType.PARTICLE_SYSTEM;
        }

        public extern bool playing
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
        }

        public extern bool looping
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void Play();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void Stop();

    }


}
