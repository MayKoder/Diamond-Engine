using System;
using DiamondEngine;

public class Confirmation : DiamondComponent
{
	public GameObject confirmationWindow = null;
	private bool toDisable = false;
	private bool firstFrame = true;
	public void OnExecuteButton()
	{
		if (gameObject.Name == "Yes")
			InternalCalls.CloseGame();
		if (gameObject.Name == "No")
			toDisable = true;
	}
	public void Update()
	{
		if (firstFrame)
			toDisable = firstFrame = false;
		if (Input.GetGamepadButton(DEControllerButton.B) == KeyState.KEY_DOWN || toDisable)
		{
			toDisable = false;
			confirmationWindow.Enable(false);
		}
	}

}