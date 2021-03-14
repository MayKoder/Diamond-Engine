using System;
using DiamondEngine;

public class Settings : DiamondComponent
{
	private GameObject settingsWindow = null;
	public GameObject bigBrother = null;
	private bool toDisable = false;
	private bool firstFrame = true;
	public void onExecuteCheckbox()
	{
		Debug.Log("Controller Vibration");
	}
	public void OnExecuteButton()
	{
		if (gameObject.Name == "Brightness")
			Debug.Log("Brightness");
		if (gameObject.Name == "BrightnessUp")
			Debug.Log("BrightnessUp");
		if (gameObject.Name == "BrightnessDown")
			Debug.Log("BrightnessDown");

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