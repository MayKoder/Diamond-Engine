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
            pointer = UIntPtr.Zero;
        }

        public GameObject(string _name, UIntPtr ptr)
        {
            name = _name;
            pointer = ptr;
            //Debug.Log(ptr.ToString());
            //Debug.Log("Created: " + UID.ToString());
        }

        public string name;
        public UIntPtr pointer; //Searching all the GO's with UID's? Nah boy we cast stuff here
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
        public extern Vector3 GetRight();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void AddComponent(int componentType);

    }
}
