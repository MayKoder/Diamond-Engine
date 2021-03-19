using System;
using DiamondEngine;

public class QuitConfirmation : DiamondComponent
{
	public GameObject confirmScreen = null;
	public GameObject bigBrother = null;
	private bool toDisable = false;
	private bool firstFrame = true;
	public void OnExecuteButton()
	{
		if (gameObject.Name == "Cancel")
			toDisable = true;
		if (gameObject.Name == "QuittoDesktop")
			InternalCalls.CloseGame();
		if (gameObject.Name == "QuittoMenu")
			SceneManager.LoadScene(1726826608);
	}
	public void Update()
	{
		if (firstFrame)
			toDisable = firstFrame = false;
		if (toDisable)
		{
			toDisable = false;
			bigBrother.Enable(true);
			confirmScreen.Enable(false);
		}
	}

}
