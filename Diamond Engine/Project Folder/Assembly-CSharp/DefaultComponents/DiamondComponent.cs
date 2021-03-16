using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;

namespace DiamondEngine
{
    public enum ComponentType
    {
        NONE,
        TRANSFORM,
        MESH_RENDERER,
        MATERIAL,
        CAMERA,
        SCRIPT,
        TRANSFORM_2D,
        BUTTON,
        CHECKBOX,
        TEXT_UI,
        CANVAS,
        IMAGE_2D,
        AUDIO_LISTENER,
        AUDIO_SOURCE,
        RIGIDBODY,
        COLLIDER,
        ANIMATOR,
        NAVIGATION,
        COUNT
    }

    public class DiamondComponent
    {
        public UIntPtr pointer;
        public ComponentType type;
        public static Dictionary<System.Type, ComponentType> componentTable = new Dictionary<Type, ComponentType> {
            { typeof(Transform), ComponentType.TRANSFORM},
            { typeof(Text), ComponentType.TEXT_UI  },
            { typeof(Material), ComponentType.MATERIAL  },
            { typeof(Image2D), ComponentType.IMAGE_2D  },
            { typeof(Navigation), ComponentType.NAVIGATION  },
        };

        public DiamondComponent()
        {
            this.type = ComponentType.SCRIPT;
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
