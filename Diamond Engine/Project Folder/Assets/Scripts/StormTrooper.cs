using System;
using DiamondEngine;

public class StormTrooper : Enemy
{

	public void Start()
    {
		currentState = STATES.WANDER;
		targetPosition = CalculateNewPosition();
		shotTimes = 0;
		Debug.Log("Started");
    }

	public void Update()
	{
        switch (currentState)
        {
			case STATES.IDLE:
				timePassed += Time.deltaTime;
				if(timePassed > idleTime)
                {
					currentState = STATES.WANDER;
					timePassed = 0.0f;
					targetPosition = CalculateNewPosition();
                }
				break;

			case STATES.RUN:
				gameObject.transform.localPosition = Vector3.Lerp(gameObject.transform.localPosition, targetPosition, 0.01f);
				
				if (Mathf.Distance(gameObject.transform.localPosition, targetPosition) < stoppingDistance)
				{
					currentState = STATES.IDLE;
				}
				
				break;

			case STATES.WANDER:

				//Debug.Log("Wander");

				// If the player is in range attack him
				if (Mathf.Distance(gameObject.transform.localPosition, player.transform.localPosition) < range)
				{
					currentState = STATES.SHOOT;
					timePassed = timeBewteenShots;
					float rotation = (float)Math.Acos(Vector3.Dot(gameObject.transform.globalPosition, player.transform.globalPosition)
																		 / (gameObject.transform.globalPosition.magnitude * player.transform.globalPosition.magnitude));

					rotation *= (180 / (float)Math.PI);
					Debug.Log("Angle: " + rotation);

					gameObject.transform.localRotation = new Quaternion(0, rotation, 0);
				}
				else  //if not, keep wandering
                { 
					if (targetPosition == null)
						targetPosition = CalculateNewPosition();

					Vector3 direction = targetPosition - gameObject.transform.localPosition;

					gameObject.transform.localPosition += direction.normalized * wanderSpeed * Time.deltaTime;

					if (Mathf.Distance(gameObject.transform.localPosition, targetPosition) < stoppingDistance)
					{
						targetPosition = CalculateNewPosition();
					}
				}
				
				break;

			case STATES.SHOOT:
				timePassed += Time.deltaTime;

				float angle = (float)Math.Acos(Vector3.Dot(gameObject.transform.globalPosition, player.transform.globalPosition));

				//add aiming rotation

				if (timePassed > timeBewteenShots)
					Shoot();

				if(shotTimes > 2)
                {
					currentState = STATES.RUN;
					targetPosition = CalculateNewPosition();
					shotTimes = 0;
                }

				break;

			case STATES.HIT:
				break;

			case STATES.DIE:
				InternalCalls.Destroy(gameObject);
				break;
		}
	}

	public void OnTriggerEnter()
	{
		//state = STATES.HIT;
	}

}