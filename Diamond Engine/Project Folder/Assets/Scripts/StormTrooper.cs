using System;
using DiamondEngine;

public class StormTrooper : DiamondComponent
{
	public GameObject player;
	public GameObject shootPoint;

	public float speed;
	public int range;
	public float damage;
	public float bullet_speed;
	private int shotTimes = 0;


	private float timeBewteenShots = 5.0f;
	private float timePassed = 0.0f;

	public float wanderTime = 5.0f;
	public float idleTime = 5.0f;
	private Vector3 targetPosition;
	private float stoppingDistance = 1.0f;

	private STATES state;
	private enum STATES
    {
		IDLE,
		RUN,
		WANDER,
		SHOOT,
		HIT,
		DIE
    }

	public void Start()
    {
		state = STATES.WANDER;
		shotTimes = 0;
    }

	public void Update()
	{
        switch (state)
        {
			case STATES.IDLE:
				timePassed += Time.deltaTime;
				if(timePassed > idleTime)
                {
					state = STATES.WANDER;
					timePassed = 0.0f;
					targetPosition = CalculateNewPosition();
                }
				break;

			case STATES.RUN:
				gameObject.transform.localPosition = Vector3.Lerp(gameObject.transform.localPosition, targetPosition, 0.01f);
				if (Mathf.Distance(gameObject.transform.localPosition, targetPosition) < stoppingDistance)
				{
					state = STATES.IDLE;
				}

				break;

			case STATES.WANDER:

				Debug.Log("Wander");

				gameObject.transform.localPosition = Vector3.Lerp(gameObject.transform.localPosition, player.transform.localPosition, 0.01f);

				if(gameObject.transform == null)
                {
					Debug.Log("Null transform");
                }

				if (Mathf.Distance(gameObject.transform.localPosition, targetPosition) < stoppingDistance)
				{
					targetPosition = CalculateNewPosition();
				}
			
				break;

			case STATES.SHOOT:
				timePassed += Time.deltaTime;
				if (timePassed > timeBewteenShots)
					Shoot();

				if(shotTimes > 2)
                {
					state = STATES.RUN;
					shotTimes = 0;
                }

				break;

			case STATES.HIT:
				break;

			case STATES.DIE:
				break;
		}
	}

	public bool Shoot()
    {
		InternalCalls.CreateBullet(shootPoint.transform.globalPosition, shootPoint.transform.globalRotation, shootPoint.transform.globalScale);
		timePassed = 0.0f;
		shotTimes++;

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

	Vector3 CalculateNewPosition()
    {
		Vector3 newPosition = new Vector3(0,0,0);

		Random random = new Random();

        newPosition.x = random.Next(range);
		newPosition.y = random.Next(range);

		return newPosition;
    }

}