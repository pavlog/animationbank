#ifndef __AnimationBank_ACCOUNT_H
#define __AnimationBank_ACCOUNT_H



struct AnimationBank_Account
{

#pragma pack(push, AccountInfo, 4)
#pragma pack(show)   // C4810
	struct AccountInfo
	{
		AccountInfo()
		{
			m_version = 0;
		}
		int m_version;
		//
		TimeValue start;
		TimeValue end;
		TimeValue cur;
		unsigned char reserved[100];
	};

	struct NodeInfoRecord
	{
		NodeInfoRecord()
		{
			m_nodeIndex = -1;
			m_controlIndex = -1;
			m_userPropDataIndex = -1;
			memset(reserved,0,sizeof(reserved));
		}
		int m_nodeIndex;
		int m_controlIndex;
		int m_userPropDataIndex;
		int m_bipDataLen;// in bytes
		unsigned char reserved[20];
	};
	/*
	struct NodeKeysData
	{
		NodeKeysData()
		{
			m_nodeIndex = -1;
			m_controlIndex = -1;
			m_userPropDataIndex = -1;
			memset(reserved,0,sizeof(reserved));
		}
		int m_DataLen;// in bytes
		unsigned char reserved[20];
	};
	*/
#pragma pack(pop, AccountInfo)   // n = 2 , stack popped
#pragma pack(show)   // C4810

	struct BipBoneInfo
	{
		BipBoneInfo()
		{
			m_iAttachNodeIK = -1;
			m_pAttachNodeIK = NULL;
		}
		int m_iAttachNodeIK;// IK node for character studio
		INode* m_pAttachNodeIK;// IK node for character studio
	};

	struct NodeInfo
	{
		NodeInfo()
		{
			m_node = NULL;
			m_control = NULL;
		};
		INode* m_node;
		Control* m_control;
		BipBoneInfo m_bipBoneInfo;
		//
		NodeInfoRecord m_dbRecord;// in order of saving and loading
		std::string m_userPropData;
		std::vector<unsigned char> m_bipData;
		std::vector<unsigned char> m_keysData;
	};

	AccountInfo m_accountInfo;

	NodeInfo* getNodeInfo(INode* node);
	NodeInfo* getBipMasterNodeInfo(IBipMaster8* bipMaster);

	~AnimationBank_Account()
	{
	}
	std::vector<NodeInfo> nodeList;
	std::map<Control*,Control*> cloneMap;
	//
	void EnumNode(INode* node,std::vector<INode*>& nodes);
	void Cleanup();

	void Save(const std::set<INode*>& skipNodes,bool bKeysOnly);
	// load
	void Load(const std::set<INode*>& skipNodes,bool bKeysOnly);
	//
	void SaveToParamBlock(IParamBlock2* pblock);
	void LoadFromParamBlock(IParamBlock2* pblock);
	//
	IOResult LoadAccount(ILoad* load);
	IOResult SaveAccount(ISave* save);
};


#endif