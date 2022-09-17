#include "MemoryPool.h"

#define _NO_USE 0
#define _IN_USE 1

MemoryPool::MemoryPool(const MemoryPool& that)
{}

MemoryPool& MemoryPool::operator = (const MemoryPool& that)
{
	return *this;
}

MemoryPool::MemoryPool()
{}

MemoryPool::~MemoryPool()
{
	for (int i = 0; i < (int)m_Nodes.size(); ++i)
		free(m_Nodes[i].data);
}

void* MemoryPool::Allocate(int size)
{
	//�ڱ��в��ҷ���Ҫ����ڴ�ڵ�
	for (int i = 0; i < (int)m_Nodes.size(); ++i)
	{
		//�ҵ��˾ͷ�������ڴ�ڵ�洢���ڴ��׵�ַ
		if (m_Nodes[i].use == _NO_USE && m_Nodes[i].size >= size)
		{
			m_Nodes[i].use = _IN_USE;
			return m_Nodes[i].data;
		}
	}

	//�Ҳ����ʹ����µĽڵ㲢��¼������
	NODE node;
	node.use = _IN_USE;
	node.size = size ;
	node.data = malloc(node.size);
	m_Nodes.push_back(node);

	//�����½ڵ���ڴ��׵�ַ
	return m_Nodes.back().data;
}

void MemoryPool::Free(void* data)
{	
	for (int i = 0; i < (int)m_Nodes.size(); ++i)
	{
		if (m_Nodes[i].data == data && m_Nodes[i].use == _IN_USE)
		{
			m_Nodes[i].use = _NO_USE;
			break;
		}
	}
}