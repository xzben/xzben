#include "ConfigFile.h"
#include <algorithm>

namespace XZBEN
{
ConfigFile::ConfigFile()
{
	m_bInit = false;
}

ConfigFile::~ConfigFile()
{
	m_Content.clear();
}

bool IsSpace(char ch)
{
	if(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\v' || ch == '\f')
		return true;
	return false;
}
int trimString(char *strTrim)
{
	if(nullptr == strTrim) return 0;

	int nlen = strlen(strTrim);
	int i;

	for(i = nlen-1; i >= 0; i--)
	{
		if( IsSpace(strTrim[i]) )
			strTrim[i] = '\0';
		else
			break;
	}
	nlen = i + 1;

	for(i = 0; strTrim[i] != '\0'; i++)
	{
		if( !IsSpace(strTrim[i]) )
			break;
	}
	nlen = nlen - i;
	strcpy_s(strTrim, MAX_LINE, &strTrim[i]);

	return nlen;
}

bool ConfigFile::ReadFile(const char* strFile)
{
	if( !m_file.Open(strFile, File::modeRead))
		return false;

	char buffer[MAX_LINE];
	char strSection[MAX_LINE] = "Global";
	char strKey[MAX_LINE] = "";
	char strValue[MAX_LINE] = "";

	while( m_file.ReadLine(buffer, MAX_LINE) )
	{
		if(0 == trimString(buffer)) //去除左右空字符，并判读长度是否为0
			continue;
		if(buffer[0] == '#') //注释
			continue;
		
		int nLen = strlen(buffer);
		if(buffer[0] == '[' && buffer[nLen-1] == ']')
		{
			buffer[nLen-1] = ' ';
			buffer[0] = ' ';
			trimString(buffer);
			strcpy_s(strSection, MAX_LINE, buffer);
			continue; 
		}

		std::string strBuffer = buffer;
		int nIndex = strBuffer.find('=', 0);
		if( std::string::npos == nIndex )
			continue;
		buffer[nIndex] = '\0';
		strcpy_s(strKey, MAX_LINE, buffer);
		strcpy_s(strValue, MAX_LINE, &buffer[nIndex+1]);

		trimString(strKey);
		trimString(strValue);
		
		sprintf_s(buffer, MAX_LINE, "%s.%s", strSection, strKey);

		m_Content.insert(ConfigItemMap::value_type(buffer, strValue));
	}
	m_file.Close();
	return true;
}

bool ConfigFile::SaveFile(const char* strFile)
{
	if( !m_file.Open(strFile, File::modeWrite))
		return false;
	//Map 会自动按 key 排序
	char strWrite[MAX_LINE];
	char strSection[MAX_LINE];
	char strKey[MAX_LINE];
	char strBuffer[MAX_LINE];
	char strOldSection[MAX_LINE] = "";
	bool bFirstSection = true;
	for(ConfigItemMap::iterator it = m_Content.begin(); it != m_Content.end(); it++)
	{
		std::string strSectionKey = it->first;
		int nIndex = strSectionKey.find('.', 0);
		if( std::string::npos == nIndex )
			continue;

		sprintf_s(strBuffer, MAX_LINE, "%s", strSectionKey.c_str());
		strBuffer[nIndex] = '\0';
		strcpy_s(strSection, MAX_LINE,  strBuffer);
		strcpy_s(strKey, MAX_LINE, &strBuffer[nIndex+1]);
		
		
		if( 0 == strlen(strOldSection) || 0 != strcmp(strOldSection, strSection))
		{
			if( !bFirstSection )
				m_file.WriteLine("");
			else 
				bFirstSection = false;

			sprintf_s(strWrite, MAX_LINE, "[ %s ]", strSection);
			m_file.WriteLine(strWrite);
		}

		sprintf_s(strWrite, MAX_LINE, "%s = %s", strKey, it->second.c_str());
		m_file.WriteLine(strWrite);

		sprintf_s(strOldSection, MAX_LINE, "%s", strSection);
	}
	m_file.Close();
	return true;
}

bool ConfigFile::SetInt(std::string strSection, std::string strKey, int nValue)
{
	char strSectionKey[MAX_LINE];
	char strValue[MAX_LINE];
	sprintf_s(strSectionKey, MAX_LINE, "%s.%s", strSection.c_str(), strKey.c_str());
	sprintf_s(strValue, MAX_LINE, "%d", nValue);

	ConfigItemMap::iterator it = m_Content.find(strSectionKey);
	if(it == m_Content.end())
	{
		m_Content.insert(ConfigItemMap::value_type(strSectionKey, strValue));
	}
	else
	{
		it->second = strValue;
	}

	return true;
}
bool ConfigFile::SetString(std::string strSection, std::string strKey, std::string strValue)
{
	char strSectionKey[MAX_LINE];
	sprintf_s(strSectionKey, MAX_LINE, "%s.%s", strSection.c_str(), strKey.c_str());
	ConfigItemMap::iterator it = m_Content.find(strSectionKey);
	if(it == m_Content.end())
	{
		m_Content.insert(ConfigItemMap::value_type(strSectionKey, strValue));
	}
	else
	{
		it->second = strValue;
	}
	return true;
}
bool ConfigFile::SetDouble(std::string strSection, std::string strKey, double dValue)
{
	char strSectionKey[MAX_LINE];
	char strValue[MAX_LINE];
	sprintf_s(strSectionKey, MAX_LINE, "%s.%s", strSection.c_str(), strKey.c_str());
	sprintf_s(strValue, MAX_LINE, "%d", dValue);
	ConfigItemMap::iterator it = m_Content.find(strSectionKey);
	if(it == m_Content.end())
	{
		m_Content.insert(ConfigItemMap::value_type(strSectionKey, strValue));
	}
	else
	{
		it->second = strValue;
	}
	return true;
}

int ConfigFile::GetInt(std::string strSection, std::string strKey, int nDefault)
{
	char strSectionKey[MAX_LINE];
	sprintf_s(strSectionKey, MAX_LINE, "%s.%s", strSection.c_str(), strKey.c_str());

	ConfigItemMap::iterator it = m_Content.find(strSectionKey);
	if(it == m_Content.end())
	{
		return nDefault;
	}
	else
		return 	atoi(it->second.c_str());
}
double ConfigFile::GetDouble(std::string strSection, std::string strKey, double dDefault)
{
	char strSectionKey[MAX_LINE];
	sprintf_s(strSectionKey, MAX_LINE, "%s.%s", strSection.c_str(), strKey.c_str());

	ConfigItemMap::iterator it = m_Content.find(strSectionKey);
	if(it == m_Content.end())
	{
		return dDefault;
	}
	else
		return 	atof(it->second.c_str());
}
std::string ConfigFile::GetString(std::string strSection, std::string strKey, std::string strDefault)
{
	char strSectionKey[MAX_LINE];
	sprintf_s(strSectionKey, MAX_LINE, "%s.%s", strSection.c_str(), strKey.c_str());

	ConfigItemMap::iterator it = m_Content.find(strSectionKey);
	if(it == m_Content.end())
	{
		return strDefault;
	}
	else
		return 	it->second;
}

bool ConfigFile::RemoveProfileItem(std::string strSection, std::string strKey)
{
	char strSectionKey[MAX_LINE];
	sprintf_s(strSectionKey, MAX_LINE, "%s.%s", strSection.c_str(), strKey.c_str());

	ConfigItemMap::iterator it = m_Content.find(strSectionKey);
	if(it == m_Content.end())
		return false;
	
	m_Content.erase(it);
	return true;
}
};//namespace XZBEN 