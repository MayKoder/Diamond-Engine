using System;
using DiamondEngine;

public class HUD : DiamondComponent
{
	public int hp;
	public int force;
	public int currency;
	public int bullets_main_weapon;
	public int bullets_secondary_weapon;
	public bool main_weapon;
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
			if (hp < 50)
			{
				hp+=5;
				float hp_float = hp;
				hp_float /= 50;
				hp_bar.GetComponent<Material>().SetFloatUniform("length_used", hp_float);
				hp_number_gameobject.GetComponent<Text>().text = hp.ToString();
			}
		}
		if (Input.GetKey(DEKeyCode.D) == KeyState.KEY_DOWN)
		{
			String hp_get = hp_number_gameobject.GetComponent<Text>().text;
			hp = int.Parse(hp_get);
			if (hp > 0)
			{
				hp-=5;
				float hp_float = hp;
				hp_float /= 50;
				hp_bar.GetComponent<Material>().SetFloatUniform("length_used", hp_float);
				hp_number_gameobject.GetComponent<Text>().text = hp.ToString();
			}
		}
		if (Input.GetKey(DEKeyCode.F) == KeyState.KEY_DOWN)
		{
			if (force > 0)
			{
				force-=10;
                if (force == 0)
                {
					skill_push.GetComponent<Material>().SetFloatUniform("alpha", 0.5f);

				}
				float force_float = force;
				force_float /= 50;
				force_bar.GetComponent<Material>().SetFloatUniform("length_used", force_float);
			}
		}
		if (Input.GetKey(DEKeyCode.M) == KeyState.KEY_DOWN)
		{
			if (force < 50)
			{
				if (force == 0)
				{
					skill_push.GetComponent<Material>().SetFloatUniform("alpha", 1.0f);

				}
				force +=10;
				float force_float = force;
				force_float /= 50;
				force_bar.GetComponent<Material>().SetFloatUniform("length_used", force_float);
			}
		}
		if (Input.GetKey(DEKeyCode.S) == KeyState.KEY_DOWN)
		{
			if (main_weapon)
            {
				if (bullets_main_weapon > 0)
				{
					bullets_main_weapon--;
					float bullets_float = bullets_main_weapon;
					bullets_float /= 10;
					weapon_bar.GetComponent<Material>().SetFloatUniform("length_used", bullets_float);
				}
			}
            else
            {
				if (bullets_secondary_weapon > 0)
				{
					bullets_secondary_weapon--;
					float bullets_float = bullets_secondary_weapon;
					bullets_float /= 10;
					weapon_bar.GetComponent<Material>().SetFloatUniform("length_used", bullets_float);
				}
			}
			
			
		}
		if (Input.GetKey(DEKeyCode.R) == KeyState.KEY_DOWN)
		{
			if (main_weapon)
			{
				if (bullets_main_weapon < 10)
				{
					bullets_main_weapon++;
					float bullets_float = bullets_main_weapon;
					bullets_float /= 10;
					weapon_bar.GetComponent<Material>().SetFloatUniform("length_used", bullets_float);
				}
			}
            else
            {
				if (bullets_secondary_weapon < 10)
				{
					bullets_secondary_weapon++;
					float bullets_float = bullets_secondary_weapon;
					bullets_float /= 10;
					weapon_bar.GetComponent<Material>().SetFloatUniform("length_used", bullets_float);
				}
			}
				
		}
		if (Input.GetKey(DEKeyCode.W) == KeyState.KEY_DOWN)
		{
			primary_weapon.GetComponent<Image2D>().SwapTwoImages(secondary_weapon);
			main_weapon = !main_weapon;
            if (main_weapon)
            {
				float bullets_float = bullets_main_weapon;
				bullets_float /= 10;
				weapon_bar.GetComponent<Material>().SetFloatUniform("length_used", bullets_float);
			}
            else
            {
				float bullets_float = bullets_secondary_weapon;
				bullets_float /= 10;
				weapon_bar.GetComponent<Material>().SetFloatUniform("length_used", bullets_float);
			}
		}
	}

}