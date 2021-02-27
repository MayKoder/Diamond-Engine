using System;
using DiamondEngine;

public class CameraController : DiamondComponent
{
	public GameObject reference = null;

	public Vector3 offset;
	public GameObject target;
	public float smoothSpeed = 0;

	public void Update()
	{
		Vector3 desiredPosition = target.localPosition + offset;
		Vector3 smoothPosition = Vector3.Lerp(reference.localPosition, desiredPosition, smoothSpeed);
		reference.localPosition = smoothPosition;
	}

}