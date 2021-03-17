using System;
using DiamondEngine;

public class DisSett : DiamondComponent
{
	public GameObject displayScreen = null;
	public GameObject bigBrother = null;
	private bool toDisable = false;
	private bool firstFrame = true;

	public void OnExecuteCheckbox(bool active)
    {
		if (active)
			Config.VSYNCEnable(true);
		else
			Config.VSYNCEnable(false);
	}
	public void OnExecuteButton()
	{
		Debug.Log("Enter button");
		if (gameObject.Name == "Return")
			toDisable = true;
		else if (gameObject.Name == "ResolutionUp")
			Config.SetResolution(Config.GetResolution() + 1);
		else if (gameObject.Name == "ResolutionDown")
			Config.SetResolution(Config.GetResolution() - 1);
		else  if (gameObject.Name == "WindowModeUp")
			Config.SetWindowMode(Config.GetWindowMode() + 1);
		else if (gameObject.Name == "WindowModeDown")
			Config.SetWindowMode(Config.GetWindowMode() - 1);
		
		Debug.Log("Executed");
	}
	public void Update()
	{
		if (firstFrame)
			toDisable = firstFrame = false;
		if (toDisable)
		{
			toDisable = false;
			bigBrother.Enable(true);
			displayScreen.Enable(false);
		}
	}

}