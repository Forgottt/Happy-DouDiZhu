#ifndef _SEND_PACKAGE_H_
#define _SEND_PACKAGE_H_

class SendPackage
{
	//����
	int m_capacity;

	//����
	int m_size;

	//����
	char* m_data;

	//����
	void _add_mem(int size);

public:

	//����
	SendPackage();

	//��������
	SendPackage(const SendPackage& that);

	//ͬ�ำֵ
	SendPackage& operator = (const SendPackage& that);

	//����
	~SendPackage();

	//���
	void clear();

	//�õ������׵�ַ
	void* data() const;

	//�õ������ܳ���
	int size() const;

	//��ӻ�����������
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

	//����ַ���
	void add_string(const char* v);

	//����ֽ�����
	void add_byte_array(const void* v, int c);
};

#endif