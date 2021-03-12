using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using DiamondEngine;

public class Core : DiamondComponent
{
    public GameObject shootPoint = null;

    public float rotationSpeed = 2.0f;
    public float movementSpeed = 35.0f;
    public float mouseSens = 1.0f;
    public float delayTime = 0.2f;
    private float timePassed = 0.0f;

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

        if (Input.GetMouseX() != 0 && gameObject.transform != null)
            gameObject.transform.localRotation = Quaternion.RotateAroundAxis(Vector3.up, -Input.GetMouseX() * mouseSens * Time.deltaTime) * gameObject.transform.localRotation;

        //if (Input.GetMouseY() != 0 && turret != null)
        //    turret.localRotation = turret.localRotation * Quaternion.RotateAroundAxis(Vector3.right, -Input.GetMouseY() * Time.deltaTime);

        timePassed += Time.deltaTime;

        if ((Input.GetMouseClick(MouseButton.LEFT) == KeyState.KEY_REPEAT || Input.GetRightTrigger() > 0) && timePassed >= delayTime)
        {
            InternalCalls.CreateBullet(shootPoint.transform.globalPosition, shootPoint.transform.globalRotation, shootPoint.transform.globalScale);
            timePassed = 0.0f;
            Input.PlayHaptic(1f,30);
        }
	}
}
