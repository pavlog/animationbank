#ifndef __ANIMATIONBANK_ACCOUNT_MANAGER_H
#define __ANIMATIONBANK_ACCOUNT_MANAGER_H

struct AnimationBank_AccountManager
{
public:
	AnimationBank_AccountManager();
	~AnimationBank_AccountManager();
	//
	//! creates new - overwrites existing if exist
	void Create(const std::string& name,IParamBlock2* pblock);
	bool isExist(const char* name) const;
	bool isKeys(IParamBlock2* pblock) const;
	//
	void Load(const char* name,const std::set<INode*>& skipNodes,IParamBlock2* pblock);
	//
	void Rename(const char* oldName,const char* newName,IParamBlock2* pblock);
	void Delete(const char* name);
	//
	typedef std::map<std::string,AnimationBank_Account*> t_accountList;
	const t_accountList& getAccountMap() const;
	//! param block will be updated
	void SaveToParamBlock(IParamBlock2* pblock);
	//! restore realtime data from pblock (ie init all nodes* and controls*)
	void LoadFromParamBlock(IParamBlock2* pblock);
	//
	//
	IOResult LoadAccount(ILoad* load);
	IOResult SaveAccount(ISave* save);
protected:
	t_accountList m_accountList;
};

#endif