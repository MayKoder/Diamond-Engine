using System;
using DiamondEngine;

public class HumanWalk : DiamondComponent
{
	public GameObject thisReference = null;
	public void Update()
	{
        if (Input.GetKey(DEKeyCode.W) == KeyState.KEY_REPEAT)
            thisReference.localPosition += thisReference.GetForward() * 10 * Time.deltaTime;
	}

}
