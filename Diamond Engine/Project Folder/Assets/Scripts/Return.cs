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
			bigBrother.Enable(true);
			controlsWindow.Enable(false);
		}
	}
	public void Update()
	{
	}

}