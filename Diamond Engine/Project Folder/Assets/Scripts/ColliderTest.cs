using System;
using DiamondEngine;

public class ColliderTest : DiamondComponent
{
	public void Update()
	{

	}

	public void OnCollisionEnter(GameObject collidedGameObject)
	{
		//Debug.Log("CS: Collided object: " + gameObject.tag + ", Collider: " + collidedGameObject.tag);
		//Debug.Log("Collided by tag: " + collidedGameObject.tag);
		Debug.Log("Collision detected with: " + collidedGameObject.tag);
	}

	public void OnTriggerEnter(GameObject triggeredGameObject)
	{
		//Debug.Log("CS: Collided object: " + gameObject.tag + ", Collider: " + triggeredGameObject.tag);
		//if (triggeredGameObject.CompareTag("Bullet"))
		//{
			Debug.Log("Trigger detecte dwith: " + triggeredGameObject.tag);
		//}

		//Debug.Log("Triggered by tag: " + triggeredGameObject.tag);
	}
}