using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using DiamondEngine;

public class Core : DiamondComponent
{
    public GameObject reference = null;
    public GameObject shootPoint = null;

    public float rotationSpeed = 2.0f;
    public float movementSpeed = 35.0f;
    public float mouseSens = 1.0f;
    public float delayTime = 0.2f;
    private float timePassed = 0.0f;

    //public Vector3 testOtherClass; //Should find a way to tell if the class is a gameobject or not

    public void Update(/*int x*/)
    {
        if (this.reference == null)
            return;

        if (Input.GetKey(DEKeyCode.W) == KeyState.KEY_REPEAT || Input.GetLeftAxisY() < -30000)
        {
            Vector3 vectorUp = new Vector3(-0.5f, 0, 0.5f);
            reference.localPosition += vectorUp * movementSpeed * Time.deltaTime;
        }

        if (Input.GetKey(DEKeyCode.S) == KeyState.KEY_REPEAT || Input.GetLeftAxisY() > 30000)
        {
            Vector3 vectorDown = new Vector3(0.5f, 0, -0.5f);
            reference.localPosition += vectorDown * movementSpeed * Time.deltaTime;
        }
        if (Input.GetKey(DEKeyCode.A) == KeyState.KEY_REPEAT || Input.GetLeftAxisX() < -30000)
        {
            Vector3 vectorLeft = new Vector3(0.5f, 0, 0.5f);
            reference.localPosition += vectorLeft * movementSpeed * Time.deltaTime;
        }
        if (Input.GetKey(DEKeyCode.D) == KeyState.KEY_REPEAT || Input.GetLeftAxisX() > 30000)
        {
            Vector3 vectorRight = new Vector3(-0.5f, 0, -0.5f);
            reference.localPosition += vectorRight * movementSpeed * Time.deltaTime;
        }

        if (Input.GetMouseX() != 0 && reference != null)
            reference.localRotation = Quaternion.RotateAroundAxis(Vector3.up, -Input.GetMouseX() * mouseSens * Time.deltaTime) * reference.localRotation;

        //if (Input.GetMouseY() != 0 && turret != null)
        //    turret.localRotation = turret.localRotation * Quaternion.RotateAroundAxis(Vector3.right, -Input.GetMouseY() * Time.deltaTime);

        timePassed += Time.deltaTime;

        if ((Input.GetMouseClick(MouseButton.LEFT) == KeyState.KEY_REPEAT || Input.GetRightTrigger() > 0) && timePassed >= delayTime)
        {
            InternalCalls.CreateBullet(shootPoint.globalPosition, shootPoint.globalRotation, shootPoint.globalScale);
            timePassed = 0.0f;
        }
    }
}