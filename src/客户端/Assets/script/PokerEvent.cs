using UnityEngine;
using System.Collections;

public class PokerEvent : MonoBehaviour {
    public bool PokerSelectState;
	public Vector3 Curpos,Maxpos;
	// Use this for initialization
	void Start () {
        PokerSelectState = false;
		Curpos = gameObject.transform.position;
		Maxpos = new Vector3 (Curpos.x, Curpos.y + 0.4f, Curpos.z);
    }
	
	// Update is called once per frame
	void Update () {
       	 if (PokerSelectState)
			gameObject.transform.position = Maxpos;
		else
			gameObject.transform.position = Curpos;
    }

	void OnMouseDown()
    {
        PokerSelectState = !PokerSelectState;
		if (PokerSelectState) {
			GameObject.Find ("SelectCard").GetComponent<SelectCard> ().SetSelectCardPos_add(gameObject.transform.position.x);
		} else {
			GameObject.Find ("SelectCard").GetComponent<SelectCard> ().SetSelectCardPos_remove(gameObject.transform.position.x);
		}
    }

	public void SetPokerSelectState(bool b)
	{
		PokerSelectState = b;
	}

	public void SetPos(Vector3 pos)
	{
		Curpos = pos;
		Maxpos = new Vector3 (Curpos.x, Curpos.y + 0.4f, Curpos.z);
	}
}
