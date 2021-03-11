using System;
using DiamondEngine;

public class StormTrooper : DiamondComponent
{
	public float dmg;
	public float bullet_speed;
	public float range;
	public float speed;

	private STATES state;
	private enum STATES
    {
		IDLE,
		SHOOT,
		RUN,
		HIT,
		DIE
    }

	public void Start()
    {
		state = STATES.IDLE;
    }

	public void Update()
	{
        switch (state)
        {
			case STATES.IDLE:
				break;
			case STATES.RUN:
				break;
			case STATES.SHOOT:
				break;
			case STATES.HIT:
				break;
			case STATES.DIE:
				break;
		}
	}

	public bool Shoot()
    {
		return true;
    }

	public bool Idle()
    {
		return true;
    }

	public bool Hit()
    {
		return true;
    }

	public bool Run()
    {
		return true;
    }

}