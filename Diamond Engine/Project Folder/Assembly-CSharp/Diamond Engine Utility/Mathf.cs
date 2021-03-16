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
    }
}
