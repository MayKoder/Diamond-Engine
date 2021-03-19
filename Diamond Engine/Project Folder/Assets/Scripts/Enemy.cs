using System;
using DiamondEngine;


public class Enemy : DiamondComponent
{
	public GameObject player;
	public GameObject shootPoint;

	public float wanderSpeed = 3.5f;
	public float runningSpeed = 12.5f;
	public float range = 20.0f;
	public float damage = 20.0f;
	public float bulletSpeed = 10.0f;
	protected int shotTimes = 0;

	protected float timeBewteenShots = 0.5f;
	protected float timePassed = 0.0f;

	public float idleTime = 5.0f;
	protected Vector3 targetPosition = null;
	protected float stoppingDistance = 1.0f;
	public float wanderRange = 5.0f;
	public float runningRange = 15.0f;

	public float slerpSpeed = 1000.5f;
	//private float timeCount = 0.0f;

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
		InternalCalls.CreateBullet(shootPoint.transform.globalPosition, shootPoint.transform.globalRotation, new Vector3(1.0f, 1.0f, 1.0f));
		timePassed = 0.0f;
		shotTimes++;

		return true;
	}

	public virtual bool TakeDamage()
	{
		return true;
	}

	public virtual Vector3 CalculateNewPosition(float maxPos)
	{
		Vector3 newPosition = new Vector3(0, 0, 0);
		Random random = new Random();

		newPosition.x = random.Next((int)maxPos);
		newPosition.y = gameObject.transform.localPosition.y;
		newPosition.z = random.Next((int)maxPos);

		return newPosition;
	}

	public void MoveToPosition(Vector3 positionToReach, float speed)
	{
		Vector3 direction = positionToReach - gameObject.transform.localPosition;

		gameObject.transform.localPosition += direction.normalized * speed * Time.deltaTime;
	}

	public void LookAt(Vector3 pointToLook)
	{
		//Vector3 direction = pointToLook - gameObject.transform.globalPosition;

		//direction = direction.normalized;


		//gameObject.transform.localRotation = new Quaternion(0, Mathf.LerpAngle(gameObject.transform.localRotation.y, angle, 0.01f), 0);


		Vector3 direction = pointToLook - gameObject.transform.globalPosition;
		direction = direction.normalized;
		float angle = (float)Math.Atan2(direction.x, direction.z);

		//Debug.Log("Desired angle: " + (angle * Mathf.Rad2Deg).ToString());

		if (Math.Abs(angle * Mathf.Rad2Deg) < 1.0f)
			return;

		Quaternion dir = Quaternion.RotateAroundAxis(Vector3.up, angle);

		float rotationSpeed = Time.deltaTime * slerpSpeed * 1000.0f;
		//Debug.Log("Rotation speed: " + rotationSpeed.ToString());

		Quaternion desiredRotation = Quaternion.Slerp(gameObject.transform.localRotation, dir, rotationSpeed);

		gameObject.transform.localRotation = desiredRotation;

	}

	public bool InRange(Vector3 point, float givenRange)
    {
		return Mathf.Distance(gameObject.transform.globalPosition, point) < givenRange;
	}
}
