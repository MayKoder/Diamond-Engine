using System;
using DiamondEngine;

public class DisplayOptions : DiamondComponent
{
	public GameObject optionsWindow = null;
	public GameObject settings = null;
	public GameObject display = null;
	public GameObject controls = null;
	public GameObject bigBrother = null;
	private bool toDisable = false;
	private bool firstFrame = true;
	public void onExecuteCheckbox()
    {
		Debug.Log("Vsync");
    }
	public void OnExecuteButton()
	{
		if (gameObject.Name == "Settings")
        {
			settings.Enable(true);
			optionsWindow.Enable(false);
        }
		if (gameObject.Name == "Display")
		{
			display.Enable(true);
			optionsWindow.Enable(false);
		}
		if (gameObject.Name == "Controls")
		{
			controls.Enable(true);
			optionsWindow.Enable(false);
		}
		if (gameObject.Name == "Return")
			toDisable = true;
	}
	public void Update()
	{
		if (firstFrame)
			toDisable = firstFrame = false;
		if (Input.GetGamepadButton(DEControllerButton.B) == KeyState.KEY_DOWN || toDisable)
		{
			toDisable = false;
			bigBrother.Enable(true);
			optionsWindow.Enable(false);
		}
	}

}