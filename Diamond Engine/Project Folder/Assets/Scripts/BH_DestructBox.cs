using System;
using DiamondEngine;

public class BH_DestructBox : DiamondComponent
{
	public GameObject thisReference;

	public void Update()
	{

	}

	public void OnTriggerEnter()
	{
		InternalCalls.Destroy(thisReference);
	}

}