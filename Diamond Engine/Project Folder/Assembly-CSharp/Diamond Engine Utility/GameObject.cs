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
            //transform = new Transform(); //ERROR TODO: If we request something out of a gameobject created like this, we are fucked
            //InternalCalls.CreateGameObject(name, Vector3.zero);
        }
        public GameObject(string _name, UIntPtr ptr, UIntPtr transPTR)
        {
            name = _name;
            pointer = ptr;

            transform = new Transform();
            transform.pointer = transPTR;
        }


        public T GetComponent<T>() where T : DiamondComponent
        {
            //ComponentType type = T.get;
            ComponentType retValue = ComponentType.Script;
            if (DiamondComponent.componentTable.ContainsKey(typeof(T)))
            {
                retValue = DiamondComponent.componentTable[typeof(T)];
            }
            return TryGetComponent<T>(typeof(T).ToString(), (int)retValue);
        }


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal T TryGetComponent<T>(string type, int inputType = 0);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void AddComponent(int componentType);

    }
}
