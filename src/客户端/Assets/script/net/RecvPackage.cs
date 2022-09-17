using System;
using System.Text;

public class RecvPackage
{
	//位置
	int m_position;
	
	//数据
	byte[] m_data;

	//辅助
	byte[] m_help;

	//编码
	Encoding m_gb2312;
		
	//构造
	public RecvPackage()
	{
		m_position = -1;
		m_data = null;
		m_help = new byte[sizeof(double)];
		m_gb2312 = Encoding.GetEncoding("gb2312");
	}
	
	//设置数据
	public void set_data(byte[] data, int begin, int size)
	{
		m_position = 0;
		m_data = new byte[size];
		for (int i = 0; i < size; ++i)
			m_data [i] = data [begin + i];
	}
	
	//设置位置
	public bool set_position(int position)
	{
		if (position < 0 || position >= m_data.Length)
			return false;
		
		m_position = position;
		
		return true;
	}
	
	//得到位置
	public int get_position()
	{
		return m_position;
	}
	
	//得到长度
	public int size()
	{
		return m_data.Length;
	}

	//得到基本数据类型
	public bool get_bool(ref bool v)
	{
		if (m_data.Length - m_position < sizeof(bool))
			return false;
		if (BitConverter.IsLittleEndian)
			v = BitConverter.ToBoolean (m_data, m_position);
		else
		{
			int end = m_position + sizeof(bool) - 1;
			for (int i = 0; i < sizeof(bool); ++i)
				m_help[i] = m_data[end--];
			v = BitConverter.ToBoolean (m_help, 0);
		}
		m_position += sizeof(bool);
		return true;
	}
	public bool get_char(ref char v)
	{
		if (m_data.Length - m_position < sizeof(char))
			return false;
		if (BitConverter.IsLittleEndian)
			v = BitConverter.ToChar (m_data, m_position);
		else
		{
			int end = m_position + sizeof(char) - 1;
			for (int i = 0; i < sizeof(char); ++i)
				m_help[i] = m_data[end--];
			v = BitConverter.ToChar (m_help, 0);
		}
		m_position += sizeof(char);
		return true;
	}
	public bool get_sbyte(ref sbyte v)
	{
		if (m_data.Length - m_position < sizeof(sbyte))
			return false;
		v = (sbyte)m_data [m_position];
		m_position += sizeof(sbyte);
		return true;
	}
	public bool get_byte(ref byte v)
	{
		if (m_data.Length - m_position < sizeof(byte))
			return false;
		v = m_data [m_position];
		m_position += sizeof(byte);
		return true;
	}
	public bool get_short(ref short v)
	{
		if (m_data.Length - m_position < sizeof(short))
			return false;
		if (BitConverter.IsLittleEndian)
			v = BitConverter.ToInt16 (m_data, m_position);
		else
		{
			int end = m_position + sizeof(short) - 1;
			for (int i = 0; i < sizeof(short); ++i)
				m_help[i] = m_data[end--];
			v = BitConverter.ToInt16 (m_help, 0);
		}
		m_position += sizeof(short);
		return true;
	}
	public bool get_ushort(ref ushort v)
	{
		if (m_data.Length - m_position < sizeof(ushort))
			return false;
		if (BitConverter.IsLittleEndian)
			v = BitConverter.ToUInt16 (m_data, m_position);
		else
		{
			int end = m_position + sizeof(ushort) - 1;
			for (int i = 0; i < sizeof(ushort); ++i)
				m_help[i] = m_data[end--];
			v = BitConverter.ToUInt16 (m_help, 0);
		}
		m_position += sizeof(ushort);
		return true;
	}
	public bool get_int(ref int v)
	{
		if (m_data.Length - m_position < sizeof(int))
			return false;
		if (BitConverter.IsLittleEndian)
			v = BitConverter.ToInt32 (m_data, m_position);
		else
		{
			int end = m_position + sizeof(int) - 1;
			for (int i = 0; i < sizeof(int); ++i)
				m_help[i] = m_data[end--];
			v = BitConverter.ToInt32 (m_help, 0);
		}
		m_position += sizeof(int);
		return true;
	}
	public bool get_uint(ref uint v)
	{
		if (m_data.Length - m_position < sizeof(uint))
			return false;
		if (BitConverter.IsLittleEndian)
			v = BitConverter.ToUInt32 (m_data, m_position);
		else
		{
			int end = m_position + sizeof(uint) - 1;
			for (int i = 0; i < sizeof(uint); ++i)
				m_help[i] = m_data[end--];
			v = BitConverter.ToUInt32 (m_help, 0);
		}
		m_position += sizeof(uint);
		return true;
	}
	public bool get_long(ref long v)
	{
		if (m_data.Length - m_position < sizeof(long))
			return false;
		if (BitConverter.IsLittleEndian)
			v = BitConverter.ToInt64 (m_data, m_position);
		else
		{
			int end = m_position + sizeof(long) - 1;
			for (int i = 0; i < sizeof(long); ++i)
				m_help[i] = m_data[end--];
			v = BitConverter.ToInt64 (m_help, 0);
		}
		m_position += sizeof(long);
		return true;
	}
	public bool get_ulong(ref ulong v)
	{
		if (m_data.Length - m_position < sizeof(ulong))
			return false;
		if (BitConverter.IsLittleEndian)
			v = BitConverter.ToUInt64 (m_data, m_position);
		else
		{
			int end = m_position + sizeof(ulong) - 1;
			for (int i = 0; i < sizeof(ulong); ++i)
				m_help[i] = m_data[end--];
			v = BitConverter.ToUInt64 (m_help, 0);
		}
		m_position += sizeof(ulong);
		return true;
	}
	public bool get_float(ref float v)
	{
		if (m_data.Length - m_position < sizeof(float))
			return false;
		if (BitConverter.IsLittleEndian)
			v = BitConverter.ToSingle (m_data, m_position);
		else
		{
			int end = m_position + sizeof(float) - 1;
			for (int i = 0; i < sizeof(float); ++i)
				m_help[i] = m_data[end--];
			v = BitConverter.ToSingle (m_help, 0);
		}
		m_position += sizeof(float);
		return true;
	}
	public bool get_double(ref double v)
	{
		if (m_data.Length - m_position < sizeof(double))
			return false;
		if (BitConverter.IsLittleEndian)
			v = BitConverter.ToDouble (m_data, m_position);
		else
		{
			int end = m_position + sizeof(double) - 1;
			for (int i = 0; i < sizeof(double); ++i)
				m_help[i] = m_data[end--];
			v = BitConverter.ToDouble (m_help, 0);
		}
		m_position += sizeof(double);
		return true;
	}
	
	//得到字符串
	public bool get_string(ref string v)
	{
		//得到字符串长度
		int sl = -1;
		if (!get_int (ref sl))
			return false;

		//判断长度是否合法
		if (m_data.Length - m_position < sl)
			return false;

		//得到字符串
		v = m_gb2312.GetString (m_data, m_position, sl);
		m_position += sl;
		return true;
	}

	//得到字节数组
	public bool get_byte_array(ref byte[] v)
	{
		//得到字节数组长度
		int bal = -1;
		if (!get_int (ref bal))
			return false;

		//判断长度是否合法
		if (m_data.Length - m_position < bal)
			return false;

		//得到字节数组
		v = new byte[bal];
		for (int i = 0; i < bal; ++i)
			v [i] = m_data [m_position + i];
		m_position += bal;
		return true;
	}
}
