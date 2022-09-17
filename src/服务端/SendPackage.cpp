#include "SendPackage.h"
#include <malloc.h>
#include <string.h>

void SendPackage::_add_mem(int size)
{
	if (m_capacity - m_size < size)
	{
		m_capacity = m_capacity * 2 + size;
		char* p = (char*)malloc(m_capacity);
		for (int i = m_size - 1; i >= 0; --i)
			p[i] = m_data[i];
		free(m_data);
		m_data = p;
	}
}

SendPackage::SendPackage()
:
m_capacity(32),
m_size(0),
m_data((char*)malloc(m_capacity))
{}

SendPackage::SendPackage(const SendPackage& that)
:
m_capacity(that.m_capacity),
m_size(that.m_size),
m_data((char*)malloc(m_capacity))
{
	for (int i = m_size - 1; i >= 0; --i)
		m_data[i] = that.m_data[i];
}

SendPackage& SendPackage::operator = (const SendPackage& that)
{
	if (this != &that)
	{
		free(m_data);

		m_capacity = that.m_capacity;
		m_size = that.m_size;
		m_data = (char*)malloc(m_capacity);
		for (int i = m_size - 1; i >= 0; --i)
			m_data[i] = that.m_data[i];
	}

	return *this;
}

SendPackage::~SendPackage()
{
	free(m_data);
}

void SendPackage::clear()
{
	m_size = 0;
}

void* SendPackage::data() const
{
	return m_data;
}

int SendPackage::size() const
{
	return m_size;
}

void SendPackage::add_bool(bool v)
{
	_add_mem(sizeof(v));
	*((bool*)(m_data + m_size)) = v;
	m_size += sizeof(v);
}
void SendPackage::add_char(char v)
{
	_add_mem(sizeof(v));
	*((char*)(m_data + m_size)) = v;
	m_size += sizeof(v);
}
void SendPackage::add_unsigned_char(unsigned char v)
{
	_add_mem(sizeof(v));
	*((unsigned char*)(m_data + m_size)) = v;
	m_size += sizeof(v);
}
void SendPackage::add_wchar_t(wchar_t v)
{
	_add_mem(sizeof(v));
	*((wchar_t*)(m_data + m_size)) = v;
	m_size += sizeof(v);
}
void SendPackage::add_short(short v)
{
	_add_mem(sizeof(v));
	*((short*)(m_data + m_size)) = v;
	m_size += sizeof(v);
}
void SendPackage::add_unsigned_short(unsigned short v)
{
	_add_mem(sizeof(v));
	*((unsigned short*)(m_data + m_size)) = v;
	m_size += sizeof(v);
}
void SendPackage::add_int(int v)
{
	_add_mem(sizeof(v));
	*((int*)(m_data + m_size)) = v;
	m_size += sizeof(v);
}
void SendPackage::add_unsigned_int(unsigned int v)
{
	_add_mem(sizeof(v));
	*((unsigned int*)(m_data + m_size)) = v;
	m_size += sizeof(v);
}
void SendPackage::add_long(long v)
{
	_add_mem(sizeof(v));
	*((long*)(m_data + m_size)) = v;
	m_size += sizeof(v);
}
void SendPackage::add_unsigned_long(unsigned long v)
{
	_add_mem(sizeof(v));
	*((unsigned long*)(m_data + m_size)) = v;
	m_size += sizeof(v);
}
void SendPackage::add_float(float v)
{
	_add_mem(sizeof(v));
	*((float*)(m_data + m_size)) = v;
	m_size += sizeof(v);
}
void SendPackage::add_double(double v)
{
	_add_mem(sizeof(v));
	*((double*)(m_data + m_size)) = v;
	m_size += sizeof(v);
}
void SendPackage::add_long_double(long double v)
{
	_add_mem(sizeof(v));
	*((long double*)(m_data + m_size)) = v;
	m_size += sizeof(v);
}

void SendPackage::add_string(const char* v)
{
	//得到字符串长度
	int sl = (int)strlen(v);

	//扩容判断
	_add_mem(sizeof(int) + sl);

	//加入长度
	*((int*)(m_data + m_size)) = sl;
	m_size += sizeof(int);

	//加入字符串
	for (int i = sl - 1; i >= 0; --i)
		m_data[m_size + i] = v[i];

	m_size += sl;
}

void SendPackage::add_byte_array(const void* v, int c)
{
	//扩容判断
	_add_mem(sizeof(int) + c);

	//加入长度
	*((int*)(m_data + m_size)) = c;
	m_size += sizeof(int);

	//加入字节数组
	const char* p = (const char*)v;
	for (int i = c - 1; i >= 0; --i)
		m_data[m_size + i] = p[i];
	m_size += c;
}