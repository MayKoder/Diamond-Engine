using System;
using DiamondEngine;

public class CameraController : DiamondComponent
{
	public GameObject reference = null;

	public float x, y, z;
	public float rx, ry, rz;
	public GameObject target;
	public float smoothSpeed = 0;
	private Vector3 offset;

	public void Update()
	{
		offset = new Vector3(x, y, z);
		reference.localRotation = Quaternion.ToQuaternion(rx,ry,rz);
		Vector3 desiredPosition = target.localPosition + offset;
		Vector3 smoothPosition = Vector3.Lerp(reference.localPosition, desiredPosition, smoothSpeed);
		reference.localPosition = smoothPosition;
	}

}