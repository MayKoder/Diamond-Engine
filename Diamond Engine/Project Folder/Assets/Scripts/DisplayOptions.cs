using System;
using DiamondEngine;

public class DisplayOptions : DiamondComponent
{
	public GameObject displayWindow = null;
	public GameObject bigBrother = null;
	private bool toDisable = false;
	private bool firstFrame = true;
	public void onExecuteCheckbox()
    {
		Debug.Log("Vsync");
    }
	public void OnExecuteButton()
	{
		if (gameObject.Name == "Resoltion")
			Debug.Log("Res");
		if (gameObject.Name == "ResoltionUp")
			Debug.Log("ResUp");
		if (gameObject.Name == "ResoltionDown")
			Debug.Log("ResDown");
		if (gameObject.Name == "WindowsMode")
			Debug.Log("WinMode");
		if (gameObject.Name == "WindowsModeUp")
			Debug.Log("WinModeUp");
		if (gameObject.Name == "WindowsModeDown")
			Debug.Log("WinModeDown");
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
			displayWindow.Enable(false);
		}
	}

}