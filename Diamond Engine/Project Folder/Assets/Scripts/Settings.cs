using System;
using DiamondEngine;

public class Settings : DiamondComponent
{
	public GameObject settingsWindow = null;
	public GameObject bigBrother = null;
	public GameObject bar = null;


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
		}
	}
	public void FirstFrame()
    {
		if ((gameObject.Name == "BrightnessDown" || gameObject.Name == "BrightnessUp") && bar != null)
			bar.GetComponent<Material>().SetFloatUniform("length_used", Config.GetBrightness());
		if ((gameObject.Name == "MasterVolumeUp" || gameObject.Name == "MasterVolumeDown") && bar != null)
			bar.GetComponent<Material>().SetFloatUniform("length_used", Config.GetMasterVolume() / 99);
		if ((gameObject.Name == "MusicVolumeUp" || gameObject.Name == "MusicVolumeDown") && bar != null)
			bar.GetComponent<Material>().SetFloatUniform("length_used", Config.GetMusicVolume() / 99);
		if ((gameObject.Name == "SFXVolumeUp" || gameObject.Name == "SFXVolumeDown") && bar != null)
			bar.GetComponent<Material>().SetFloatUniform("length_used", Config.GetSFXVolume() / 99);
	}
	public void OnExecuteButton()
	{
		if (gameObject.Name == "BrightnessUp")
        {
			Config.SetBrightness(Config.GetBrightness() + 0.05f);
			Debug.Log(Config.GetBrightness().ToString());
			if (bar == null)
				return;
			bar.GetComponent<Material>().SetFloatUniform("length_used",Config.GetBrightness());
        }
		else if (gameObject.Name == "BrightnessDown")
        {
			Config.SetBrightness(Config.GetBrightness() - 0.05f); 
			Debug.Log(Config.GetBrightness().ToString());
			if (bar == null)
				return;
			bar.GetComponent<Material>().SetFloatUniform("length_used", Config.GetBrightness());
		}
		else if (gameObject.Name == "MasterVolumeUp")
        {
			Config.SetMasterVolume(Config.GetMasterVolume() + 5.0f);
			if (bar == null)
				return;
			bar.GetComponent<Material>().SetFloatUniform("length_used", Config.GetMasterVolume()/99);
		}
		else if (gameObject.Name == "MasterVolumeDown")
        {
			Config.SetMasterVolume(Config.GetMasterVolume() - 5.0f);
			if (bar == null)
				return;
			bar.GetComponent<Material>().SetFloatUniform("length_used", Config.GetMasterVolume() / 99);
		}
		else if (gameObject.Name == "MusicVolumeUp")
        {
			Config.SetMusciVolume(Config.GetMusicVolume() + 5.0f);
			if (bar == null)
				return;
			bar.GetComponent<Material>().SetFloatUniform("length_used", Config.GetMusicVolume() / 99);
		}
		else if (gameObject.Name == "MusicVolumeDown")
        {
			Config.SetMusciVolume(Config.GetMusicVolume() - 5.0f);
			if (bar == null)
				return;
			bar.GetComponent<Material>().SetFloatUniform("length_used", Config.GetMusicVolume() / 99);
		}
		else if (gameObject.Name == "SFXVolumeUp")
        {
			Config.SetSFXVolume(Config.GetSFXVolume() + 5.0f);
			if (bar == null)
				return;
			bar.GetComponent<Material>().SetFloatUniform("length_used", Config.GetSFXVolume() / 99);
		}
		else if (gameObject.Name == "SFXVolumeDown")
        {
			Config.SetSFXVolume(Config.GetSFXVolume() - 5.0f);
			if (bar == null)
				return;
			bar.GetComponent<Material>().SetFloatUniform("length_used", Config.GetSFXVolume() / 99);
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