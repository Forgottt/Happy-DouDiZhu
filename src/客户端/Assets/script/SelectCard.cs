using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class SelectCard : MonoBehaviour {

	public List<float> SelectCardPos;//选牌位置（用于转换为索引）

	public void SetSelectCardPos_add(float pos)
	{
		SelectCardPos.Add (pos);
	}
	public void SetSelectCardPos_remove(float pos)
	{
		SelectCardPos.Remove(pos);
	}

	public void ClearList()
	{
		SelectCardPos.Clear ();
	}
}
