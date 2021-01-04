using System;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace DiamondEngine
{

    [StructLayout(LayoutKind.Sequential)]
    public partial class Quaternion /*: IEquatable<Vector3>*/
    {

        public float x;
        public float y;
        public float z;
        public float w;

        public float this[int index] //Unity does this, is not the fastest way but maybe it's the way to fix the transform bug?
        {
            get
            {
                switch (index)
                {
                    case 0: return x;
                    case 1: return y;
                    case 2: return z;
                    case 3: return w;
                    default:
                        throw new IndexOutOfRangeException("Invalid Quaternion index!");
                }
            }

            set
            {
                switch (index)
                {
                    case 0: x = value; break;
                    case 1: y = value; break;
                    case 2: z = value; break;
                    case 3: w = value; break;
                    default:
                        throw new IndexOutOfRangeException("Invalid Quaternion index!");
                }
            }
        }

        public Quaternion(float x, float y, float z, float w) 
        { 
            this.x = x; this.y = y; this.z = z; this.w = w; 
        }

        static readonly Quaternion identityQuaternion = new Quaternion(0F, 0F, 0F, 1F);
        public static Quaternion identity
        {
            get
            {
                return identityQuaternion;
            }
        }

        public void Set(float newX, float newY, float newZ, float newW)
        {
            x = newX; y = newY; z = newZ; w = newW;
        }

        //Rotate an angle(radiants) aroun an axis
        public static Quaternion RotateAroundAxis(Vector3 axis, float angle)
        {
            Quaternion ret = Quaternion.identity;

            float factor = (float)Math.Sin(angle / 2.0);

            ret.x = axis.x * factor;
            ret.y = axis.y * factor;
            ret.z = axis.z * factor;

            ret.w = (float)Math.Cos(angle / 2.0);

            //InternalCalls.CSLog(ret);
            //ret = ret.normalized;

            return ret;
        }

        public static float Dot(Quaternion a, Quaternion b)
        {
            return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
        }

        public static Quaternion Normalize(Quaternion q)
        {
            float mag = (float)Math.Sqrt(Dot(q, q));

            if (mag < float.Epsilon)
                return Quaternion.identity;

            return new Quaternion(q.x / mag, q.y / mag, q.z / mag, q.w / mag);
        }

        public void Normalize()
        {
            Quaternion norm = Normalize(this);
            x = norm.x;
            y = norm.y;
            z = norm.z;
            w = norm.w;
        }

        public Quaternion normalized
        {
            get 
            {
                return Normalize(this); 
            }
        }

        public static Quaternion operator *(Quaternion q1, Quaternion q2)
        {
            return new Quaternion(
                q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
                q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
                q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x,
                q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z);
        }


        public override string ToString()
        {
            return x.ToString() + ", " + y.ToString() + ", " + z.ToString() + ", " + w.ToString();
        }

    }
}
