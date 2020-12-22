using System;
using DiamondEngine;


public class BH_Bullet
{
    public GameObject thisReference; //This is needed until i make all this be part of a component base class

    public float speed;
    public float maxLifeTime;

    private float currentLifeTime = 0;
    public void Update()
    {
        currentLifeTime += Time.deltaTime;
        if (currentLifeTime >= maxLifeTime)
            InternalCalls.Destroy(thisReference);

        thisReference.position += thisReference.globalMatrix.GetForward() * speed;
        //thisReference.position += (Vector3.up * -1) * Time.deltaTime;

        //Debug.Log(Time.deltaTime);
    }

    //Things i need
        //DT
        //Spawn gameobject with mesh...

}

