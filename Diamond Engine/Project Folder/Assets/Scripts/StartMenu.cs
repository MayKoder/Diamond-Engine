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
			menuButtons.EnableNav(false);
			options.EnableNav(true);
			background.EnableNav(true);
			if (default_selected != null)
				default_selected.GetComponent<Navigation>().Select();
		}
		else if (gameObject.Name == "Quit")
		{
			options.EnableNav(true);
			menuButtons.EnableNav(false);
		}
	}
	public void Update()
	{

	}

}