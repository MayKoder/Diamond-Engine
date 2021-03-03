using System;
using DiamondEngine;

public class AnimationTest : DiamondComponent
{
	public GameObject reference = null;

	public void Update()
	{
		if(Input.GetKey(DEKeyCode.F1) == KeyState.KEY_DOWN)
        {
			Animator.Play(reference, "Idle");
        }
		else if (Input.GetKey(DEKeyCode.F2) == KeyState.KEY_DOWN)
		{
			Animator.Play(reference, "Run");
		}
		else if (Input.GetKey(DEKeyCode.F3) == KeyState.KEY_DOWN)
		{
			Animator.Play(reference, "Shoot");
		}
		else if(Input.GetKey(DEKeyCode.F4) == KeyState.KEY_DOWN)
        {
			Animator.Pause(reference);
        }
		else if (Input.GetKey(DEKeyCode.F5) == KeyState.KEY_DOWN)
		{
			Animator.Resume(reference);
		}
	}
}