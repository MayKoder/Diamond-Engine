using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace DiamondEngine
{
    public sealed class GameObject
    {
        public GameObject()
        {
            globalMatrix = mat4x4.identity;
            name = "Empty";

            lRotation = Quaternion.identity;
            lScale = Vector3.one;
        }
        public GameObject(string _name, int _UID)
        {
            globalMatrix = mat4x4.identity;

            name = _name;

            lRotation = Quaternion.identity;
            lScale = Vector3.one;

            UID = _UID;
        }
        public GameObject(string _name, Quaternion _rotation, Vector3 _scale, int _UID)
        {
            _rotation.Normalize();
            globalMatrix = mat4x4.identity;

            name = _name; 

            lRotation = _rotation;
            lScale = _scale;

            UID = _UID;
        }

        public string name; //Can't use get; set; because mono can't use mono_get_field then lol

        public extern Vector3 position
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;

            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }

        private Quaternion lRotation = Quaternion.identity;
        public Quaternion rotation
        {
            get
            {
                return lRotation;
            }

            set
            {
                //InternalCalls.CSLog(rotForce.ToString());
                //Debug.Log("PreSet: " + name.ToString() + ": " + lRotation.ToString() + " // " + value.ToString() + " and Test = " + Test.ToString());

                this.lRotation = value;
                globalMatrix.SetFromTRS(position, lRotation, lScale); //Something wrong here, doing crasy stuff while mixing the pyramid variables
                InternalCalls.UpdateCppRotation(UID, lRotation);
                //Debug.Log("Exit: " + lScale.ToString());
                //InternalCalls.UpdateCppGO(UID, this.lPosition, this.lRotation/*, this.lScale*/);
                //Debug.Log("Afterset: " + name.ToString() + ": " + lRotation.ToString() + " // " + value.ToString() + " and Test = " + Test.ToString());
            }
        }

        private Vector3 lScale = Vector3.zero;
        public Vector3 scale
        {
            get
            {
                return lScale;
            }

            set
            {
                lScale = value;
                globalMatrix.SetFromTRS(position, lRotation, lScale);
                InternalCalls.UpdateCppScale(UID, lScale);
            }
        }

        int GetHash()
        {
            return this.GetHashCode();
        }

        public mat4x4 globalMatrix;
        public int UID = 0;
    }
}
