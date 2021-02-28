using System;
using DiamondEngine;

public class CameraController : DiamondComponent
{
	public GameObject reference = null;
	public GameObject target;

	public float x_offset, y_offset, z_offset;
	public float smoothSpeed = 0;

	public float x1_limit, x2_limit, y1_limit, y2_limit;

	public void Update()
	{
		if (target.localPosition.x <= x1_limit && target.localPosition.x >= x2_limit && target.localPosition.y <= y1_limit && target.localPosition.y >= y2_limit)
		{
			Vector3 offset = new Vector3(x_offset, y_offset, z_offset);
			Vector3 desiredPosition = target.localPosition + offset;
			Vector3 smoothPosition = Vector3.Lerp(reference.localPosition, desiredPosition, smoothSpeed);
			reference.localPosition = smoothPosition;
		}
	}

}
