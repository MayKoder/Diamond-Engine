using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace DiamondEngine
{
    class Core
    {
		public GameObject reference = null;

		public int testInt = 0;
		public float rotationSpeed = 0.0f;
		public bool testBool = false;
		public string testString = "Hello World";

		public int Update(int x)
		{
			x += 10;
			if (reference == null)
				return 0;

			//InternalCalls.CreateGameObject("Test Game Object", Vector3.one);
			//Log does not work if we compile with Release wtf?
			if (InternalCalls.GetKey(DEKeyCode.W) == KeyState.KEY_REPEAT)
				reference.position.z += 0.1f;
			if (InternalCalls.GetKey(DEKeyCode.S) == KeyState.KEY_REPEAT)
				reference.position.z -= 0.1f;
			if (InternalCalls.GetKey(DEKeyCode.A) == KeyState.KEY_REPEAT)
				reference.position.x += 0.1f;
			if (InternalCalls.GetKey(DEKeyCode.D) == KeyState.KEY_REPEAT)
				reference.position.x -= 0.1f;

			if (InternalCalls.GetKey(DEKeyCode.Q) == KeyState.KEY_REPEAT)
            {
				//InternalCalls.CSLog("Before: " + reference.rotation.ToString());
				reference.rotation *= Quaternion.RotateAroundAxis(Vector3.up, rotationSpeed);
				//InternalCalls.CSLog("After: " + reference.rotation.ToString());
			}
			if (InternalCalls.GetKey(DEKeyCode.E) == KeyState.KEY_REPEAT)
            {
				reference.rotation *= Quaternion.RotateAroundAxis(Vector3.up, -rotationSpeed);
            }

			if (InternalCalls.GetKey(DEKeyCode.SPACE) == KeyState.KEY_DOWN)
				InternalCalls.CreateGameObject("Bullet", reference.position);

			if (InternalCalls.GetKey(DEKeyCode.T) == KeyState.KEY_DOWN)
            {
				//InternalCalls.CSLog(reference.name.ToString());
				//InternalCalls.CSLog(testInt.ToString());
				//InternalCalls.CSLog(testFloat.ToString());
				//InternalCalls.CSLog(testBool.ToString());
				//InternalCalls.CSLog(testString.ToString());
				reference.globalMatrix.SetFromTRS(reference.position, reference.rotation, reference.scale);

    //            for (int i = 0; i < 16; i += 4)
    //            {
				//	InternalCalls.CSLog(reference.globalMatrix.v[i].ToString() + reference.globalMatrix.v[i + 1].ToString() + reference.globalMatrix.v[i + 2].ToString()+ reference.globalMatrix.v[i + 3].ToString());
				//}


			}

			return 0;
		}

		static void Main()
		{

		}
	}
}
