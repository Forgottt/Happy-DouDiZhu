#ifndef _SEND_PACKAGE_H_
#define _SEND_PACKAGE_H_

class SendPackage
{
	//容量
	int m_capacity;

	//长度
	int m_size;

	//数据
	char* m_data;

	//扩容
	void _add_mem(int size);

public:

	//构造
	SendPackage();

	//拷贝构造
	SendPackage(const SendPackage& that);

	//同类赋值
	SendPackage& operator = (const SendPackage& that);

	//析构
	~SendPackage();

	//清空
	void clear();

	//得到数据首地址
	void* data() const;

	//得到数据总长度
	int size() const;

	//添加基本数据类型
	void add_bool(bool v);
	void add_char(char v);
	void add_unsigned_char(unsigned char v);
	void add_wchar_t(wchar_t v);
	void add_short(short v);
	void add_unsigned_short(unsigned short v);
	void add_int(int v);
	void add_unsigned_int(unsigned int v);
	void add_long(long v);
	void add_unsigned_long(unsigned long v);
	void add_float(float v);
	void add_double(double v);
	void add_long_double(long double v);

	//添加字符串
	void add_string(const char* v);

	//添加字节数组
	void add_byte_array(const void* v, int c);
};

#endif