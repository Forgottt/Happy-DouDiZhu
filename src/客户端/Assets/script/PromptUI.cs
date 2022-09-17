using UnityEngine;
using System.Collections;
using UnityEngine.UI;

public class PromptUI : MonoBehaviour {

	public GameObject m_PromptUI;
	float m_time;
	void Start(){
		m_PromptUI.SetActive (false);
		m_time = 0.0f;
	}

	void Update () {
		if (m_time > 0.0f) {
			m_time -= Time.deltaTime;
		} 
		else 
		{
			m_PromptUI.SetActive (false);
		}
	}
	
	public void SetPromptUI(string s)
	{
		m_time = 2.0f;
		m_PromptUI.SetActive (true);
		m_PromptUI.GetComponent<Text>().text = s;
	}
}
