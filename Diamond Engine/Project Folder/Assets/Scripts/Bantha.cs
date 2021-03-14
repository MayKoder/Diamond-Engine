using System;
using DiamondEngine;

public class Bantha : Enemy
{
	public void Start()
	{
		currentState = STATES.WANDER;
		targetPosition = CalculateNewPosition(wanderRange);
		shotTimes = 0;
	}
	public void Update()
	{
        switch (currentState)
        {
			case STATES.IDLE:
				break;
			case STATES.RUN:
				break;
			case STATES.WANDER:
				break;
			case STATES.SHOOT:
				break;
			case STATES.HIT:
				break;
			case STATES.DIE:
				InternalCalls.Destroy(gameObject);
				break;
		}
	}

}