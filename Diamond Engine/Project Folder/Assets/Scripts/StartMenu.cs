using System;
using DiamondEngine;

public class StartMenu : DiamondComponent
{
	public GameObject menuButtons = null;
	public GameObject options = null;
	public GameObject background = null;
	public GameObject default_selected = null;
	public void OnExecuteButton()
	{
		if (gameObject.Name == "Play")
			SceneManager.LoadScene(1482507639);
		else if (gameObject.Name == "Options")
		{
			menuButtons.Enable(false);
			options.Enable(true);
			background.Enable(true);
			if (default_selected != null)
				default_selected.GetComponent<Navigation>().Select();
		}
		else if (gameObject.Name == "Quit")
		{
			options.Enable(true);
			menuButtons.Enable(false);
		}
	}
	public void Update()
	{

	}

}