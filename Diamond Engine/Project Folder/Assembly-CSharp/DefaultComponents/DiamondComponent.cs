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
        COUNT
    }

    public class DiamondComponent
    {
        public UIntPtr pointer;
        public ComponentType type;
        public static Dictionary<System.Type, ComponentType> componentTable = new Dictionary<Type, ComponentType> {
            { typeof(Transform), ComponentType.TRANSFORM },
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
