using System;
using DiamondEngine;


public class Enemy : DiamondComponent
   {
	public GameObject player;
	public GameObject shootPoint;

	public float wanderSpeed;
	public float runningSpeed;
	public int range;
	public float damage;
	public float bullet_speed;
	protected int shotTimes = 0;

	protected float timeBewteenShots = 5.0f;
	protected float timePassed = 0.0f;

	public float wanderTime = 5.0f;
	public float idleTime = 5.0f;
	protected Vector3 targetPosition = null;
	protected float stoppingDistance = 1.0f;

	protected STATES currentState = STATES.WANDER;

	protected enum STATES
	{
		IDLE,
		RUN,
		WANDER,
		SHOOT,
		HIT,
		DIE
	}

	public virtual bool Shoot()
	{
		InternalCalls.CreateBullet(shootPoint.transform.globalPosition, shootPoint.transform.globalRotation, shootPoint.transform.globalScale);
		timePassed = 0.0f;
		shotTimes++;

		return true;
	}

	public virtual bool TakeDamage()
	{
		return true;
	}

	public virtual Vector3 CalculateNewPosition()
	{
		Vector3 newPosition = new Vector3(0, 0, 0);
		Random random = new Random();

		newPosition.x = random.Next(range);
		newPosition.y = gameObject.transform.localPosition.y;
		newPosition.z = random.Next(range);

		return newPosition;
	}

}
