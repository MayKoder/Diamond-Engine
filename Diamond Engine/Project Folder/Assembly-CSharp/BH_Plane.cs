using System;
using DiamondEngine;


public class BH_Plane : DiamondComponent
{
    public GameObject thisReference = null;
    public float speedIncrement = 0.05f;

    float cSpeed = 0.0f;

    public void Update()
    {
        if (InternalCalls.GetKey(DEKeyCode.W) == KeyState.KEY_REPEAT && cSpeed < 0.1f)
            cSpeed += speedIncrement * Time.deltaTime;

        if ((InternalCalls.GetKey(DEKeyCode.S) == KeyState.KEY_REPEAT || InternalCalls.GetKey(DEKeyCode.W) == KeyState.KEY_IDLE) && cSpeed > 0.05f)
        {
            cSpeed -= speedIncrement * Time.deltaTime;
            if(cSpeed < 0.05f)
                cSpeed = 0.05f;
        }

        if (InternalCalls.GetKey(DEKeyCode.A) == KeyState.KEY_REPEAT)
            thisReference.localRotation = Quaternion.RotateAroundAxis(Vector3.up, 0.4f * Time.deltaTime) * thisReference.localRotation;

        if (InternalCalls.GetKey(DEKeyCode.D) == KeyState.KEY_REPEAT)
            thisReference.localRotation = Quaternion.RotateAroundAxis(Vector3.up, -0.4f * Time.deltaTime) * thisReference.localRotation;

        if (InternalCalls.GetMouseX() > 0)
            thisReference.localRotation = thisReference.localRotation * Quaternion.RotateAroundAxis(Vector3.forward, InternalCalls.GetMouseX() * 0.5f * Time.deltaTime);

        if (InternalCalls.GetMouseX() < 0)
            thisReference.localRotation = thisReference.localRotation * Quaternion.RotateAroundAxis(Vector3.forward, InternalCalls.GetMouseX() * 0.5f * Time.deltaTime);

        if (InternalCalls.GetMouseY() > 0)
            thisReference.localRotation = thisReference.localRotation * Quaternion.RotateAroundAxis(Vector3.right, InternalCalls.GetMouseY() * 0.5f * Time.deltaTime);

        if (InternalCalls.GetMouseY() < 0)
            thisReference.localRotation = thisReference.localRotation * Quaternion.RotateAroundAxis(Vector3.right, InternalCalls.GetMouseY() * 0.5f * Time.deltaTime);

        thisReference.localPosition += thisReference.GetForward() * cSpeed;
    }




}

