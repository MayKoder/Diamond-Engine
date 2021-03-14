using System;
using DiamondEngine;

public class HUD : DiamondComponent
{
	public int hp;
	public int force;
	public int currency;
	public GameObject hp_bar;
	public GameObject hp_number_gameobject;
	public GameObject force_bar;
	public GameObject skill_push;
	public GameObject weapon_bar;
	public GameObject primary_weapon;
	public GameObject secondary_weapon;
	public GameObject currency_number_gameobject;
	public void Update()
	{
		if(Input.GetKey(DEKeyCode.C) == KeyState.KEY_DOWN)
        {
			String curren = currency_number_gameobject.GetComponent<Text>().text;
			currency = int.Parse(curren);
			currency++;
			currency_number_gameobject.GetComponent<Text>().text=currency.ToString();
        }
		if (Input.GetKey(DEKeyCode.H) == KeyState.KEY_DOWN)
		{
			String hp_get = hp_number_gameobject.GetComponent<Text>().text;
			hp = int.Parse(hp_get);
			if(hp<50)
				hp++;
			hp_number_gameobject.GetComponent<Text>().text = hp.ToString();
		}
		if (Input.GetKey(DEKeyCode.D) == KeyState.KEY_DOWN)
		{
			String hp_get = hp_number_gameobject.GetComponent<Text>().text;
			hp = int.Parse(hp_get);
			if(hp>0)
				hp--;
			hp_number_gameobject.GetComponent<Text>().text = hp.ToString();
		}
	}

}