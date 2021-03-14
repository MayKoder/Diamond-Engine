using System;
using DiamondEngine;

public class StartMenu : DiamondComponent
{
	public GameObject menuButtons;
	public GameObject options;
	public void OnExecuteButton()
	{
		if (gameObject.Name == "Play")
			SceneManager.LoadScene(1482507639);
		else if (gameObject.Name == "Options")
		{
			menuButtons.Enable(false);
			options.Enable(true);
		}
		else if (gameObject.Name == "Quit")
			InternalCalls.CloseGame();
	}
	public void Update()
	{

	}

}