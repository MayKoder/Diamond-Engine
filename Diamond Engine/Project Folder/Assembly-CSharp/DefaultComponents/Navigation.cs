using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace DiamondEngine
{
    public class Navigation : DiamondComponent
    {
        public Navigation()
        {
            type = ComponentType.NAVIGATION;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void Select();

    }


}
