using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using System.Collections.Generic;

using DiamondEngine;

public class Core : DiamondComponent
{
	public GameObject turret = null;
	public GameObject shootPoint = null;
		
	public float rotationSpeed = 2.0f;
	public float movementSpeed = 35.0f;
    public float mouseSens = 1.0f;

	public bool testBool = false;
	public string testString = "Hello World";

    public int[] hola = new int[5] {8,2, 1, 3, 4 };
    //public List<int> holas = new List<int>() {5, 80, 2, 1};

    //public Vector3 testOtherClass; //Should find a way to tell if the class is a gameobject or not

	public void Update(/*int x*/)
	{
        if (Input.GetKey(DEKeyCode.W) == KeyState.KEY_REPEAT)
            gameObject.transform.localPosition += gameObject.transform.GetForward() * movementSpeed * Time.deltaTime;
        if (Input.GetKey(DEKeyCode.S) == KeyState.KEY_REPEAT)
            gameObject.transform.localPosition += gameObject.transform.GetForward() * -movementSpeed * Time.deltaTime;
        if (Input.GetKey(DEKeyCode.A) == KeyState.KEY_REPEAT)
            gameObject.transform.localRotation *= Quaternion.RotateAroundAxis(Vector3.up, rotationSpeed * Time.deltaTime);
        if (Input.GetKey(DEKeyCode.D) == KeyState.KEY_REPEAT)
            gameObject.transform.localRotation *= Quaternion.RotateAroundAxis(Vector3.up, -rotationSpeed * Time.deltaTime);


        if (Input.GetMouseX() != 0 && turret != null)
            turret.transform.localRotation = Quaternion.RotateAroundAxis(Vector3.up, -Input.GetMouseX() * mouseSens * Time.deltaTime) * turret.transform.localRotation;

        //if (Input.GetMouseY() != 0 && turret != null)
        //    turret.localRotation = turret.localRotation * Quaternion.RotateAroundAxis(Vector3.right, -Input.GetMouseY() * Time.deltaTime);

        if (Input.GetMouseClick(MouseButton.LEFT) == KeyState.KEY_REPEAT)
        {
            //InternalCalls.CreateBullet(shootPoint.globalPosition, shootPoint.globalRotation, shootPoint.globalScale);
            //Debug.Log(gameObject.transform.pointer.ToString());
           // Debug.Log(gameObject.transform.globalPosition.ToString());
        }
	}
}
















