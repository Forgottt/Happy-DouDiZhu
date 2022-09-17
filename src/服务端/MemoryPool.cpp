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
	//在表中查找符合要求的内存节点
	for (int i = 0; i < (int)m_Nodes.size(); ++i)
	{
		//找到了就返回这个内存节点存储的内存首地址
		if (m_Nodes[i].use == _NO_USE && m_Nodes[i].size >= size)
		{
			m_Nodes[i].use = _IN_USE;
			return m_Nodes[i].data;
		}
	}

	//找不到就创建新的节点并记录到表中
	NODE node;
	node.use = _IN_USE;
	node.size = size ;
	node.data = malloc(node.size);
	m_Nodes.push_back(node);

	//返回新节点的内存首地址
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