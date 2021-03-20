using System;
using DiamondEngine;

public class DisSett : DiamondComponent
{
	public GameObject displayScreen = null;
	public GameObject bigBrother = null;

	public void OnExecuteCheckbox(bool active)
    {
		if (active)
			Config.VSYNCEnable(true);
		else
			Config.VSYNCEnable(false);
	}
	public void OnExecuteButton()
	{
		if (gameObject.Name == "Return")
		{
			bigBrother.Enable(true);
			displayScreen.Enable(false);
		}
		else if (gameObject.Name == "ResolutionUp")
			Config.SetResolution(Config.GetResolution() + 1);
		else if (gameObject.Name == "ResolutionDown")
			Config.SetResolution(Config.GetResolution() - 1);
		else if (gameObject.Name == "WindowModeUp")
		{ Config.SetWindowMode(Config.GetWindowMode() + 1); Debug.Log("Res should go up"); }
		else if (gameObject.Name == "WindowModeDown")
		{ Config.SetWindowMode(Config.GetWindowMode() - 1); Debug.Log("Res should go down"); }

		Debug.Log("Executed");
	}
	public void Update()
	{

	}

}