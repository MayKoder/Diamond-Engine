using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace DiamondEngine
{
    public class Text : DiamondComponent
    {
        public Text()
        {
            type = ComponentType.TEXT_UI;
        }

        public extern String text
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;

            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }

    }


}
