#ifndef _RECV_PACKAGE_H_
#define _RECV_PACKAGE_H_

class RecvPackage
{
	//长度
	int m_size;

	//位置
	int m_position;

	//数据
	char* m_data;

public:

	//构造
	RecvPackage();

	//拷贝构造
	RecvPackage(const RecvPackage& that);

	//同类赋值
	RecvPackage& operator = (const RecvPackage& that);

	//析构
	~RecvPackage();

	//设置数据
	void set_data(const void* data, int size);

	//设置位置
	bool set_position(int position);

	//得到位置
	int get_position() const;

	//得到长度
	int size() const;

	//得到基本数据类型
	bool get_bool(bool* v);
	bool get_char(char* v);
	bool get_unsigned_char(unsigned char* v);
	bool get_wchar_t(wchar_t* v);
	bool get_short(short* v);
	bool get_unsigned_short(unsigned short* v);
	bool get_int(int* v);
	bool get_unsigned_int(unsigned int* v);
	bool get_long(long* v);
	bool get_unsigned_long(unsigned long* v);
	bool get_float(float* v);
	bool get_double(double* v);
	bool get_long_double(long double* v);
	
	//得到字符串长度，不会移动位置
	int get_string_length();

	//得到字符串
	bool get_string(char* v);

	//得到字节数组长度，不会移动位置
	int get_byte_array_length();

	//得到字节数组
	bool get_byte_array(void* v);
};

#endif