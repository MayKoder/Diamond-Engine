using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;

namespace DiamondEngine
{
    public enum ComponentType
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
        public static Dictionary<System.Type, ComponentType> componentTable = new Dictionary<Type, ComponentType> {
            { typeof(Transform), ComponentType.Transform },
        };

        public DiamondComponent()
        {
            this.type = ComponentType.Script;
        }

        public extern GameObject gameObject
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
        }

        public ComponentType GetComponentType()
        {
            return type;
        }
    }
}
