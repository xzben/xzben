/************************************************************************************
 *	File   Name: AutoDelete.h														*
 * 	Description: 主要利用局部变量生命周期原理，										*
 *				在生命周期结束时，释放其存储的对象的内存							*
 *				此类目前还未在框架中使用，只是一时兴起写来的，还未做测试			* 													
 *																					*
 *	Create Time:																	*
 *		 Author: xzben																*	
 *	Author Blog:  www.xzben.com														*
 ************************************************************************************
 * Modify Log: 																		*
 *																					*
 ************************************************************************************/

#ifndef __2013_08_17_AUTO_DELETE_H__
#define __2013_08_17_AUTO_DELETE_H__

namespace XZBEN
{
template<typename DeleteObj>
class  AutoDelete
{
public:
	AutoDelete(DeleteObj *pObj)
	{
		m_pObj = pObj;			
	}
	~AutoDelete()
	{
		if(m_pObj)
			delete m_pObj;
		m_pObj = NULL;
	}
private:
	DeleteObj *m_pObj;
};
};//namespace XZBEN
#endif//__2013_08_17_AUTO_DELETE_H__