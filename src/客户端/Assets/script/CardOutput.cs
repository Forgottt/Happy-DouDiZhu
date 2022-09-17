using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class CardOutput : MonoBehaviour {

	public List<GameObject> CardOutputInst;
	float time;

	// Use this for initialization
	void Start () {
		time = 0.0f;
		CardOutputInst = new List<GameObject> ();
	}
	
	// Update is called once per frame
	void Update () {
		if (time > 0.0f) {
			time -= Time.deltaTime;
		} else {
			if(0!=CardOutputInst.Count)
			{
				for(int i = CardOutputInst.Count - 1;i>=0;--i)
				{
					Destroy(CardOutputInst[i]);
				}
			}
			CardOutputInst.Clear();
		}
	}

	public void SetCardOutputInst(List<int> objindex,List<GameObject> obj)
	{
		time = 8.0f; 
		int count = objindex.Count;

		if(0!=CardOutputInst.Count)
		{
			for(int i = CardOutputInst.Count - 1;i>=0;--i)
			{
				Destroy(CardOutputInst[i]);
			}
		}
		CardOutputInst.Clear();

		for (int i=0; i<count; ++i) 
		{
			Vector3 pos = new Vector3(gameObject.transform.position.x,gameObject.transform.position.y,gameObject.transform.position.z+1.0f+i*0.5f);
			GameObject temp = (GameObject)Instantiate(obj[objindex[i]],pos,gameObject.transform.rotation);
			CardOutputInst.Add(temp);
		}
		//禁用卡牌移动
		for(int i=0;i<CardOutputInst.Count;++i)
			CardOutputInst[i].GetComponent<BoxCollider>().enabled = false;
		ShowOutputCard (count);
	}

	//展示出牌
	void ShowOutputCard(int num)
	{
		//第一张牌的位置
		float posx = (num - 1) / 4.0f;
		
		for (int i=0; i<num; ++i) 
		{
			Vector3 pos = new Vector3 (posx, 0.0f, 0.0f);
			pos.x = pos.x - i * 0.5f;
			CardOutputInst[i].transform.position = pos;
		}
	}

	public void SetTime(float t)
	{
		time = t;
	}
}
