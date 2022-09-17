#include "RecvPackage.h"
#include <malloc.h>

RecvPackage::RecvPackage()
:
m_size(0),
m_position(-1),
m_data(0)
{}

RecvPackage::RecvPackage(const RecvPackage& that)
{
	if (0 == that.m_size)
	{
		m_size = 0;
		m_position = -1;
		m_data = 0;
	}
	else
	{
		m_size = that.m_size;
		m_position = that.m_position;
		m_data = (char*)malloc(m_size);
		for (int i = m_size - 1; i >= 0; --i)
			m_data[i] = that.m_data[i];
	}
}

RecvPackage& RecvPackage::operator = (const RecvPackage& that)
{
	if (this != &that)
	{
		if (0 != m_data)
			free(m_data);

		if (0 == that.m_size)
		{
			m_size = 0;
			m_position = -1;
			m_data = 0;
		}
		else
		{
			m_size = that.m_size;
			m_position = that.m_position;
			m_data = (char*)malloc(m_size);
			for (int i = m_size - 1; i >= 0; --i)
				m_data[i] = that.m_data[i];
		}
	}

	return *this;
}

RecvPackage::~RecvPackage()
{
	if (0 != m_data)
		free(m_data);
}

void RecvPackage::set_data(const void* data, int size)
{
	if (0 != m_data)
		free(m_data);

	m_size = size;
	m_position = 0;
	m_data = (char*)malloc(m_size);
	for (int i = m_size - 1; i >= 0; --i)
		m_data[i] = ((const char*)data)[i];
}

bool RecvPackage::set_position(int position)
{
	if (position < 0 || position >= m_size)
		return false;

	m_position = position;

	return true;
}

int RecvPackage::get_position() const
{
	return m_position;
}

int RecvPackage::size() const
{
	return m_size;
}

bool RecvPackage::get_bool(bool* v)
{
	if (m_size - m_position < sizeof(*v))
		return false;
	*v = *((bool*)(m_data + m_position));
	m_position += sizeof(*v);
	return true;
}
bool RecvPackage::get_char(char* v)
{
	if (m_size - m_position < sizeof(*v))
		return false;
	*v = *((char*)(m_data + m_position));
	m_position += sizeof(*v);
	return true;
}
bool RecvPackage::get_unsigned_char(unsigned char* v)
{
	if (m_size - m_position < sizeof(*v))
		return false;
	*v = *((unsigned char*)(m_data + m_position));
	m_position += sizeof(*v);
	return true;
}
bool RecvPackage::get_wchar_t(wchar_t* v)
{
	if (m_size - m_position < sizeof(*v))
		return false;
	*v = *((wchar_t*)(m_data + m_position));
	m_position += sizeof(*v);
	return true;
}
bool RecvPackage::get_short(short* v)
{
	if (m_size - m_position < sizeof(*v))
		return false;
	*v = *((short*)(m_data + m_position));
	m_position += sizeof(*v);
	return true;
}
bool RecvPackage::get_unsigned_short(unsigned short* v)
{
	if (m_size - m_position < sizeof(*v))
		return false;
	*v = *((unsigned short*)(m_data + m_position));
	m_position += sizeof(*v);
	return true;
}
bool RecvPackage::get_int(int* v)
{
	if (m_size - m_position < sizeof(*v))
		return false;
	*v = *((int*)(m_data + m_position));
	m_position += sizeof(*v);
	return true;
}
bool RecvPackage::get_unsigned_int(unsigned int* v)
{
	if (m_size - m_position < sizeof(*v))
		return false;
	*v = *((unsigned int*)(m_data + m_position));
	m_position += sizeof(*v);
	return true;
}
bool RecvPackage::get_long(long* v)
{
	if (m_size - m_position < sizeof(*v))
		return false;
	*v = *((long*)(m_data + m_position));
	m_position += sizeof(*v);
	return true;
}
bool RecvPackage::get_unsigned_long(unsigned long* v)
{
	if (m_size - m_position < sizeof(*v))
		return false;
	*v = *((unsigned long*)(m_data + m_position));
	m_position += sizeof(*v);
	return true;
}
bool RecvPackage::get_float(float* v)
{
	if (m_size - m_position < sizeof(*v))
		return false;
	*v = *((float*)(m_data + m_position));
	m_position += sizeof(*v);
	return true;
}
bool RecvPackage::get_double(double* v)
{
	if (m_size - m_position < sizeof(*v))
		return false;
	*v = *((double*)(m_data + m_position));
	m_position += sizeof(*v);
	return true;
}
bool RecvPackage::get_long_double(long double* v)
{
	if (m_size - m_position < sizeof(*v))
		return false;

	*v = *((long double*)(m_data + m_position));
	m_position += sizeof(*v);

	return true;
}

int RecvPackage::get_string_length()
{
	int sl = -1;

	if (m_size - m_position >= sizeof(int))
		sl = *((int*)(m_data + m_position));

	return sl;
}

bool RecvPackage::get_string(char* v)
{
	//先得到字符串的长度
	int sl = 0;
	if (!get_int(&sl))
		return false;

	//判断当前余下的数据字节是否足够
	if (m_size - m_position < sl)
		return false;

	//得到字符串
	for (int i = 0; i < sl; ++i)
		v[i] = m_data[m_position + i];

	//设置字符串结束符
	v[sl] = 0;

	//读取位置递增
	m_position += sl;
	
	return true;
}

int RecvPackage::get_byte_array_length()
{
	int bal = -1;

	if (m_size - m_position >= sizeof(int))
		bal = *((int*)(m_data + m_position));

	return bal;
}

bool RecvPackage::get_byte_array(void* v)
{
	//先得到字节数组的长度
	int bal = 0;
	if (!get_int(&bal))
		return false;

	//判断当前余下的数据字节是否足够
	if (m_size - m_position < bal)
		return false;

	//得到字节数组
	char* p = (char*)v;
	for (int i = 0; i < bal; ++i)
		p[i] = m_data[m_position + i];

	//读取位置递增
	m_position += bal;
	
	return true;
}