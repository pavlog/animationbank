#include "StdAfx.h"
#include "AnimationBank.h"
#include "AnimationBank_Account.h"
#include "AnimationBank_AccountManager.h"

AnimationBank_AccountManager::AnimationBank_AccountManager()
{

}

AnimationBank_AccountManager::~AnimationBank_AccountManager()
{
	for(t_accountList::iterator it=m_accountList.begin();it!=m_accountList.end();it++)
	{
		if( it->second )
		{
			delete it->second;
		}
	}
	m_accountList.clear();
}

const AnimationBank_AccountManager::t_accountList& AnimationBank_AccountManager::getAccountMap() const
{
	return m_accountList;
}

bool AnimationBank_AccountManager::isExist(const char* name) const
{
	return ( m_accountList.find(name)!=m_accountList.end() );
}

bool AnimationBank_AccountManager::isKeys(IParamBlock2* pblock) const
{
	int aword = 0;
	pblock->GetValue(pb_saveloadMethod,0,aword,FOREVER,0);
	if( aword==1 )
		return true;
	return false;
}


void AnimationBank_AccountManager::Create(const std::string& name,IParamBlock2* pblock)
{

	Interface* ip = GetCOREInterface();
	{
		/*
		INode* node = ip->GetRootNode()->GetChildNode(2);
		Control* ctrl = node->GetTMController();
		IBipMaster8* bipMaster = GetBipMaster8Interface(ctrl);
		if( bipMaster )
		{
			char * temFN = _tempnam("","ANB");
			//
			int res = bipMaster->SaveBipfileSegment(temFN,ip->GetAnimRange().Start(),ip->GetAnimRange().End(),FALSE,FALSE,FALSE);
		}
		return;
		*/
	}
	//
	AnimationBank_Account* pAccount = new AnimationBank_Account();
	std::set<INode*> skipNodes;
	{
		int count = pblock->Count(pb_skipNodes);
		for(int n=0;n<count;n++)
		{
			INode* node = NULL;
			pblock->GetValue(pb_skipNodes,0,node,FOREVER,n);
			skipNodes.insert(node);
		}
	}
	//
	pAccount->Save(skipNodes,isKeys(pblock));
	//
	if( m_accountList[name]!=NULL )
	{
		delete m_accountList[name];
	}
	//
	m_accountList[name] = pAccount;
	//
	int index = -1;
	int count = pblock->Count(pb_animationList);
	for(int n=0;n<count;n++)
	{
#if (MAX_PRODUCT_VERSION_MAJOR != 14)
	const TCHAR* pVal = NULL;
#else
	const MCHAR* pVal = NULL;
#endif
		pblock->GetValue(pb_animationList,0,pVal,FOREVER,n);
		if( pVal )
		{
			if( stricmp(pVal,name.c_str())==0 )
			{
				index = n;
				break;
			}
		}
	}
	//
	if( index!=-1 )
	{
		pblock->SetValue(pb_animationList,0,(TCHAR*)name.c_str(),index);
	}
	else
	{
		TCHAR* str = (TCHAR*)name.c_str();
		pblock->Append(pb_animationList,1,(TCHAR**)&str);
	}
	//
	SaveToParamBlock(pblock);
	pAccount->Load(skipNodes,isKeys(pblock));
}

void AnimationBank_AccountManager::Load(const char* name,const std::set<INode*>& skipNodes,IParamBlock2* pblock)
{
	Interface* ip = GetCOREInterface();
	if( !isExist(name) )
	{
		char buffer[1024];
		sprintf(buffer,"Account with name:%s not found",name);
		MessageBox(ip->GetMAXHWnd(),buffer,"Error",MB_OK);
		return;
	}
	//
	m_accountList[name]->Load(skipNodes,isKeys(pblock));
}

void AnimationBank_AccountManager::Rename(const char* oldName,const char* newName,IParamBlock2* pblock)
{
	if( !isExist(oldName) )
	{
		MessageBox(GetCOREInterface()->GetMAXHWnd(),"Error","Failed",MB_OK);
		return;
	}
	//
	AnimationBank_Account* pOldAccount = m_accountList[oldName];
	if( m_accountList[newName]!=NULL )
	{
		delete m_accountList[newName];
	}
	m_accountList.erase(oldName);
	//
	m_accountList[newName] = pOldAccount;
	SaveToParamBlock(pblock);
}

void AnimationBank_AccountManager::Delete(const char* name)
{
	if( m_accountList[name]!=NULL )
	{
		delete m_accountList[name];
	}
	m_accountList.erase(name);
}


void AnimationBank_AccountManager::SaveToParamBlock(IParamBlock2* pblock)
{
	pblock->ZeroCount(pb_controlListTemp);
	// to leave references
	for(int n=0;n<pblock->Count(pb_controlList);n++)
	{
		ReferenceTarget* pCtrl = NULL;
		pblock->GetValue(pb_controlList,0,pCtrl,FOREVER,n);
		pblock->Append(pb_controlListTemp,1,&pCtrl);
	}
	//
	pblock->ZeroCount(pb_animationList);
	pblock->ZeroCount(pb_nodeList);
	pblock->ZeroCount(pb_controlList);
	//
	//
	for(t_accountList::iterator it=m_accountList.begin();it!=m_accountList.end();it++)
	{
		TCHAR* pStr = (TCHAR*)it->first.c_str();
		pblock->Append(pb_animationList,1,&pStr);
		//
		it->second->SaveToParamBlock(pblock);
	}
	//
	pblock->ZeroCount(pb_controlListTemp);
}

enum ChunkID
{
	ACCMANAGER_ACCOUNT_NAME = 4321,
	ACCMANAGER_ACCOUNT_DATA = 4322,
};

IOResult AnimationBank_AccountManager::SaveAccount(ISave* isave)
{
	for(t_accountList::iterator it=m_accountList.begin();it!=m_accountList.end();it++)
	{
		//
		isave->BeginChunk(ACCMANAGER_ACCOUNT_NAME);
		isave->WriteCString(it->first.c_str());
		isave->EndChunk();
		//
		isave->BeginChunk(ACCMANAGER_ACCOUNT_DATA);
		it->second->SaveAccount(isave);
		isave->EndChunk();
	}
	return IO_OK;
}


IOResult AnimationBank_AccountManager::LoadAccount(ILoad* iload)
{
	//
	m_accountList.clear();
	//
	std::string curName="";
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) 
	{
		if( iload->CurChunkID()==ACCMANAGER_ACCOUNT_NAME )
		{
			char* pBuffer = NULL;
			iload->ReadCStringChunk(&pBuffer);
			curName = pBuffer;
		}
		else if( iload->CurChunkID()==ACCMANAGER_ACCOUNT_DATA )
		{
			m_accountList[curName] = new AnimationBank_Account();
			m_accountList[curName]->LoadAccount(iload);
		}
		iload->CloseChunk();
		if (res!=IO_OK)  return res;
	}
	return IO_OK;
}


void AnimationBank_AccountManager::LoadFromParamBlock(IParamBlock2* pblock)
{
	for(t_accountList::iterator it=m_accountList.begin();it!=m_accountList.end();it++)
	{
		it->second->LoadFromParamBlock(pblock);
	}
}

