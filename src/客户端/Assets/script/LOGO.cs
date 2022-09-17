using UnityEngine;
using System.Collections;

public class LOGO : MonoBehaviour {

	// Use this for initialization
	void Start () {
		Invoke ("LoadLevel", 2.0f);
	}
	
	// Update is called once per frame
	void Update () {
	
	}

	void LoadLevel()
	{
		Application.LoadLevel (1);
	}
}
