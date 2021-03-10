using System;
using DiamondEngine;

public class FPS_Controller : DiamondComponent
{
    public GameObject turret = null;

    public float rotationSpeed = 2.0f;
    public float movementSpeed = 35.0f;
    public float mouseSens = 1.0f;

    public void Update()
	{
        if (Input.GetKey(DEKeyCode.W) == KeyState.KEY_REPEAT)
            gameObject.transform.localPosition += gameObject.transform.GetForward() * movementSpeed * Time.deltaTime;
        if (Input.GetKey(DEKeyCode.S) == KeyState.KEY_REPEAT)
            gameObject.transform.localPosition += gameObject.transform.GetForward() * -movementSpeed * Time.deltaTime;
        if (Input.GetKey(DEKeyCode.A) == KeyState.KEY_REPEAT)
            gameObject.transform.localPosition += gameObject.transform.GetRight() * movementSpeed * Time.deltaTime;
        if (Input.GetKey(DEKeyCode.D) == KeyState.KEY_REPEAT)
            gameObject.transform.localPosition += gameObject.transform.GetRight() * -movementSpeed * Time.deltaTime;


        if (Input.GetMouseX() != 0 && turret != null)
            turret.transform.localRotation = Quaternion.RotateAroundAxis(Vector3.up, -Input.GetMouseX() * mouseSens * Time.deltaTime) * turret.transform.localRotation;
    }

}