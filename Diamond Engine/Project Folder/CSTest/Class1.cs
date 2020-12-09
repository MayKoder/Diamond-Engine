using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Security;

namespace MaykLogic
{
    public sealed class GameObject : Object
    {
        public GameObject()
        {
            name = "Empty";
            position = Vector3.zero;
        }
        public GameObject(string _name)
        {
            name = _name;
            position = Vector3.zero;
        }

        public string name { get; set; }
        public Vector3 position { get; set; }



    }
}
