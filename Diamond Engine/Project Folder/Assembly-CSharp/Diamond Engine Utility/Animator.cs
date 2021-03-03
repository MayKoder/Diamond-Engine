using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace DiamondEngine
{
    class Animator
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void Play(object gameObject, string animationName);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void Pause(object gameObject);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void Resume(object gameObject);
    }
}
