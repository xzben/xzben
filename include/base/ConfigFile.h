/************************************************************************************
 *	File   Name: 																	*
 * 	Description:  																	* 													
 *																					*
 *	Create Time:																	*
 *		 Author: xzben																*	
 *	Author Blog:  www.xzben.com														*
 ************************************************************************************
 * Modify Log: 																		*
 *																					*
 ************************************************************************************/
#ifndef __2013_10_22_CONFIG_FILE_H__
#define __2013_10_22_CONFIG_FILE_H__

#include "File.h"
#include <map>
#include <string>
#include <cassert>
namespace XZBEN
{
#define MAX_LINE	1024
class ConfigFile
{
public:
	typedef std::map<std::string, std::string>	ConfigItemMap;
	ConfigFile();
	ConfigFile(const char* strFile, bool bRead);
	virtual ~ConfigFile();

	bool ReadFile(const char* strFile);
	bool SaveFile(const char* strFile);

	bool SetInt(std::string strSection, std::string strKey, int nValue);
	bool SetString(std::string strSection, std::string strKey, std::string strValue);
	bool SetDouble(std::string strSection, std::string strKey, double dValue);
	
	int GetInt(std::string strSection, std::string strKey, int nDefault);
	double GetDouble(std::string strSection, std::string strKey, double dDefault);
	std::string GetString(std::string strSection, std::string strKey, std::string strDefault);

	bool RemoveProfileItem(std::string strSection, std::string strKey);
private:
	bool			m_bInit;
	File			m_file;
	ConfigItemMap	m_Content;
};

};//namespace XZBEN
#endif//__2013_10_22_CONFIG_FILE_H__