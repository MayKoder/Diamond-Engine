using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace DiamondEngine
{
    enum ComponentType
    {
        None,
		Transform,
		MeshRenderer,
		Material,
		Camera,
		Script,
		Count,
    }

    public class DiamondComponent
    {
        public UIntPtr pointer;
        public ComponentType type;

        public virtual DiamondComponent()
        {
            this.type = ComponentType.Script;
        }

        public extern GameObject gameObject
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
        }
    }
}
