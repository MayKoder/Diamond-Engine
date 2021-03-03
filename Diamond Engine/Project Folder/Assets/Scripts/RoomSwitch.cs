using System;
using DiamondEngine;

public class RoomSwitch : DiamondComponent
{

	static int[] rooms = { 1076838722,
		1934547592,
		1406013733,
		1482507639};

	//public static int index = 0;
	static Random test = new Random();

    public void OnCollisionEnter()
    {
        Debug.Log("Ayo i've been called.");
		Debug.Log(rooms[test.Next(0, rooms.Length)].ToString());
		SceneManager.LoadScene(rooms[test.Next(0, rooms.Length)]);
	}

    public void Update()
	{
		//if (Input.GetKey(DEKeyCode.I) == KeyState.KEY_DOWN)
  //      {
			
		//}
	}

}
