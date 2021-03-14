using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace DiamondEngine
{
    public class Image2D : DiamondComponent
    {
        public Image2D()
        {
            type = ComponentType.IMAGE_2D;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SwapTwoImages(GameObject other_image);

    }


}
