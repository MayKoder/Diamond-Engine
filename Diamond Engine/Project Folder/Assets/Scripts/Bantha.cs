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
				timePassed += Time.deltaTime;

				if (InRange(player.transform.globalPosition, range))
				{
					LookAt(player.transform.globalPosition);

					if (timePassed > idleTime)
					{
						currentState = STATES.SHOOT;
						timePassed = timeBewteenShots;
					}
				}
				else
				{
					if (timePassed > idleTime)
					{
						currentState = STATES.WANDER;
						timePassed = 0.0f;
						targetPosition = CalculateNewPosition(wanderRange);
					}
				}
				break;
			case STATES.RUN:
				LookAt(targetPosition);
				MoveToPosition(targetPosition, runningSpeed);

				if (Mathf.Distance(gameObject.transform.localPosition, targetPosition) < stoppingDistance)
				{
					currentState = STATES.IDLE;
					timePassed = 0.0f;
				}
				break;
			case STATES.WANDER:
				
				// If the player is in range attack him
				if (InRange(player.transform.globalPosition, range))
				{
					currentState = STATES.SHOOT;
					timePassed = timeBewteenShots;
				}
				else  //if not, keep wandering
				{
					if (targetPosition == null)
						targetPosition = CalculateNewPosition(wanderRange);

					LookAt(targetPosition);
					MoveToPosition(targetPosition, wanderSpeed);

					if (Mathf.Distance(gameObject.transform.localPosition, targetPosition) < stoppingDistance)
					{
						//targetPosition = CalculateNewPosition(wanderRange);
						currentState = STATES.IDLE;
						timePassed = 0.0f;
					}
				}
				break;
			case STATES.SHOOT:
				timePassed += Time.deltaTime;

				LookAt(player.transform.globalPosition);

				if (timePassed > timeBewteenShots)
				{
					Charge();
				}

				break;
			case STATES.HIT:
				break;
			case STATES.DIE:
				InternalCalls.Destroy(gameObject);
				break;
		}


	}
	public void Charge()
    {
		MoveToPosition(player.transform.globalPosition, bulletSpeed);

		if (Mathf.Distance(gameObject.transform.localPosition, player.transform.globalPosition) < stoppingDistance)
		{
			currentState = STATES.IDLE;
			timePassed = 0.0f;
		}
	}

}