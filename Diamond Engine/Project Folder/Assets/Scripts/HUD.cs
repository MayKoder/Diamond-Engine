using System;
using DiamondEngine;

public class HUD : DiamondComponent
{
	public int hp;
	public int force=50;
	public int currency;
	public int bullets;
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
			if (bullets > 0)
			{
				bullets--;
				float bullets_float = bullets;
				bullets_float /= 10;
				weapon_bar.GetComponent<Material>().SetFloatUniform("length_used", bullets_float);
			}
		}
		if (Input.GetKey(DEKeyCode.R) == KeyState.KEY_DOWN)
		{
			if (bullets < 10)
			{
				bullets++;
				float bullets_float = bullets;
				bullets_float /= 10;
				weapon_bar.GetComponent<Material>().SetFloatUniform("length_used", bullets_float);
			}
		}
	}

}