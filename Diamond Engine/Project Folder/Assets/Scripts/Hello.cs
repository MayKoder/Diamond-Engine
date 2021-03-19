using System;
using DiamondEngine;

public class Hello : DiamondComponent
{

	public GameObject helloWorld = null;

    public void OnExecuteCheckbox(bool checkbox_active)
    {
        if(checkbox_active)
            Debug.Log("Checkbox activated");
        else
            Debug.Log("Checkbox disactivated");
    }
    public void OnExecuteButton()
    {
        SceneManager.LoadScene(1076838722);
    }
    public void Update()
	{
		//Debug.Log(helloWorld.ToString());
	}


}


