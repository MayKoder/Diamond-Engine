using System;
using DiamondEngine;

public class Pause : DiamondComponent
{
	public GameObject pauseWindow = null;
	public GameObject settingsWindow = null;
	public GameObject displayWindow = null;
	public GameObject controlsWindow = null;
	public GameObject quitWindow = null;
	private bool toDisable = false;
	private bool firstFrame = true;

	public void OnExecuteButton()
	{
		if (gameObject.Name == "Continue")
			toDisable = true;
		if (gameObject.Name == "Settings")
        {
			settingsWindow.Enable(true);
			pauseWindow.Enable(false);
        }
		if (gameObject.Name == "Display")
		{
			displayWindow.Enable(true);
			pauseWindow.Enable(false);
		}
		if (gameObject.Name == "Controls")
		{
			controlsWindow.Enable(true);
			pauseWindow.Enable(false);
		}
		if (gameObject.Name == "Quit")
		{
			quitWindow.Enable(true);
			pauseWindow.Enable(false);
		}
	}
	public void Update()
	{
		if (firstFrame)
			toDisable = firstFrame = false;
		if (Input.GetGamepadButton(DEControllerButton.B) == KeyState.KEY_DOWN || toDisable)
		{
			toDisable = false;
			pauseWindow.Enable(false);
		}
	}

}