using UnityEngine;
using System.Collections;

public class Boom : MonoBehaviour {

	// Use this for initialization
	void Start () {
		Destroy (gameObject,0.98f);
	}
	
	// Update is called once per frame
	void Update () {
		if (gameObject.transform.localPosition.y > 0.0f)
			gameObject.transform.localPosition = Vector3.MoveTowards(gameObject.transform.localPosition, new Vector3(0, 0.0f, -2.0f), 5.0f*Time.deltaTime);
	}
}
