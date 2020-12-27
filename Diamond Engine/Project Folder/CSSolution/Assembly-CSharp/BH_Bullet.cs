using System;
using DiamondEngine;


public class BH_Bullet
{
    public GameObject thisReference; //This is needed until i make all this be part of a component base class

    public float speed = 0.10f;
    public float maxLifeTime = 5.0f;

    private float currentLifeTime = 0.0f;
    public void Update()
    {
        currentLifeTime += Time.deltaTime;
        if (currentLifeTime >= maxLifeTime)
        {
            Debug.Log(/*thisReference.ToString() +*/ "Bullet removed after:  " + currentLifeTime.ToString());
            InternalCalls.Destroy(thisReference);
        }

        thisReference.localPosition += thisReference.GetForward() * speed;
        //thisReference.position += (Vector3.up * -1) * Time.deltaTime;

        //Debug.Log(Time.deltaTime);
    }

    //Things i need
        //Spawn gameobject with mesh...

}

