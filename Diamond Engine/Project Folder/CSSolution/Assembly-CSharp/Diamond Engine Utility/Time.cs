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







    }
}
