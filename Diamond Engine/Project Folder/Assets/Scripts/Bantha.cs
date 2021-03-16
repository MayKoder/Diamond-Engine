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
				Debug.Log("Idle");

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
				Debug.Log("Running");

				LookAt(targetPosition);
				MoveToPosition(targetPosition, runningSpeed);

				if (Mathf.Distance(gameObject.transform.localPosition, targetPosition) < stoppingDistance)
				{
					currentState = STATES.IDLE;
					timePassed = 0.0f;
				}
				break;
			case STATES.WANDER:
				Debug.Log("Wander");

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
				Debug.Log("Charging");

				timePassed += Time.deltaTime;

				LookAt(player.transform.globalPosition);
				
				Debug.Log(player.transform.localPosition.ToString());
				Debug.Log(gameObject.transform.localPosition.ToString());

				MoveToPosition(player.transform.localPosition, bulletSpeed);

				if (Mathf.Distance(gameObject.transform.localPosition, player.transform.localPosition) < stoppingDistance)
				{
					currentState = STATES.IDLE;
					timePassed = 0.0f;
				}

				break;
			case STATES.HIT:
				Debug.Log("Being Hit");

				break;

			case STATES.DIE:
				Debug.Log("Dying");

				InternalCalls.Destroy(gameObject);
				break;
		}


	}
	public void Charge()
    {

	}

}