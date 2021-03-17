using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace DiamondEngine
{
    public sealed class GameObject
    {
        public string name;
        public UIntPtr pointer; //Searching all the GO's with UID's? Nah boy we cast stuff here
        public Transform transform;

        public GameObject()
        {
            name = "Empty";
            pointer = UIntPtr.Zero;
        }

        public GameObject(string _name, UIntPtr ptr, UIntPtr transPtr)
        {
            name = _name;
            pointer = ptr;

            transform = new Transform();
            transform.pointer = transPtr;
            //Debug.Log(transform.type.ToString());
            //Debug.Log(ptr.ToString());
            //Debug.Log("Created: " + UID.ToString());
        }

        
        public T GetComponent<T>() where T : DiamondComponent
        {
            //ComponentType type = T.get;
            ComponentType retValue = ComponentType.SCRIPT;
            if(DiamondComponent.componentTable.ContainsKey(typeof(T)))
            {
                retValue = DiamondComponent.componentTable[typeof(T)];
            }
            return TryGetComponent<T>(typeof(T).ToString(), (int)retValue);
        }

        public extern string tag
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal T TryGetComponent<T>(string type, int inputType = 0);


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void AddComponent(int componentType);
        public extern string Name
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
        }
        public extern GameObject parent
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void Enable(bool enable);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern bool IsEnabled();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern bool CompareTag(string tag);
    }
}
