using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace DiamondEngine
{
    public class Time
    {
        public static extern float deltaTime
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
        }

        public static extern float totalTime
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void PauseGame();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void ResumeGame();
    }
}
