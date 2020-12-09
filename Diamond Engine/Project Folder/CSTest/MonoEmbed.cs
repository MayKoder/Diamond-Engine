using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace MaykLogic
{
    class MonoEmbed
	{
		public GameObject reference;
		int Update(int x)
		{
			x += 10;

			if (InternalCalls.GetKey(DEKeyCode.W) == true) // 26 == W{
			{
				InternalCalls.CreateGameObject("Test Game Object", Vector3.one);
				InternalCalls.CSLog(reference.name);
			}

			return 0;
		}

		static void Main()
        {

        }
	}
}