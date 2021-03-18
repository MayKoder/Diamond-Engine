using System;
using DiamondEngine;

public class HUD : DiamondComponent
{
	public int hp = 0;
	public int max_hp = 0;
	public int force = 0;
	public int max_force = 0;
	public int currency = 10000;
	public int bullets_main_weapon = 0;
	public int max_bullets_main_weapon = 0;
	public int bullets_secondary_weapon = 0;
	public int max_bullets_secondary_weapon = 0;
	public int combo_number = 0;
	public int combo_seconds = 0;
	public bool main_weapon = true;
	public GameObject hp_bar = null;
	public GameObject hp_number_gameobject = null;
	public GameObject force_bar = null;
	public GameObject skill_push = null;
	public GameObject weapon_bar = null;
	public GameObject primary_weapon = null;
	public GameObject secondary_weapon = null;
	public GameObject currency_number_gameobject = null;
	public GameObject combo_bar = null;
	public GameObject combo_text = null;
	public GameObject combo_gameobject = null;

	private float combo_time_limit;
	private float combo_time;
	public void Update()
	{
		if(Input.GetKey(DEKeyCode.C) == KeyState.KEY_DOWN)
        {
			currency++;
			UpdateCurrency(currency);
        }
		if (Input.GetKey(DEKeyCode.H) == KeyState.KEY_DOWN)
		{
			if (hp < max_hp)
			{
				hp+=5;
				UpdateHP(hp, max_hp);
			}
		}
		if (Input.GetKey(DEKeyCode.D) == KeyState.KEY_DOWN)
		{
			if (hp > 0)
			{
				hp-=5;
				UpdateHP(hp, max_hp);
			}
		}
		if (Input.GetKey(DEKeyCode.F) == KeyState.KEY_DOWN)
		{
			if (force > 0)
			{
				force-=10;
                if (force == 0)
                {
					ChangeAlphaSkillPush(false);

				}
				UpdateForce(force, max_force);
			}
		}
		if (Input.GetKey(DEKeyCode.M) == KeyState.KEY_DOWN)
		{
			if (force < max_force)
			{
				if (force == 0)
				{
					ChangeAlphaSkillPush(true);

				}
				force +=10;
				UpdateForce(force, max_force);

			}
		}
		if (Input.GetKey(DEKeyCode.S) == KeyState.KEY_DOWN)
		{
			if (main_weapon)
            {
				if (bullets_main_weapon > 0)
				{
					bullets_main_weapon--;
					UpdateBullets(bullets_main_weapon, max_bullets_main_weapon);
				}
			}
            else
            {
				if (bullets_secondary_weapon > 0)
				{
					bullets_secondary_weapon--;
					UpdateBullets(bullets_secondary_weapon, max_bullets_secondary_weapon);

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
					UpdateBullets(bullets_main_weapon, max_bullets_main_weapon);

				}
			}
            else
            {
				if (bullets_secondary_weapon < 10)
				{
					bullets_secondary_weapon++;
					UpdateBullets(bullets_secondary_weapon, max_bullets_secondary_weapon);
				}
			}
				
		}
		if (Input.GetKey(DEKeyCode.W) == KeyState.KEY_DOWN)
		{
			SwapWeapons();
		}
        if (Input.GetKey(DEKeyCode.B) == KeyState.KEY_DOWN)
        {
			combo_number++;
			ComboIncrease(combo_number, combo_seconds);
		}
        if (combo_bar != null && combo_number > 0)
        {
			if(!UpdateCombo(combo_number, combo_seconds, combo_time_limit))
            {
				combo_number = 0;
            }

		}
	}

	public void ComboIncrease(int number_combo, int seconds)
    {
		if (number_combo == 1)
		{
			combo_gameobject.Enable(true);
			combo_text.GetComponent<Text>().color_red = 0;
			combo_text.GetComponent<Text>().color_green = 0.8f;
			combo_text.GetComponent<Text>().color_blue = 1;
		}

		combo_time_limit = Time.totalTime + seconds;

		if (combo_bar != null)
		{
			combo_bar.GetComponent<Material>().SetIntUniform("combo_number", number_combo);
			combo_bar.GetComponent<Material>().SetFloatUniform("length_used", combo_time_limit / seconds);
		}
		if (combo_text == null)
			return;
		combo_text.GetComponent<Text>().text = "x" + number_combo.ToString();
		if (number_combo == 10)
		{
			combo_text.GetComponent<Text>().color_red = 0;
			combo_text.GetComponent<Text>().color_green = 1;
			combo_text.GetComponent<Text>().color_blue = 0;
		}
		else if (number_combo == 25)
		{
			combo_text.GetComponent<Text>().color_red = 1;
			combo_text.GetComponent<Text>().color_green = 1;
			combo_text.GetComponent<Text>().color_blue = 0;
		}
		else if (number_combo == 45)
		{
			combo_text.GetComponent<Text>().color_red = 0.79f;
			combo_text.GetComponent<Text>().color_green = 0.28f;
			combo_text.GetComponent<Text>().color_blue = 0.96f;
		}
		else if (number_combo == 77)
		{
			combo_text.GetComponent<Text>().color_red = 1;
			combo_text.GetComponent<Text>().color_green = 1;
			combo_text.GetComponent<Text>().color_blue = 1;
		}
	}

	public bool UpdateCombo(int number_combo, int seconds, float limit)
    {
		combo_time = Time.totalTime;
		if (combo_time > limit)
		{
			number_combo = 0;
			combo_bar.GetComponent<Material>().SetIntUniform("combo_number", number_combo);
			combo_gameobject.Enable(false);
			return false;
		}
		combo_bar.GetComponent<Material>().SetFloatUniform("length_used", (limit - combo_time) / seconds);
		return true;
	}
	public void UpdateHP(int new_hp, int max_hp)
    {
		if(hp_number_gameobject!=null)
			hp_number_gameobject.GetComponent<Text>().text = new_hp.ToString();
		if (hp_bar == null)
			return;
		float hp_float = new_hp;
		hp_float /= max_hp;
		hp_bar.GetComponent<Material>().SetFloatUniform("length_used", hp_float);
	}

	public void UpdateForce(int new_force, int max_force)
    {
		if (force_bar == null)
			return;
		float force_float = new_force;
		force_float /= max_force;
		force_bar.GetComponent<Material>().SetFloatUniform("length_used", force_float);
	}

	public void ChangeAlphaSkillPush(bool alpha_full)
    {
		if (skill_push == null)
			return;
		if(alpha_full)
			skill_push.GetComponent<Material>().SetFloatUniform("alpha", 1.0f);
		else
			skill_push.GetComponent<Material>().SetFloatUniform("alpha", 0.5f);
	}

	public void SwapWeapons()
    {
		if (primary_weapon != null && secondary_weapon != null)
			primary_weapon.GetComponent<Image2D>().SwapTwoImages(secondary_weapon);
		main_weapon = !main_weapon;
		if (weapon_bar == null)
			return;
		if (main_weapon)
		{
			float bullets_float = bullets_main_weapon;
			bullets_float /= max_bullets_main_weapon;
			weapon_bar.GetComponent<Material>().SetFloatUniform("length_used", bullets_float);
		}
		else
		{
			float bullets_float = bullets_secondary_weapon;
			bullets_float /= max_bullets_secondary_weapon;
			weapon_bar.GetComponent<Material>().SetFloatUniform("length_used", bullets_float);
		}
	}

	public void UpdateBullets(int num_bullets, int max_bullets)
    {
		if (weapon_bar == null)
			return;
		float bullets_float = num_bullets;
		bullets_float /= max_bullets;
		weapon_bar.GetComponent<Material>().SetFloatUniform("length_used", bullets_float);
	}

	public void UpdateCurrency(int total_currency)
    {
		if (currency_number_gameobject == null)
			return;
		currency_number_gameobject.GetComponent<Text>().text = total_currency.ToString();
	}

}