using System;
using DiamondEngine;

public class Stormtrooper : Enemy
{
	public void Start()
	{
		currentState = STATES.WANDER;
		targetPosition = CalculateNewPosition(wanderRange);
		shotTimes = 0;
		Debug.Log("Started");
	}

	public void Update()
	{
		switch (currentState)
		{
			case STATES.IDLE:
				//Debug.Log("Idle");

				timePassed += Time.deltaTime;

				LookAt(player.transform.globalPosition);

				if (Mathf.Distance(gameObject.transform.globalPosition, player.transform.globalPosition) < range)
				{
					if(timePassed > idleTime)
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
				//Debug.Log("Run");

				MoveToPosition(targetPosition, runningSpeed);

				if (Mathf.Distance(gameObject.transform.localPosition, targetPosition) < stoppingDistance)
				{
					currentState = STATES.IDLE;
					timePassed = 0.0f;
				}
			
				break;

			case STATES.WANDER:

				//Debug.Log("Wander");

				if (player == null)
					Debug.Log("Null player");

				
				// If the player is in range attack him
				if (Mathf.Distance(gameObject.transform.globalPosition, player.transform.globalPosition) < range)
				{
					currentState = STATES.SHOOT;
					timePassed = timeBewteenShots;
					float rotation = (float)Math.Acos(Vector3.Dot(gameObject.transform.globalPosition, player.transform.globalPosition)
																		 / (gameObject.transform.globalPosition.magnitude * player.transform.globalPosition.magnitude));

					rotation *= (180 / (float)Math.PI);
					Debug.Log("Angle: " + rotation);

					//gameObject.transform.localRotation = new Quaternion(0, rotation, 0);
				}
				else  //if not, keep wandering
				{
					if (targetPosition == null)
						targetPosition = CalculateNewPosition(wanderRange);

					MoveToPosition(targetPosition, wanderSpeed);

					if (Mathf.Distance(gameObject.transform.localPosition, targetPosition) < stoppingDistance)
					{
						targetPosition = CalculateNewPosition(wanderRange);
					}
				}
				
				break;

			case STATES.SHOOT:
				Debug.Log("Shoot");
				
				timePassed += Time.deltaTime;

				//float angle = (float)Math.Acos(Vector3.Dot(gameObject.transform.globalPosition, player.transform.globalPosition));

				//add aiming rotation


				if (timePassed > timeBewteenShots)
				{
					Shoot();

					if (shotTimes > 2)
					{
						currentState = STATES.RUN;
						targetPosition = CalculateNewPosition(range);
						shotTimes = 0;
					}
				}
				break;

			case STATES.HIT:
				break;

			case STATES.DIE:
				Debug.Log("Die");
				InternalCalls.Destroy(gameObject);
				break;
		}
		
	}

	public void OnTriggerEnter()
	{
		//state = STATES.HIT;
	}
}