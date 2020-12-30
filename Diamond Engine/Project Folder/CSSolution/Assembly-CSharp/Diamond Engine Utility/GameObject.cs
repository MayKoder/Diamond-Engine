using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace DiamondEngine
{
    public sealed class GameObject
    {
        public GameObject()
        {
            name = "Empty";
            UID = 0;
        }
        public GameObject(string _name, int _UID)
        {
            name = _name;
            UID = _UID;
            //Debug.Log("Created: " + UID.ToString());
        }

        public string name; //Can't use get; set; because mono can't use mono_get_field then lol
        public int UID;
        public extern Vector3 localPosition
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;

            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }

        public extern Vector3 globalPosition
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
        }

        public extern Quaternion localRotation
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;

            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }

        public extern Quaternion globalRotation
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
        }

        public extern Vector3 localScale
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;

            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }

        public extern Vector3 globalScale
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
        }


        int GetHash()
        {
            return this.GetHashCode();
        }


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Vector3 GetForward();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void AddComponent(int componentType);

    }
}
