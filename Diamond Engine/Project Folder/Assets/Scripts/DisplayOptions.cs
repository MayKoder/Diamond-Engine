using System;
using DiamondEngine;

public class DisplayOptions : DiamondComponent
{
	public GameObject optionsWindow = null;
	public GameObject settings = null;
	public GameObject display = null;
	public GameObject controls = null;
	public GameObject bigBrother = null;
	public GameObject background = null;
	public GameObject default_selected = null;

	public void OnExecuteButton()
	{
		if (gameObject.Name == "Settings")
        {
			settings.EnableNav(true);
			optionsWindow.EnableNav(false);
			if (default_selected != null)
            {
				default_selected.GetComponent<Navigation>().Select();
				default_selected.GetComponent<Settings>().FirstFrame();

            }
		}
		if (gameObject.Name == "Display")
		{
			display.EnableNav(true);
			optionsWindow.EnableNav(false);
			if (default_selected != null)
				default_selected.GetComponent<Navigation>().Select();
		}
		if (gameObject.Name == "Controls")
		{
			controls.EnableNav(true);
			optionsWindow.EnableNav(false);
			if (default_selected != null)
				default_selected.GetComponent<Navigation>().Select();
		}
		if (gameObject.Name == "Return")
		{
			bigBrother.EnableNav(true);
			optionsWindow.EnableNav(false);
			background.EnableNav(false);
		}
	}
	public void Update()
	{

	}

}