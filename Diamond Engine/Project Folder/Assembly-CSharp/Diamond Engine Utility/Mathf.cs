using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace DiamondEngine
{
    public static class Mathf
    {
        public static float Distance(Vector3 pointA, Vector3 pointB)
        {
            Vector3 distance = pointB - pointA;
            return distance.magnitude;
        }

        public static float LerpAngle(float from, float to, float t)
        {
            float delta = Repeat((to - from), 360);
            if (delta > 180)
                delta -= 360;
            return from + delta * Clamp01(t);
        }

        public static float Clamp01(float value)
        {
            if (value < 0f)
                return 0f;
            else if (value > 1f)
                return 1f;
            else
                return value;
        }

        public static float Clamp(float value, float min, float max)
        {
            if (value < min)
                value = min;
            else if (value > max)
                value = max;
            return value;
        }

        public static float Repeat(float t, float length)
        {
            return Clamp(t - Mathf.Floor(t / length) * length, 0.0f, length);
        }
        public static float Floor(float f) { return (float)Math.Floor(f); }

        public const float Rad2Deg = 57.29578f;
    }
}
