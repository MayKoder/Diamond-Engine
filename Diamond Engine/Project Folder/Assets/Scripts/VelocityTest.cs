using System;
using DiamondEngine;

public class VelocityTest : DiamondComponent
{
    public float forcex = 0;
    public float forcey = 0;
    public float forcez = 0;

    public float velocityx = 0;
    public float velocityy = 0;
    public float velocityz = 0;

    public void Update()
	{
        if (Input.GetMouseClick(MouseButton.RIGHT) == KeyState.KEY_REPEAT)
        {
            gameObject.SetVelocity(new Vector3(velocityx, velocityy, velocityz));
        }
        if (Input.GetMouseClick(MouseButton.LEFT) == KeyState.KEY_REPEAT)
        {
            gameObject.AddForce(new Vector3(forcex, forcey, forcez));
        }
    }

}