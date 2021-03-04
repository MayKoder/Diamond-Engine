using System;
using DiamondEngine;

public class FPS_Controller : DiamondComponent
{
    public GameObject reference = null;
    public GameObject turret = null;

    public float rotationSpeed = 2.0f;
    public float movementSpeed = 35.0f;
    public float mouseSens = 1.0f;

    public void OnCollisionEnter()
    {
        Debug.Log("Ayo i've been called.");
       // InternalCalls.Destroy(reference);
    }

    public void Update()
	{
        if (this.reference == null)
            return;

        if (Input.GetKey(DEKeyCode.W) == KeyState.KEY_REPEAT)
            reference.localPosition += reference.GetForward() * movementSpeed * Time.deltaTime;
        if (Input.GetKey(DEKeyCode.S) == KeyState.KEY_REPEAT)
            reference.localPosition += reference.GetForward() * -movementSpeed * Time.deltaTime;
        if (Input.GetKey(DEKeyCode.A) == KeyState.KEY_REPEAT)
            reference.localPosition += reference.GetRight() * movementSpeed * Time.deltaTime;
        if (Input.GetKey(DEKeyCode.D) == KeyState.KEY_REPEAT)
            reference.localPosition += reference.GetRight() * -movementSpeed * Time.deltaTime;

        if (Input.GetMouseX() != 0 && turret != null)
            turret.localRotation = Quaternion.RotateAroundAxis(Vector3.up, -Input.GetMouseX() * mouseSens * Time.deltaTime) * turret.localRotation;
    }
}