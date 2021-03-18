using System;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

using System.Collections;

namespace DiamondEngine
{

    [StructLayout(LayoutKind.Sequential)]
    public partial class Vector3 //We use class because struct needs to be boxed and unboxed but class doesn't
    {
        //public bool Equals(Vector3 other)
        //{
        //    return (x == other.x && y == other.y && z == other.z);
        //}
        public float x;
        public float y;
        public float z;

        public float this[int index]
        {
            get
            {
                switch (index)
                {
                    case 0: return x;
                    case 1: return y;
                    case 2: return z;
                    default:
                        throw new IndexOutOfRangeException("Invalid Vector3 index!");
                }
            }

            set
            {
                switch (index)
                {
                    case 0: x = value; break;
                    case 1: y = value; break;
                    case 2: z = value; break;
                    default:
                        throw new IndexOutOfRangeException("Invalid Vector3 index!");
                }
            }
        }

        //[MethodImpl(MethodImplOptions.AggressiveInlining)]
        public Vector3(float x, float y, float z) { this.x = x; this.y = y; this.z = z; /*Debug.Log("Setting: " + x + ", " + y + ", " + z);*/ }
        //[MethodImpl(MethodImplOptions.AggressiveInlining)]
        public Vector3(float x, float y) { this.x = x; this.y = y; z = 0F; }
        //[MethodImpl(MethodImplOptions.AggressiveInlining)]
        public void Set(float newX, float newY, float newZ) { x = newX; y = newY; z = newZ;}

        public static Vector3 operator *(Vector3 a, float d) { return new Vector3(a.x * d, a.y * d, a.z * d); }
        public static Vector3 operator +(Vector3 a, Vector3 b) { return new Vector3(a.x + b.x, a.y + b.y, a.z + b.z); }
        public static Vector3 operator /(Vector3 a, float d) { return new Vector3(a.x / d, a.y / d, a.z / d); }
        public static Vector3 operator -(Vector3 a, float d) { return new Vector3(a.x - d, a.y - d, a.z - d); }
        public static Vector3 operator -(Vector3 a, Vector3 b) { return new Vector3(a.x - b.x, a.y - b.y, a.z - b.z); }

        static readonly Vector3 zeroVector = new Vector3(0F, 0F, 0F);
        static readonly Vector3 oneVector = new Vector3(1F, 1F, 1F);
        static readonly Vector3 rightVector = new Vector3(1F, 0F, 0F);
        static readonly Vector3 upVector = new Vector3(0F, 1F, 0F);
        static readonly Vector3 forwardVector = new Vector3(0F, 0F, 1F);
        static readonly Vector3 positiveInfinityVector = new Vector3(float.PositiveInfinity, float.PositiveInfinity, float.PositiveInfinity);
        static readonly Vector3 negativeInfinityVector = new Vector3(float.NegativeInfinity, float.NegativeInfinity, float.NegativeInfinity);

        public static Vector3 zero { get { return zeroVector; } }
        public static Vector3 one { get { return oneVector; } }
        public static Vector3 forward { get { return forwardVector; } }
        public static Vector3 up { get { return upVector; } }
        public static Vector3 right { get { return rightVector; } }
        public static Vector3 positiveInfinity { get { return positiveInfinityVector; } }
        public static Vector3 negativeInfinity { get { return negativeInfinityVector; } }

        public static float Dot(Vector3 a, Vector3 b)
        {
            return (a.x * b.x + a.y + b.y + a.z + b.z);
        }

        public static Vector3 Cross(Vector3 v1, Vector3 v2)
        {
            float x, y, z;
            x = v1.y * v2.z - v2.y * v1.z;
            y = (v1.x * v2.z - v2.x * v1.z) * -1;
            z = v1.x * v2.y - v2.x * v1.y;

            Vector3 result = new Vector3(x, y, z);
            return result;
        }

        public static float Magnitude(Vector3 vector) { return (float)Math.Sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z); }
        public float magnitude { get { return (float)Math.Sqrt(x * x + y * y + z * z); } }

        public static Vector3 Normalize(Vector3 value)
        {
            float mag = Magnitude(value);
            if (mag > float.Epsilon)
                return value / mag;
            else
                return zero;
        }

        public static Vector3 Lerp(Vector3 a, Vector3 b, float t)
        {
            return (a+ (b - a)*t);
        }

        public static Vector3 SlerpVector(Vector3 start, Vector3 end, float percent)
        {
            float dot = Vector3.Dot(start, end);
            Mathf.Clamp(dot, -1.0f, 1.0f);
            float theta = (float)Math.Acos(dot) * percent;
            Vector3 RelativeVec = end - start * dot;
            return ((start * (float)Math.Cos(theta)) + (RelativeVec.normalized * (float)Math.Sin(theta)));
        }

        //public void Normalize()
        //{
        //    float mag = Magnitude(this);
        //    if (mag > float.Epsilon)
        //        this = this / mag;
        //    else
        //        this = zero;
        //}

        public Vector3 normalized { get { return Vector3.Normalize(this); } }


        public override string ToString()
        {
            return (this.x.ToString() + ", " + this.y.ToString() + ", " + this.z.ToString());
        }


    }
}
