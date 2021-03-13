using System;
using DiamondEngine;

public class StartMenu : DiamondComponent
{
	public void OnExecuteButton()
	{
		if (gameObject.Name == "Play")
			SceneManager.LoadScene(1482507639);
		else if (gameObject.Name == "Options")
			SceneManager.LoadScene(950542469);
		else if (gameObject.Name == "Quit")
			InternalCalls.CloseGame();
	}
	public void Update()
	{

	}

}