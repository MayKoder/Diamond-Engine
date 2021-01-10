using System;
using DiamondEngine;

public class Hello : DiamondComponent
{

	public GameObject helloWorld = null;

	public void Update()
	{
		Debug.Log(helloWorld.ToString());
	}

}
