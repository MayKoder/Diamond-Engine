using System;
using DiamondEngine;

public class Settings : DiamondComponent
{
	private GameObject settingsWindow = null;
	public GameObject bigBrother = null;
	private bool toDisable = false;
	private bool firstFrame = true;
	public void OnExecuteCheckbox(bool active)
	{
		if (active)
			Debug.Log("Controller Vibration ON");
		else 
			Debug.Log("Controller Vibration OFF");
	}
	public void OnExecuteButton()
	{
		if (gameObject.Name == "BrightnessUp")
			Config.SetBrightness(Config.GetBrightness() + 0.05f); Debug.Log(Config.GetBrightness());
		if (gameObject.Name == "BrightnessDown")
			Config.SetBrightness(Config.GetBrightness() - 0.05f); Debug.Log(Config.GetBrightness());

		if (gameObject.Name == "MasterVolume")
			Debug.Log("MasterVolume");
		if (gameObject.Name == "MasterVolumeUp")
			Debug.Log("MasterVolumeUp");
		if (gameObject.Name == "MasterVolumeDown")
			Debug.Log("MasterVolumeDown");

		if (gameObject.Name == "MusicVolume")
			Debug.Log("MusicVolume");
		if (gameObject.Name == "MusicVolumeUp")
			Debug.Log("MusicVolumeUp");
		if (gameObject.Name == "MusicVolumeDown")
			Debug.Log("MusicVolumeDown");

		if (gameObject.Name == "SFXVolume")
			Debug.Log("SFXVolume");
		if (gameObject.Name == "SFXVolumeUp")
			Debug.Log("SFXVolumeUp");
		if (gameObject.Name == "SFXVolumeDown")
			Debug.Log("SFXVolumeDown");

		if (gameObject.Name == "Return")
			toDisable = true;
	}
	public void Update()
	{
		if (firstFrame)
		{
			toDisable = firstFrame = false;

		}
		if (Input.GetGamepadButton(DEControllerButton.B) == KeyState.KEY_DOWN || toDisable)
		{
			toDisable = false;
			bigBrother.Enable(true);
			settingsWindow.Enable(false);
		}
	}

}