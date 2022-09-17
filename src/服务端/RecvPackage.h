#ifndef _RECV_PACKAGE_H_
#define _RECV_PACKAGE_H_

class RecvPackage
{
	//����
	int m_size;

	//λ��
	int m_position;

	//����
	char* m_data;

public:

	//����
	RecvPackage();

	//��������
	RecvPackage(const RecvPackage& that);

	//ͬ�ำֵ
	RecvPackage& operator = (const RecvPackage& that);

	//����
	~RecvPackage();

	//��������
	void set_data(const void* data, int size);

	//����λ��
	bool set_position(int position);

	//�õ�λ��
	int get_position() const;

	//�õ�����
	int size() const;

	//�õ�������������
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
	
	//�õ��ַ������ȣ������ƶ�λ��
	int get_string_length();

	//�õ��ַ���
	bool get_string(char* v);

	//�õ��ֽ����鳤�ȣ������ƶ�λ��
	int get_byte_array_length();

	//�õ��ֽ�����
	bool get_byte_array(void* v);
};

#endif