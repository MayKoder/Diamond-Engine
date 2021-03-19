using System;
using DiamondEngine;

public class Settings : DiamondComponent
{
	public GameObject settingsWindow = null;
	public GameObject bigBrother = null;

	public void OnExecuteCheckbox(bool active)
	{
		if (active)
		{
			Config.ControllerVibrationEnable(true); 
			Input.PlayHaptic(0.5f, 1);
		}
		else
		{
			Config.ControllerVibrationEnable(false);
			Input.PlayHaptic(0.5f, 5);
		}
	}
	public void OnExecuteButton()
	{
		if (gameObject.Name == "BrightnessUp")
        {
			Config.SetBrightness(Config.GetBrightness() + 0.05f); 
			Debug.Log(Config.GetBrightness().ToString());
        }
		else if (gameObject.Name == "BrightnessDown")
        {
			Config.SetBrightness(Config.GetBrightness() - 0.05f); 
			Debug.Log(Config.GetBrightness().ToString());
        }

		else if (gameObject.Name == "MasterVolume")
        {
			Debug.Log("MasterVolume");
        }
		else if (gameObject.Name == "MasterVolumeUp")
        {
			Debug.Log("MasterVolumeUp");
        }
		else if (gameObject.Name == "MasterVolumeDown")
        {
			Debug.Log("MasterVolumeDown");
        }

		else if (gameObject.Name == "MusicVolume")
        {
			Debug.Log("MusicVolume");
        }
		else if (gameObject.Name == "MusicVolumeUp")
        {
			Debug.Log("MusicVolumeUp");
        }
		else if (gameObject.Name == "MusicVolumeDown")
        {
			Debug.Log("MusicVolumeDown");
        }

		else if (gameObject.Name == "SFXVolume")
        {
			Debug.Log("SFXVolume");
        }
		else if (gameObject.Name == "SFXVolumeUp")
        {
			Debug.Log("SFXVolumeUp");
        }
		else if (gameObject.Name == "SFXVolumeDown")
        {
			Debug.Log("SFXVolumeDown");
        }

		else if (gameObject.Name == "Return")
        {
			bigBrother.Enable(true);
			settingsWindow.Enable(false);
		}
	}
	public void Update()
	{

	}

}