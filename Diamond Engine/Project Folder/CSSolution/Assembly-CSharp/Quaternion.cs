using System;

namespace DiamondEngine
{
    public sealed class Quaternion /*: IEquatable<Vector3>*/
    {
        public float x, y, z, w;
        static Quaternion()
        {
            identity = new Quaternion(0, 0, 0, 1);
            one = new Quaternion(1, 1, 1, 1);
        }
        public Quaternion()
        {
            x = 0.0f;
            y = 0.0f;
            z = 0.0f;
            w = 0.0f;
        }
        public Quaternion(float _x, float _y, float _z, float _w)
        {
            x = _x;
            y = _y;
            z = _z;
            w = _w;
        }

        public static Quaternion identity { get; }
        public static Quaternion one { get; }

        public void Set(float newX, float newY, float newZ, float newW)
        {
            x = newX; y = newY; z = newZ; w = newW;
        }

        //Rotate an angle(radiants) aroun an axis
        public static Quaternion RotateAroundAxis(Vector3 axis, float angle)
        {
            Quaternion ret = Quaternion.one;

            float factor = (float)Math.Sin(angle / 2.0);

            ret.x = axis.x * factor;
            ret.y = axis.y * factor;
            ret.z = axis.z * factor;

            ret.w = (float)Math.Cos(angle / 2.0);

            //InternalCalls.CSLog(ret);
            ret.Normalize();

            return ret;
        }

        public void Normalize()
        {
            float n = (float)Math.Sqrt((x * x) + (y * y) + (z * z) + (w * w));
            x /= n;
            y /= n;
            z /= n;
            w /= n;
        }

        public static Quaternion operator*(Quaternion q1, Quaternion q2)
        {
            Quaternion ret = Quaternion.identity;
            ret.x = (q1.x * q2.w) + (q1.y * q2.z) - (q1.z * q2.y) + (q1.w * q2.x);
            ret.y = (-q1.x * q2.z) + (q1.y * q2.w) + (q1.z * q2.x) + (q1.w * q2.y);
            ret.z = (q1.x * q2.y) - (q1.y * q2.x) + (q1.z * q2.w) + (q1.w * q2.z);
            ret.w = (-q1.x * q2.x) - (q1.y * q2.y) - (q1.z * q2.z) + (q1.w * q2.w);
            ret.Normalize();

            return ret;
        }

        public override string ToString()
        {
            return x.ToString() + ", " + y.ToString() + ", " + z.ToString() + ", " + w.ToString();
        }

    }
}
