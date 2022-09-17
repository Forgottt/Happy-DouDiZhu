using UnityEngine;
using System.Collections;

public class Plane : MonoBehaviour {

	// Use this for initialization
	void Start () {
		Destroy (gameObject,0.98f);
	}
	
	// Update is called once per frame
	void Update () {
		gameObject.transform.localPosition = Vector3.MoveTowards(gameObject.transform.localPosition, new Vector3(-3.0f, 0, -2.0f), 5.0f*Time.deltaTime);
		if (gameObject.transform.localPosition.x <= -2.9f) {
			Destroy (gameObject);
		}
	}
}
