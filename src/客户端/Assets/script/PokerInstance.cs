using UnityEngine;
using System.Collections;

public class PokerInstance : MonoBehaviour {

	public static GameObject PokerCard;

    public string PokerName;
    public int PokerType;
	public Vector3 pos;
	void Start () {
		pos = new Vector3 (0, -2.0f, 0);
		//Instantiate(PokerCard,pos,gameObject.transform.rotation);
	}
}
