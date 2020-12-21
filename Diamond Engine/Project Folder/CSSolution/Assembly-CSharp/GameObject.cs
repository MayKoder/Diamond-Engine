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


            lPosition = Vector3.zero;
            position = Vector3.zero;

            lRotation = Quaternion.identity;
            rotation = Quaternion.identity;

            lScale = Vector3.one;
            scale = Vector3.one;
        }
        public GameObject(string _name, int _UID)
        {
            globalMatrix = mat4x4.identity;

            name = _name;

            lPosition = Vector3.zero;
            position = Vector3.zero;

            lRotation = Quaternion.identity;
            rotation = Quaternion.identity;

            lScale = Vector3.one;
            scale = Vector3.one;

            UID = _UID;
        }
        public GameObject(string _name, Vector3 _position, Quaternion _rotation, Vector3 _scale, int _UID)
        {
            _rotation.Normalize();
            globalMatrix = mat4x4.identity;

            name = _name;

            lPosition = _position;
            position = _position;

            lRotation = _rotation;
            rotation = _rotation;

            lScale = _scale;
            scale = _scale;

            UID = _UID;
        }

        public string name; //Can't use get; set; because mono can't use mono_get_field then lol

        private Vector3 lPosition = Vector3.zero;
        public Vector3 position
        {
            get
            {
                return lPosition;
            }

            set
            {
                lPosition = value;
                globalMatrix.SetFromTRS(lPosition, lRotation, lScale);
                InternalCalls.UpdateCppPosition(UID, lPosition);
            }
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
                globalMatrix.SetFromTRS(lPosition, lRotation, lScale); //Something wrong here, doing crasy stuff while mixing the pyramid variables
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
                globalMatrix.SetFromTRS(lPosition, lRotation, lScale);
                InternalCalls.UpdateCppScale(UID, lScale);
            }
        }

        public extern Vector3 testPosition 
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;

            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set; 
        }

        int GetHash()
        {
            return this.GetHashCode();
        }

        public mat4x4 globalMatrix;
        public int UID = 0;
    }
}
