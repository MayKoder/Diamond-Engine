using System;
using DiamondEngine;

public class AnimationTest : DiamondComponent
{
	public GameObject reference = null;
	public string idle_animation = "Idle";
	public string run_animation = "Run";
	public string shoot_animation = "Shoot";

	public void Update()
	{
		if(Input.GetKey(DEKeyCode.F1) == KeyState.KEY_DOWN)
        {
			Animator.Play(reference, idle_animation);
        }
		else if (Input.GetKey(DEKeyCode.F2) == KeyState.KEY_DOWN)
		{
			Animator.Play(reference, run_animation);
		}
		else if (Input.GetKey(DEKeyCode.F3) == KeyState.KEY_DOWN)
		{
			Animator.Play(reference, shoot_animation);
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