using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using DiamondEngine;

public class Core : DiamondComponent
{
	public GameObject reference = null;
	public GameObject turret = null;
	public GameObject shootPoint = null;
    public int Holas = 0;

    public int testInt = 0;
		
	public float rotationSpeed = 2.0f;
	public float movementSpeed = 25.0f;

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
            reference.localPosition += reference.GetForward() * movementSpeed * Time.deltaTime;
        if (InternalCalls.GetKey(DEKeyCode.S) == KeyState.KEY_REPEAT)
            reference.localPosition += reference.GetForward() * -movementSpeed * Time.deltaTime;
        if (InternalCalls.GetKey(DEKeyCode.A) == KeyState.KEY_REPEAT)
            reference.localRotation *= Quaternion.RotateAroundAxis(Vector3.up, rotationSpeed * Time.deltaTime);
        if (InternalCalls.GetKey(DEKeyCode.D) == KeyState.KEY_REPEAT)
            reference.localRotation *= Quaternion.RotateAroundAxis(Vector3.up, -rotationSpeed * Time.deltaTime);

        if (InternalCalls.GetMouseX() != 0 && turret != null)
        {
            turret.localRotation *= Quaternion.RotateAroundAxis(Vector3.up, -InternalCalls.GetMouseX() * Time.deltaTime);
        }
        //if (InternalCalls.GetMouseY() != 0 && turret != null)
        //{
        //    turret.localRotation *= Quaternion.RotateAroundAxis(turret.globalMatrix.GetRight(), InternalCalls.GetMouseY() * 0.001f);
        //}

        if (InternalCalls.GetKey(DEKeyCode.SPACE) == KeyState.KEY_REPEAT)
        {
            InternalCalls.CreateBullet(shootPoint.globalPosition, shootPoint.globalRotation, shootPoint.globalScale);
        }

        //InternalCalls.CreateBullet(Vector3.zero, Quaternion.identity, Vector3.one);

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

