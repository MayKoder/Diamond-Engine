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
			Debug.Log("Vsync ON");
		else
			Debug.Log("Vsync OFF");
	}
	public void OnExecuteButton()
	{
		if (gameObject.Name == "Return")
			toDisable = true;
		if (gameObject.Name == "ResolutionUp")
			Debug.Log("Res Up");
		if (gameObject.Name == "ResolutionDown")
			Debug.Log("Res Down");
		if (gameObject.Name == "WindowModeUp")
			Debug.Log("Win Up");
		if (gameObject.Name == "WindowModeDown")
			Debug.Log("Win Down");
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