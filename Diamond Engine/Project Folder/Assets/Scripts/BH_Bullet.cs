using System;
using DiamondEngine;


public class BH_Bullet : DiamondComponent
{
    public GameObject thisReference = null; //This is needed until i make all this be part of a component base class

    public float speed = 60.0f;
    public float maxLifeTime = 5.0f;

    public float currentLifeTime = 0.0f;

    public float yVel = 0.0f;

    public void Update()
    {
        currentLifeTime += Time.deltaTime;

        thisReference.localPosition += thisReference.GetForward() * (speed * Time.deltaTime);

        yVel -= Time.deltaTime / 15.0f;
        thisReference.localPosition += (Vector3.up * yVel);

        if (currentLifeTime >= maxLifeTime)
        {
            InternalCalls.Destroy(this.thisReference);
        }
    }
}


