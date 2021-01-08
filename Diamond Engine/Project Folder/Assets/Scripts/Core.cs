using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using DiamondEngine;

public class Core : DiamondComponent
{
	public GameObject reference = null;
	public GameObject turret = null;
	public GameObject shootPoint = null;
    //public int Holas = 0;

    public int testInt = 0;
		
	public float rotationSpeed = 2.0f;
	public float movementSpeed = 25.0f;

	public bool testBool = false;
	public string testString = "Hello World";

    public Vector3 testOtherClass; //Should find a way to tell if the class is a gameobject or not

	public void Update(/*int x*/)
	{
		//x += 10;
		if (this.reference == null)
			return;

        //Debug.Log(reference.GetForward());

        //Log does not work if we compile with Release wtf?
        if (Input.GetKey(DEKeyCode.W) == KeyState.KEY_REPEAT)
            reference.localPosition += reference.GetForward() * movementSpeed * Time.deltaTime;
        if (Input.GetKey(DEKeyCode.S) == KeyState.KEY_REPEAT)
            reference.localPosition += reference.GetForward() * -movementSpeed * Time.deltaTime;
        if (Input.GetKey(DEKeyCode.A) == KeyState.KEY_REPEAT)
            reference.localRotation *= Quaternion.RotateAroundAxis(Vector3.up, rotationSpeed * Time.deltaTime);
        if (Input.GetKey(DEKeyCode.D) == KeyState.KEY_REPEAT)
            reference.localRotation *= Quaternion.RotateAroundAxis(Vector3.up, -rotationSpeed * Time.deltaTime);

        //if (InternalCalls.GetMouseY() != 0 && turret != null)
        //{
        //    turret.localRotation *= Quaternion.RotateAroundAxis(turret.GetRight(), InternalCalls.GetMouseY() * 0.001f);
        //}
        if (Input.GetMouseX() != 0 && turret != null)
        {
            turret.localRotation *= Quaternion.RotateAroundAxis(Vector3.up, -Input.GetMouseX() * Time.deltaTime);
        }

        if (Input.GetKey(DEKeyCode.SPACE) == KeyState.KEY_REPEAT)
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

