using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using DiamondEngine;

public class Core
{
	public GameObject reference = null;
	public GameObject turret = null;
	public GameObject shootPoint = null;

	public int testInt = 0;
		
	public float rotationSpeed = 0.002f;
	public float movementSpeed = 0.025f;

	public bool testBool = false;
	public string testString = "Hello World";

	public bool once = true;

	public void Update(/*int x*/)
	{
		//x += 10;
		if (this.reference == null)
			return;

        //Log does not work if we compile with Release wtf?
        if (InternalCalls.GetKey(DEKeyCode.W) == KeyState.KEY_REPEAT)
            reference.position += reference.GetForward() * movementSpeed;
        if (InternalCalls.GetKey(DEKeyCode.S) == KeyState.KEY_REPEAT)
            reference.position += reference.GetForward() * -movementSpeed;
        if (InternalCalls.GetKey(DEKeyCode.A) == KeyState.KEY_REPEAT)
            reference.rotation *= Quaternion.RotateAroundAxis(Vector3.up, rotationSpeed);
        if (InternalCalls.GetKey(DEKeyCode.D) == KeyState.KEY_REPEAT)
            reference.rotation *= Quaternion.RotateAroundAxis(Vector3.up, -rotationSpeed);

        if (InternalCalls.GetMouseX() != 0 && turret != null)
        {
            turret.rotation *= Quaternion.RotateAroundAxis(Vector3.up, -InternalCalls.GetMouseX() * 0.001f);
        }
        //if (InternalCalls.GetMouseY() != 0 && turret != null)
        //{
        //    turret.rotation *= Quaternion.RotateAroundAxis(turret.globalMatrix.GetRight(), InternalCalls.GetMouseY() * 0.001f);
        //}

        if (InternalCalls.GetKey(DEKeyCode.SPACE) == KeyState.KEY_DOWN)
            InternalCalls.CreateBullet(shootPoint.position, shootPoint.rotation, shootPoint.scale);

        //if (rotationSpeed != 0.0f)
        //{
        //    this.reference.rotation *= Quaternion.RotateAroundAxis(Vector3.up, -this.rotationSpeed);

        //    reference.rotation *= Quaternion.RotateAroundAxis(Vector3.up, this.rotationSpeed);
        //    reference.position += reference.globalMatrix.GetForward() * movementSpeed;

        //    Debug.Log(reference.UID.ToString());
        //Debug.Log(reference.position.ToString());
        //}
        //GetHashCode();
        return;
	}
}

