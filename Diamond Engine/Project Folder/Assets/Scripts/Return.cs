using System;
using DiamondEngine;

public class Return : DiamondComponent
{
	public GameObject controlsWindow = null;
	public GameObject bigBrother = null;
	public void OnExecuteButton()
	{
		if (gameObject.Name == "Return")
        {
			bigBrother.EnableNav(true);
			controlsWindow.EnableNav(false);
		}
	}
	public void Update()
	{
	}

}