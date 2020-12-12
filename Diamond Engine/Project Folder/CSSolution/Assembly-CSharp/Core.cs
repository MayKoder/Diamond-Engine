using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace DiamondEngine
{
    class Core
    {
		public GameObject reference = null;
		public int test = 0;
		public int Update(int x)
		{
			x += 10;
			if (reference == null)
				return 0;

			//InternalCalls.CreateGameObject("Test Game Object", Vector3.one);
			//Log does not work if we compile with Release wtf?
			if (InternalCalls.GetKey(DEKeyCode.W) == true)
				reference.position.z += 0.1f;
			if (InternalCalls.GetKey(DEKeyCode.S) == true)
				reference.position.z -= 0.1f;
			if (InternalCalls.GetKey(DEKeyCode.A) == true)
				reference.position.x += 0.1f;
			if (InternalCalls.GetKey(DEKeyCode.D) == true)
				reference.position.x -= 0.1f;

			return 0;
		}

		static void Main()
		{

		}
	}
}
