using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace DiamondEngine
{
    class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern KeyState GetKey(object keyPressed);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern int GetMouseX();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern int GetMouseY();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void CSLog(object logText);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void CreateGameObject(object name, Vector3 position);
    }

    public sealed class Vector3 /*: IEquatable<Vector3>*/
    {
        //public bool Equals(Vector3 other)
        //{
        //    return (x == other.x && y == other.y && z == other.z);
        //}

        static Vector3()
        {
            zero = new Vector3(0, 0, 0);
            one = new Vector3(1, 1, 1);

            right = new Vector3(1, 0, 0);
            up = new Vector3(0, 1, 0);
            forward = new Vector3(0,0,1);
        }
        public Vector3()
        {
            x = 0.0f;
            y = 0.0f;
            z = 0.0f;
        }
        public Vector3(float _x, float _y, float _z)
        {
            x = _x;
            y = _y;
            z = _z;
        }

        public static Vector3 zero { get; }
        public static Vector3 one { get; }
        public static Vector3 up { get; }
        public static Vector3 forward { get; }
        public static Vector3 right { get; }

        public void Set(float newX, float newY, float newZ)
        {
            x = newX; y = newY; z = newZ;
        }

        public float x, y, z;
    }

    public sealed class mat4x4 /*: IEquatable<Vector3>*/
    {
        static mat4x4()
        {
            //zero = new mat4x4(0, 0, 0, 1);
            identity = new mat4x4();
            identity.SetIdentity();

        }
        public mat4x4()
        {
            for (int i = 0; i < 16; i++)
            {
                v[i] = 0.0f;
            }
        }
        public mat4x4(Vector3 pos, Quaternion rot, Vector3 size)
        {
            SetFromTRS(pos, rot, size);
        }

        public void SetFromTRS(Vector3 pos, Quaternion rot, Vector3 size)
        {
            mat4x4 posMat = SetTranslatePart(pos);
            mat4x4 rotMat = SetRotatePart(rot);
            mat4x4 scaleMat = SetScalePart(size);

            //InternalCalls.CSLog(posMat.ToString());
            //InternalCalls.CSLog(rotMat.ToString());
            //InternalCalls.CSLog(scaleMat.ToString());

            InternalCalls.CSLog("Rot * scale: " + (mat4x4.Mult(ref rotMat, ref scaleMat)).ToString());

            mat4x4 posRotMat = Mult(ref posMat, ref rotMat);

            mat4x4 res = Mult(ref posRotMat, ref scaleMat);
            v = res.v;
        }

        public static mat4x4 SetTranslatePart(Vector3 pos)
        {
            mat4x4 mat = identity;

            mat.v[0] = 1; mat.v[1] = 0; mat.v[2] = 0; mat.v[3] = pos.x;
            mat.v[4] = 0; mat.v[5] = 1; mat.v[6] = 0; mat.v[7] = pos.y;
            mat.v[8] = 0; mat.v[9] = 0; mat.v[10] = 1; mat.v[11] = pos.z;
            mat.v[12] = 0; mat.v[13] = 0; mat.v[14] = 0; mat.v[15] = 1;

            InternalCalls.CSLog("Position: " + mat.ToString());

            return mat;
        }
        public static mat4x4 SetRotatePart(Quaternion rot)
        {
            mat4x4 mat = identity;

            float x = rot.x;
            float y = rot.y;
            float z = rot.z;
            float w = rot.w;

            mat.v[0] = 1 - 2 * (y * y + z * z); mat.v[1] = 2 * (x * y - z * w); mat.v[2] = 2 * (x * z + y * w); mat.v[3] = 0;
            mat.v[4] = 2 * (x * y + z * w); mat.v[5] = 1 - 2 * (x * x + z * z); mat.v[6] = 2 * (y * z - x * w); mat.v[7] = 0;
            mat.v[8] = 2 * (x * z - y * w); mat.v[9] = 2 * (y * z + x * w); mat.v[10] = 1 - 2 * (x * x + y * y); mat.v[11] = 0;
            mat.v[12] = 0; mat.v[13] = 0; mat.v[14] = 0; mat.v[15] = 1;

            InternalCalls.CSLog("Rotation: " +mat.ToString());

            return mat;
        }
        public static mat4x4 SetScalePart(Vector3 scale)
        {
            mat4x4 mat = identity;

            mat.v[0] = scale.x; mat.v[1] = 0; mat.v[2] = 0; mat.v[3] = 0;
            mat.v[4] = 0; mat.v[5] = scale.y; mat.v[6] = 0; mat.v[7] = 0;
            mat.v[8] = 0; mat.v[9] = 0; mat.v[10] = scale.z; mat.v[11] = 0;
            mat.v[12] = 0; mat.v[13] = 0; mat.v[14] = 0; mat.v[15] = 1;

            InternalCalls.CSLog("Scale: "+mat.ToString());


            return mat;
        }
        void SetIdentity()
        {
            int iteration = 0;
            for (int i = 0; i < 16; i += 4)
            {
                v[i] = (iteration == 0) ? 1.0f : 0.0f;
                v[i + 1] = (iteration == 1) ? 1.0f : 0.0f;
                v[i + 2] = (iteration == 2) ? 1.0f : 0.0f;
                v[i + 3] = (iteration == 3) ? 1.0f : 0.0f;
                iteration++;
            }
        }

        public static mat4x4 zero { get; }
        public static mat4x4 identity { get; }

        public override string ToString()
        {
            string ret = "";
            for (int i = 0; i < 16; i += 4)
            {
                ret += (v[i].ToString() + ", " + v[i + 1].ToString() + ", " + v[i + 2].ToString() + ", " + v[i+3].ToString()) + Environment.NewLine;
            }

            return ret;
        }

        //TODO: A && B are identity all the time, this why it aint working, find a way to fix
        public static mat4x4 Mult(ref mat4x4 a, ref mat4x4 b)
        {

            InternalCalls.CSLog("A: " + a.ToString());
            InternalCalls.CSLog("B: " + b.ToString());

            mat4x4 C = identity;
            int n = 4;
            int m = 4;
            int p = 4;

            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < p; j++)
                {
                    float sum = 0;

                    for (int k = 0; k < m; k++)
                    {
                        sum += + (a.At(i, k) * b.At(k, j));
                    }
                    C.Set(i, j, sum);
                }
            }
            return C;
        }

        public float At(int x, int y)
        {
            return v[(y * 4) + x];
        }
        public void Set(int x, int y, float value)
        {
            v[(y * 4) + x] = value;
        }

        public float[] v = new float[16];
    }
}

public enum DEKeyCode //This is a mirror from the SDL scancode enum to allow C# to C++ compatibility
{
    SDL_SCANCODE_UNKNOWN = 0,
    A = 4,
    B = 5,
    C = 6,
    D = 7,
    E = 8,
    F = 9,
    G = 10,
    H = 11,
    I = 12,
    J = 13,
    K = 14,
    L = 15,
    M = 16,
    N = 17,
    O = 18,
    P = 19,
    Q = 20,
    R = 21,
    S = 22,
    T = 23,
    U = 24,
    V = 25,
    W = 26,
    X = 27,
    Y = 28,
    Z = 29,
    Alpha1 = 30,
    Alpha2 = 31,
    Alpha3 = 32,
    Alpha4 = 33,
    Alpha5 = 34,
    Alpha6 = 35,
    Alpha7 = 36,
    Alpha8 = 37,
    Alpha9 = 38,
    Alpha0 = 39,
    RETURN = 40,
    ESCAPE = 41,
    BACKSPACE = 42,
    TAB = 43,
    SPACE = 44,
    MINUS = 45,
    EQUALS = 46,
    LEFTBRACKET = 47,
    RIGHTBRACKET = 48,
    BACKSLASH = 49,
    NONUSHASH = 50,
    SEMICOLON = 51,
    APOSTROPHE = 52,
    GRAVE = 53,
    COMMA = 54,
    PERIOD = 55,
    SLASH = 56,
    CAPSLOCK = 57,
    F1 = 58,
    F2 = 59,
    F3 = 60,
    F4 = 61,
    F5 = 62,
    F6 = 63,
    F7 = 64,
    F8 = 65,
    F9 = 66,
    F10 = 67,
    F11 = 68,
    F12 = 69,
    PRINTSCREEN = 70,
    SCROLLLOCK = 71,
    PAUSE = 72,
    INSERT = 73,
    HOME = 74,
    PAGEUP = 75,
    DELETE = 76,
    END = 77,
    PAGEDOWN = 78,
    RIGHT = 79,
    LEFT = 80,
    DOWN = 81,
    UP = 82,
    NUMLOCKCLEAR = 83,
    KP_DIVIDE = 84,
    KP_MULTIPLY = 85,
    KP_MINUS = 86,
    KP_PLUS = 87,
    KP_ENTER = 88,
    KP_1 = 89,
    KP_2 = 90,
    KP_3 = 91,
    KP_4 = 92,
    KP_5 = 93,
    KP_6 = 94,
    KP_7 = 95,
    KP_8 = 96,
    KP_9 = 97,
    KP_0 = 98,
    KP_PERIOD = 99,
    NONUSBACKSLASH = 100,
    APPLICATION = 101,
    POWER = 102,
    KP_EQUALS = 103,
    F13 = 104,
    F14 = 105,
    F15 = 106,
    F16 = 107,
    F17 = 108,
    F18 = 109,
    F19 = 110,
    F20 = 111,
    F21 = 112,
    F22 = 113,
    F23 = 114,
    F24 = 115,
    EXECUTE = 116,
    HELP = 117,
    MENU = 118,
    SELECT = 119,
    STOP = 120,
    AGAIN = 121,
    UNDO = 122,
    CUT = 123,
    COPY = 124,
    PASTE = 125,
    FIND = 126,
    MUTE = 127,
    VOLUMEUP = 128,
    VOLUMEDOWN = 129,
    KP_COMMA = 133,
    KP_EQUALSAS400 = 134,
    INTERNATIONAL1 = 135,
    INTERNATIONAL2 = 136,
    INTERNATIONAL3 = 137,
    INTERNATIONAL4 = 138,
    INTERNATIONAL5 = 139,
    INTERNATIONAL6 = 140,
    INTERNATIONAL7 = 141,
    INTERNATIONAL8 = 142,
    INTERNATIONAL9 = 143,
    LANG1 = 144,
    LANG2 = 145,
    LANG3 = 146,
    LANG4 = 147,
    LANG5 = 148,
    LANG6 = 149,
    LANG7 = 150,
    LANG8 = 151,
    LANG9 = 152,
    ALTERASE = 153,
    SYSREQ = 154,
    CANCEL = 155,
    CLEAR = 156,
    PRIOR = 157,
    RETURN2 = 158,
    SEPARATOR = 159,
    OUT = 160,
    OPER = 161,
    CLEARAGAIN = 162,
    CRSEL = 163,
    EXSEL = 164,
    KP_00 = 176,
    KP_000 = 177,
    THOUSANDSSEPARATOR = 178,
    DECIMALSEPARATOR = 179,
    CURRENCYUNIT = 180,
    CURRENCYSUBUNIT = 181,
    KP_LEFTPAREN = 182,
    KP_RIGHTPAREN = 183,
    KP_LEFTBRACE = 184,
    KP_RIGHTBRACE = 185,
    KP_TAB = 186,
    KP_BACKSPACE = 187,
    KP_A = 188,
    KP_B = 189,
    KP_C = 190,
    KP_D = 191,
    KP_E = 192,
    KP_F = 193,
    KP_XOR = 194,
    KP_POWER = 195,
    KP_PERCENT = 196,
    KP_LESS = 197,
    KP_GREATER = 198,
    KP_AMPERSAND = 199,
    KP_DBLAMPERSAND = 200,
    KP_VERTICALBAR = 201,
    KP_DBLVERTICALBAR = 202,
    KP_COLON = 203,
    KP_HASH = 204,
    KP_SPACE = 205,
    KP_AT = 206,
    KP_EXCLAM = 207,
    KP_MEMSTORE = 208,
    KP_MEMRECALL = 209,
    KP_MEMCLEAR = 210,
    KP_MEMADD = 211,
    KP_MEMSUBTRACT = 212,
    KP_MEMMULTIPLY = 213,
    KP_MEMDIVIDE = 214,
    KP_PLUSMINUS = 215,
    KP_CLEAR = 216,
    KP_CLEARENTRY = 217,
    KP_BINARY = 218,
    KP_OCTAL = 219,
    KP_DECIMAL = 220,
    KP_HEXADECIMAL = 221,
    LCTRL = 224,
    LSHIFT = 225,
    LALT = 226,
    LGUI = 227,
    RCTRL = 228,
    RSHIFT = 229,
    RALT = 230,
    RGUI = 231,
    MODE = 257,
    AUDIONEXT = 258,
    AUDIOPREV = 259,
    AUDIOSTOP = 260,
    AUDIOPLAY = 261,
    AUDIOMUTE = 262,
    MEDIASELECT = 263,
    WWW = 264,
    MAIL = 265,
    CALCULATOR = 266,
    COMPUTER = 267,
    AC_SEARCH = 268,
    AC_HOME = 269,
    AC_BACK = 270,
    AC_FORWARD = 271,
    AC_STOP = 272,
    AC_REFRESH = 273,
    AC_BOOKMARKS = 274,
    BRIGHTNESSDOWN = 275,
    BRIGHTNESSUP = 276,
    DISPLAYSWITCH = 277,
    KBDILLUMTOGGLE = 278,
    KBDILLUMDOWN = 279,
    KBDILLUMUP = 280,
    EJECT = 281,
    SLEEP = 282,
    APP1 = 283,
    APP2 = 284,
    AUDIOREWIND = 285,
    AUDIOFASTFORWARD = 286,
    SDL_NUM_SCANCODES = 512
}
public enum KeyState
{
    KEY_IDLE = 0,
    KEY_DOWN,
    KEY_REPEAT,
    KEY_UP
};
