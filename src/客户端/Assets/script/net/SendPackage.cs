using System;
using System.Text;

//System名字空间中有类BitConverter，其作用是可以让我们
//在基本数据类型和byte数组直接进行转换，主要用到3个函数
//GetBytes：基本数据类型转换为byte数组
//ToXXX：byte数组转换为基本数据类型
//IsLittleEndian：是否为小端CPU存储

//System.Text名字空间中有类Encoding，其作用是可以让我们
//自由选择字符编码方式，然后可以通过这种编码来在byte数组和
//字符串之间进行转换，主要用到3个函数
//GetEncoding：得到指定名称的编码
//GetBytes：字符串转换为byte数组
//GetString：byte数组转换为字符串

public class SendPackage
{
	//长度
	int m_size;
	
	//数据
	byte[] m_data;

	//编码
	Encoding m_gb2312;
	
	//扩容
	void _add_mem(int size)
	{
		//扩容
		if (m_data.Length - m_size < size)
		{
			byte[] t = new byte[m_data.Length * 2 + size];
			for (int i = 0; i < m_size; ++i)
				t[i] = m_data[i];
			m_data = t;
		}
	}

	//字节数组拷贝
	//从src数组的src_begin位置开始拷贝size个字节
	//到dst数组的dst_begin位置，注意拷贝的时候一定
	//保证我们放入的数据是按照小端存储
	public static bool ByteCopy(byte[] dst,
	                            int dst_begin,
	                            byte[] src,
	                            int src_begin,
	                            int size)
	{
		if (dst.Length - dst_begin < size)
			return false;

		//小端拷贝
		if (BitConverter.IsLittleEndian)
		{
			for (int i = 0; i < size; ++i)
				dst [dst_begin++] = src [src_begin++];
		}
		//大端拷贝
		else 
		{
			int src_end = src_begin + size - 1;
			for (int i = 0; i < size; ++i)
				dst [dst_begin++] = src [src_end--];
		}

		return true;
	}
		
	//构造
	public SendPackage()
	{
		m_size = 0;
		m_data = new byte[32];
		m_gb2312 = Encoding.GetEncoding("gb2312");
	}
	
	//清空
	public void clear()
	{
		m_size = 0;
	}
	
	//得到数据首地址
	public byte[] data()
	{
		return m_data;
	}
	
	//得到数据总长度
	public int size()
	{
		return m_size;
	}
	
	//添加基本数据类型
	public void add_bool(bool v)
	{
		_add_mem (sizeof(bool));
		ByteCopy (
			m_data,
			m_size,
			BitConverter.GetBytes (v),
			0,
			sizeof(bool));
		m_size += sizeof(bool);
	}
	public void add_char(char v)
	{
		_add_mem (sizeof(char));
		ByteCopy (
			m_data,
			m_size,
			BitConverter.GetBytes (v),
			0,
			sizeof(char));
		m_size += sizeof(char);
	}
	public void add_sbyte(sbyte v)
	{
		_add_mem (sizeof(sbyte));
		ByteCopy (
			m_data,
			m_size,
			BitConverter.GetBytes (v),
			0,
			sizeof(sbyte));
		m_size += sizeof(sbyte);
	}
	public void add_byte(byte v)
	{
		_add_mem (sizeof(byte));
		ByteCopy (
			m_data,
			m_size,
			BitConverter.GetBytes (v),
			0,
			sizeof(byte));
		m_size += sizeof(byte);
	}
	public void add_short(short v)
	{
		_add_mem (sizeof(short));
		ByteCopy (
			m_data,
			m_size,
			BitConverter.GetBytes (v),
			0,
			sizeof(short));
		m_size += sizeof(short);
	}
	public void add_ushort(ushort v)
	{
		_add_mem (sizeof(ushort));
		ByteCopy (
			m_data,
			m_size,
			BitConverter.GetBytes (v),
			0,
			sizeof(ushort));
		m_size += sizeof(ushort);
	}
	public void add_int(int v)
	{
		_add_mem (sizeof(int));
		ByteCopy (
			m_data,
			m_size,
			BitConverter.GetBytes (v),
			0,
			sizeof(int));
		m_size += sizeof(int);
	}
	public void add_uint(uint v)
	{
		_add_mem (sizeof(uint));
		ByteCopy (
			m_data,
			m_size,
			BitConverter.GetBytes (v),
			0,
			sizeof(uint));
		m_size += sizeof(uint);
	}
	public void add_long(long v)
	{
		_add_mem (sizeof(long));
		ByteCopy (
			m_data,
			m_size,
			BitConverter.GetBytes (v),
			0,
			sizeof(long));
		m_size += sizeof(long);
	}
	public void add_ulong(ulong v)
	{
		_add_mem (sizeof(ulong));
		ByteCopy (
			m_data,
			m_size,
			BitConverter.GetBytes (v),
			0,
			sizeof(ulong));
		m_size += sizeof(ulong);
	}
	public void add_float(float v)
	{
		_add_mem (sizeof(float));
		ByteCopy (
			m_data,
			m_size,
			BitConverter.GetBytes (v),
			0,
			sizeof(float));
		m_size += sizeof(float);
	}
	public void add_double(double v)
	{
		_add_mem (sizeof(double));
		ByteCopy (
			m_data,
			m_size,
			BitConverter.GetBytes (v),
			0,
			sizeof(double));
		m_size += sizeof(double);
	}

	//添加字符串
	public void add_string(string v)
	{
		//得到字符串转换的字节编码
		byte[] b = m_gb2312.GetBytes (v);

		//添加字符串长度
		add_int (b.Length);

		//添加字符串
		_add_mem (b.Length);
		for (int i = 0; i < b.Length; ++i)
			m_data [m_size + i] = b [i];
		m_size += b.Length;
	}

	//添加字节数组
	public void add_byte_array(byte[] v, int begin, int size)
	{
		//添加字节数组长度
		add_int(size);

		//添加字节数组
		_add_mem (size);
		for (int i = 0; i < size; ++i)
			m_data [m_size + i] = v [begin + i];
		m_size += size;
	}
}


















