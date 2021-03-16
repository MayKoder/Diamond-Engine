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
		if (gameObject.Name == "Return")
			toDisable = true;
		if (gameObject.Name == "ResolutionUp")
			Config.SetResolution(Config.GetResolution() + 1);
		if (gameObject.Name == "ResolutionDown")
			Config.SetResolution(Config.GetResolution() - 1);
		if (gameObject.Name == "WindowModeUp")
			Config.SetWindowMode(Config.GetWindowMode() + 1);
		if (gameObject.Name == "WindowModeDown")
			Config.SetWindowMode(Config.GetWindowMode() - 1);
	}
	public void Update()
	{
		if (firstFrame)
			toDisable = firstFrame = false;
		if (Input.GetGamepadButton(DEControllerButton.B) == KeyState.KEY_DOWN || toDisable)
		{
			toDisable = false;
			bigBrother.Enable(true);
			displayScreen.Enable(false);
		}
	}

}