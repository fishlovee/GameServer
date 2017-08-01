
#pragma once

#include "Stdafx.h"


//服务单元
namespace FangKaHttpUnits 
{
	template<typename T>
	inline std::wstring toString (T p)
	{
		std::wostringstream stream;
		stream << p;
		return stream.str();
	}
	template<typename T>
	inline T parseValue( const std::wstring& _value )
	{
		std::wistringstream stream(_value);
		T result;
		stream >> result;
		if (stream.fail())
			return T();
		else
		{
			int item = stream.get();
			while (item != -1)
			{
				if (item != ' ' && item != '\t')
					return T();
				item = stream.get();
			}
		}
		return result;
	}
	inline int parseInt(const std::wstring& _value)
	{
		return parseValue<int>(_value);
	}
	template<typename T>
	inline T parseValue( const std::string& _value )
	{
		std::istringstream stream(_value);
		T result;
		stream >> result;
		if (stream.fail())
			return T();
		else
		{
			int item = stream.get();
			while (item != -1)
			{
				if (item != ' ' && item != '\t')
					return T();
				item = stream.get();
			}
		}
		return result;
	}
	inline int parseInt(const std::string& _value)
	{
		return parseValue<int>(_value);
	}

	template<typename T>
	static std::string getDataValueStr(T* kValue,std::string kName)
	{
		if ((*kValue).HasMember(kName.c_str())&&((*kValue)[kName.c_str()].IsString()))
		{
			return (*kValue)[kName.c_str()].GetString();
		}
		return "";
	}
	template<typename T>
	static int getDataValueInt(T* kValue,std::string kName)
	{
		if ((*kValue).HasMember(kName.c_str())&&((*kValue)[kName.c_str()].IsInt()))
		{
			return (*kValue)[kName.c_str()].GetInt();
		}
		return 0;
	}
	int getUserCard(int iUserID);
	int UseCard(int iUserID,int iCout,std::string kHttpChannel);
};

//////////////////////////////////////////////////////////////////////////////////
