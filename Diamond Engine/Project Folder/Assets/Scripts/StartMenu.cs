using System;
using DiamondEngine;

public class StartMenu : DiamondComponent
{
	public void OnExecuteButton()
	{
		if (gameObject.Name == "PlayButton")
			SceneManager.LoadScene(1482507639);
		else if (gameObject.Name == "OptionsButton")
			SceneManager.LoadScene(950542469);
		else if (gameObject.Name == "QuitButton")
			InternalCalls.CloseGame();
	}
	public void Update()
	{

	}

}