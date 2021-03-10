using System;
using DiamondEngine;

public class CameraController : DiamondComponent
{
	public GameObject reference = null;
	public GameObject target;

	public float x, y, z;
	public float smoothSpeed = 0;

	public void Update()
	{
		Vector3 offset = new Vector3(x, y, z);
		Vector3 desiredPosition = target.transform.localPosition + offset;
		Vector3 smoothPosition = Vector3.Lerp(reference.transform.localPosition, desiredPosition, smoothSpeed);
		reference.transform.localPosition = smoothPosition;
	}
}