using System;
using DiamondEngine;

public class HumanWalk : DiamondComponent
{
	public GameObject thisReference = null;

	public void Update()
	{
        if (Input.GetKey(DEKeyCode.W) == KeyState.KEY_REPEAT)
			gameObject.transform.localPosition += gameObject.transform.GetForward() * 10 * Time.deltaTime;
	}

}
