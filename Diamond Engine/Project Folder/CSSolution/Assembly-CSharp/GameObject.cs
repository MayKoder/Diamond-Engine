using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace DiamondEngine
{
    public sealed class GameObject : Object
    {
        public GameObject()
        {
            name = "Empty";
            position = Vector3.zero;
            rotation = Quaternion.zero;
            scale = Vector3.one;
        }
        public GameObject(string _name)
        {
            name = _name;
            position = Vector3.zero;
            rotation = Quaternion.zero;
            scale = Vector3.one;
        }
        public GameObject(string _name, Vector3 _position/*, Quaternion _rotation, Vector3 _scale*/)
        {
            name = _name;
            position = _position;
            //rotation = _rotation;
            //scale = _scale;
        }

        public string name; //Can't use get; set; because mono can't use mono_get_field then lol
        public Vector3 position;
        public Quaternion rotation;
        public Vector3 scale;

    }
}
