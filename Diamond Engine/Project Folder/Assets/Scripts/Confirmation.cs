using System;
using DiamondEngine;

public class Confirmation : DiamondComponent
{
	public GameObject confirmationWindow = null;
	public GameObject mainMenu = null;
	public void OnExecuteButton()
	{
		if (gameObject.Name == "Yes")
			InternalCalls.CloseGame();
		if (gameObject.Name == "No")
        {
			mainMenu.EnableNav(true);
			confirmationWindow.EnableNav(false);
		}
	}
	public void Update()
	{

	}

}