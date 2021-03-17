using System;
using DiamondEngine;

public class Return : DiamondComponent
{
	public GameObject controlsWindow = null;
	public GameObject bigBrother = null;
	private bool toDisable = false;
	private bool firstFrame = true;
	public void OnExecuteButton()
	{
		if (gameObject.Name == "Return")
			toDisable = true;
	}
	public void Update()
	{
		if (firstFrame)
			toDisable = firstFrame = false;
		if (toDisable)
		{
			toDisable = false;
			bigBrother.Enable(true);
			controlsWindow.Enable(false);
		}
	}

}