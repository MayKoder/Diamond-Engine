using System;
using DiamondEngine;

public class QuitConfirmation : DiamondComponent
{
	public GameObject confirmScreen = null;
	public GameObject bigBrother = null;
	public void OnExecuteButton()
	{
		if (gameObject.Name == "Cancel")
		{
			bigBrother.Enable(true);
			confirmScreen.Enable(false);
		}
		if (gameObject.Name == "QuittoDesktop")
			InternalCalls.CloseGame();
		if (gameObject.Name == "QuittoMenu")
			SceneManager.LoadScene(1726826608);
	}
	public void Update()
	{

	}

}
