using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace DiamondEngine 
{
    public class Audio 
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void PlayAudio(object go, object eventAudio);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void StopAudio(object go);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void PauseAudio(object go);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void ResumeAudio(object go);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern float GetVolume(object go);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetVolume(object go, float volume);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern float GetPitch(object go);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetPitch(object go, float pitch);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern bool GetMuted(object go);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetMuted(object go, bool muted);
    }
}