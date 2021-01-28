using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using DiamondEngine;

public class Core : DiamondComponent
{
	public GameObject reference = null;
	public GameObject turret = null;
	public GameObject shootPoint = null;
		
	public float rotationSpeed = 2.0f;
	public float movementSpeed = 35.0f;
    public float mouseSens = 1.0f;

	public bool testBool = false;
	public string testString = "Hello World";

    //public Vector3 testOtherClass; //Should find a way to tell if the class is a gameobject or not

	public void Update(/*int x*/)
	{
		if (this.reference == null)
			return;

 
        if (Input.GetKey(DEKeyCode.W) == KeyState.KEY_REPEAT)
            reference.localPosition += reference.GetForward() * movementSpeed * Time.deltaTime;
        if (Input.GetKey(DEKeyCode.S) == KeyState.KEY_REPEAT)
            reference.localPosition += reference.GetForward() * -movementSpeed * Time.deltaTime;
        if (Input.GetKey(DEKeyCode.A) == KeyState.KEY_REPEAT)
            reference.localRotation *= Quaternion.RotateAroundAxis(Vector3.up, rotationSpeed * Time.deltaTime);
        if (Input.GetKey(DEKeyCode.D) == KeyState.KEY_REPEAT)
            reference.localRotation *= Quaternion.RotateAroundAxis(Vector3.up, -rotationSpeed * Time.deltaTime);


        if (Input.GetMouseX() != 0 && turret != null)
            turret.localRotation = Quaternion.RotateAroundAxis(Vector3.up, -Input.GetMouseX() * mouseSens * Time.deltaTime) * turret.localRotation;

        //if (Input.GetMouseY() != 0 && turret != null)
        //    turret.localRotation = turret.localRotation * Quaternion.RotateAroundAxis(Vector3.right, -Input.GetMouseY() * Time.deltaTime);

        if (Input.GetMouseClick(MouseButton.LEFT) == KeyState.KEY_REPEAT)
        {
            InternalCalls.CreateBullet(shootPoint.globalPosition, shootPoint.globalRotation, shootPoint.globalScale);
        }
	}
}
















