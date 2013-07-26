#include "StdAfx.h"
#include "AnimationBank.h"
#include "AnimationBank_Account.h"

void AnimationBank_Account::EnumNode(INode* node,std::vector<INode*>& nodes)
{
	bool bAdd = true;
	if( node->GetObjectRef() )
	{
		if( node->GetObjectRef()->ClassID()==AnimationBank_CLASS_ID )
		{
			bAdd = false;
		}
	}
	if( bAdd )
	{
		nodes.push_back(node);
	}
	//
	for(int n=0;n<node->NumberOfChildren();n++)
	{
		EnumNode(node->GetChildNode(n),nodes);
	}
}
void AnimationBank_Account::Cleanup()
{
	nodeList.clear();
}

AnimationBank_Account::NodeInfo* AnimationBank_Account::getNodeInfo(INode* node)
{
	for(size_t n=0;n<nodeList.size();n++)
	{
		if( nodeList[n].m_node==node )
		{
			return &nodeList[n];
		}
	}
	return NULL;
}

AnimationBank_Account::NodeInfo* AnimationBank_Account::getBipMasterNodeInfo(IBipMaster8* bipMaster)
{
	for(size_t n=0;n<nodeList.size();n++)
	{
		INode* node = nodeList[n].m_node;
		Control* ctrl = node->GetTMController();
		if( ctrl )
		{
			IBipMaster8* ex_bipMaster = GetBipMaster8Interface(ctrl);
			if( bipMaster==ex_bipMaster )
			{
				return &nodeList[n];
			}
		}
	}
	return NULL;
}

void collectControllers(Control* root,std::vector<Control*>& ctrlList)
{
	if( root )
	{
		ctrlList.push_back(root);
		for(int n=0;n<root->NumSubs();n++)
		{
			TSTR name = root->SubAnimName(n);
			collectControllers((Control*)root->SubAnim(n),ctrlList);
		}
	}
}

void clearKeys(Control* root)
{
	if( root )
	{
		for(int n=0;n<root->NumSubs();n++)
		{
			TSTR name = root->SubAnimName(n);
			if( root->NumKeys()!=0 )
			{
				root->DeleteKeys(TRACK_DOALL);
			}
			clearKeys((Control*)root->SubAnim(n));
		}
	}
}

void RestoreWireForSkipped(const std::set<INode*>& skipNodes,const std::vector<Control*>& ctrlListBefore,const std::vector<Control*>& ctrlListAfter,const std::vector<Control*>& ctrlListOriginal)
{
	// restore wire for skipped
	//
	std::vector<Control*> ctrlListSkip;
	// check all skipped 
	for(std::set<INode*>::const_iterator it=skipNodes.begin();it!=skipNodes.end();it++)
	{
		INode* node = *it;
		if( node )
		{
			Control* ctrl = node->GetTMController();
			if( ctrl )
			{
				IBipMaster8* bipMaster = GetBipMaster8Interface(ctrl);
				if( !bipMaster )
				{
					collectControllers(ctrl,ctrlListSkip);
				}
			}
		}
	}
	for(int n=0;n<ctrlListSkip.size();n++)
	{
		// check for wire
		IBaseWireControl* wire = (IBaseWireControl*)ctrlListSkip[n]->GetInterface(WIRE_CONTROLLER_INTERFACE);
		if( wire )
		{
			Control* slave_ctrl = wire->get_slave_animation();
			if( slave_ctrl )
			{
				// if slave in account
				if( std::find(ctrlListSkip.begin(),ctrlListSkip.end(),slave_ctrl)==ctrlListSkip.end() )
				{
					// find in ctrlList
					bool bFound = false;
					for(int i=0;i<ctrlListBefore.size();i++)
					{
						if( ctrlListBefore[i]==slave_ctrl )
						{
							bFound = true;
							if( !wire->is_two_way() )
							{
								wire->set_slave_animation(ctrlListAfter[i]);
							}
							else
							{
								if( wire->is_master() )
								{
									int doSomething = 0;
								}
								if( wire->is_slave() )
								{
									int doSomething = 0;
									//IParamWireMgr* mgr = (IParamWireMgr*)GetCOREInterface(PARAMWIRE_MGR_INTERFACE);
									//mgr->Connect2Way(ctrlListSkip[n]wire->get_wire_parent(),wire->)
									//CStr name;
									//ctrlListAfter[i]->GetClassName(name);
									//wire->set_slave_animation(ctrlListAfter[i]);
								}
							}
						}
					}
					//
					if( !bFound )
					{
						for(int i=0;i<ctrlListOriginal.size();i++)
						{
							if( ctrlListOriginal[i]==slave_ctrl )
							{
								bFound = true;
								if( !wire->is_two_way() )
								{
									wire->set_slave_animation(ctrlListAfter[i]);
								}
								else
								{
									if( wire->is_master() )
									{
										int doSomething = 0;
									}
									if( wire->is_slave() )
									{
										int doSomething = 0;
										//IParamWireMgr* mgr = (IParamWireMgr*)GetCOREInterface(PARAMWIRE_MGR_INTERFACE);
										//mgr->Connect2Way(ctrlListSkip[n]wire->get_wire_parent(),wire->)
										//CStr name;
										//ctrlListAfter[i]->GetClassName(name);
										//wire->set_slave_animation(ctrlListAfter[i]);
									}
								}
							}
						}
						
					}
				}
			}
		}
	}
}

void AnimationBank_Account::Save(const std::set<INode*>& skipNodes,bool bKeysOnly)
{
	//
	Cleanup();
	//
	RemapDir* remap = NewRemapDir();
	//
	Interface* ip = GetCOREInterface();
	//
	m_accountInfo.start = ip->GetAnimRange().Start();
	m_accountInfo.end = ip->GetAnimRange().End();
	m_accountInfo.cur = ip->GetTime();
	//
	INode* root = ip->GetRootNode();
	std::vector<INode*> nodes;
	for(int n=0;n<root->NumberOfChildren();n++)
	{
		EnumNode(root->GetChildNode(n),nodes);
	}
	//
	for(std::set<INode*>::const_iterator it=skipNodes.begin();it!=skipNodes.end();it++)
	{
		INode* node = *it;
		if( node )
		{
			Control* ctrl = node->GetTMController();
			if( ctrl )
			{
				IBipMaster8* bipMaster = GetBipMaster8Interface(ctrl);
				if( !bipMaster )
				{
					std::vector<Control*> ctrlListSkip;
					collectControllers(ctrl,ctrlListSkip);
					for(size_t n=0;n<ctrlListSkip.size();n++)
					{
						/*
						IBaseWireControl* wire = (IBaseWireControl*)ctrlListSkip[n]->GetInterface(WIRE_CONTROLLER_INTERFACE);
						if( wire )
						{
						if( !wire->is_two_way() )
						{
						remap->AddEntry(ctrlListSkip[n],ctrlListSkip[n]);
						}
						}
						else
						{
						*/
						remap->AddEntry(ctrlListSkip[n],ctrlListSkip[n]);
						/*
						}
						*/
					}
				}
			}
		}
	}
	//
	std::vector<Control*> ctrlListBefore;
	std::vector<Control*> ctrlListAfter;
	//
	for(int n=0;n<nodes.size();n++)
	{
		INode* node = nodes[n];
		//
		if( skipNodes.find(node)==skipNodes.end() )
		{
			NodeInfo info;
			info.m_node = node;
			//
			TCHAR* name = node->GetName();
			//
			Control* ctrl = node->GetTMController();
			if( ctrl )
			{
				IBipMaster8* bipMaster = GetBipMaster8Interface(ctrl);
				if( bipMaster )
				{
					if( !getBipMasterNodeInfo(bipMaster) )
					{
						char * temFN = _tempnam("","ANB");
						//
						int res = bipMaster->SaveBipfileSegment(temFN,m_accountInfo.start,m_accountInfo.end,FALSE,FALSE,FALSE);
						if( res )
						{
							//
							info.m_bipData.clear();
							//
							FILE* fp = fopen(temFN,"rb");
							if( fp )
							{
								fseek(fp,0,SEEK_END);
								long len = ftell(fp);
								fseek(fp,0,SEEK_SET);
								//
								info.m_bipData.resize(len);
								fread(&info.m_bipData[0],len,1,fp);
								fclose(fp);
								remove(temFN);
							}
						}
					}
				}
				else
				{
					if( bKeysOnly )
					{
						//
						ILoadSaveAnimation* itf = static_cast<ILoadSaveAnimation*>(GetCOREInterface(ILOADSAVEANIMATION_INTERFACE));
						//
						std::string temFN = _tempnam("","ANBK");
						temFN+=".xaf";
						//
						Tab<CStr*> noneKeys;
						Tab<CStr*> noneVals;
						//
						info.m_keysData.clear();
						//
						Tab<INode*> nodes;
						nodes.Append(1,&node);
						BOOL bRes = itf->SaveAnimation((TCHAR*)temFN.c_str(),nodes,noneKeys,noneVals,ILoadSaveAnimation::eKeyableTracks,FOREVER);
						if( bRes )
						{
							FILE* fp = fopen(temFN.c_str(),"rb");
							if( fp )
							{
								fseek(fp,0,SEEK_END);
								long len = ftell(fp);
								fseek(fp,0,SEEK_SET);
								//
								info.m_keysData.resize(len);
								fread(&info.m_keysData[0],len,1,fp);
								fclose(fp);
								remove(temFN.c_str());
							}
						}
						info.m_control = ctrl;
					}
					else
					{
						//
						collectControllers(ctrl,ctrlListBefore);
						//
						Control* ctrlCopy = (Control*)ctrl->Clone(*remap);
						//
						collectControllers(ctrlCopy,ctrlListAfter);
						//
						info.m_control = ctrlCopy;
						//
					}
				}
			}
			//
			//
			TSTR userProp;
			info.m_node->GetUserPropBuffer(userProp);
			info.m_userPropData = userProp;
			//
			nodeList.push_back(info);
		}
	}
	//
	//
	// update wire parameters
	//
	/*
	bool bChanged=false;
	do
	{
		bChanged=false;
		for(int n=0;n<ctrlListBefore.size();n++)
		{
			IBaseWireControl* wire = (IBaseWireControl*)ctrlListBefore[n]->GetInterface(WIRE_CONTROLLER_INTERFACE);
			if( wire )
			{
				bool master = wire->is_master();
				bool slave = wire->is_slave();
				bool two_way = wire->is_two_way();
				//
				Control* slave_ctrl = wire->get_slave_animation();
				if( slave_ctrl )
				{
					IBaseWireControl* wireClone = (IBaseWireControl*)ctrlListAfter[n]->GetInterface(WIRE_CONTROLLER_INTERFACE);
					//
					if( wireClone )
					{
						bool clone_master = wireClone->is_master();
						bool clone_slave = wireClone->is_slave();
						bool clone_two_way = wireClone->is_two_way();
						//
						Control* slave_ctrlClone = wireClone->get_slave_animation();
						if( slave_ctrlClone )
						{
							// find in ctrlList
							for(int i=0;i<ctrlListBefore.size();i++)
							{
								if( ctrlListBefore[i]==slave_ctrl )
								{
									if( ctrlListAfter[i]!=slave_ctrlClone )
									{
										wireClone->set_slave_animation(ctrlListAfter[i]);
										bChanged = true;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	while(bChanged==true);
	*/
	//
	if( !bKeysOnly )
	{
		RestoreWireForSkipped(skipNodes,ctrlListBefore,ctrlListAfter,std::vector<Control*>());
	}
	//
	int cmdMode = ip->GetCommandPanelTaskMode();
	ip->SetCommandPanelTaskMode(TASK_MODE_MOTION);
	//
	for(size_t n=0;n<nodeList.size();n++)
	{
		INode* node = nodeList[n].m_node;
		Control* ctrl = node->GetTMController();
		if( ctrl )
		{
			IBipMaster8* bipMaster = GetBipMaster8Interface(ctrl);
			if( bipMaster )
			{
				theHold.Begin();
				ip->SelectNode(node);
				nodeList[n].m_bipBoneInfo.m_pAttachNodeIK = NULL;
				nodeList[n].m_bipBoneInfo.m_pAttachNodeIK = bipMaster->GetAttachNode();
				if( nodeList[n].m_bipBoneInfo.m_pAttachNodeIK )
				{
					int y=0;
				}
				theHold.Accept("Select");
			}
		}
	}
	//
	ip->SetCommandPanelTaskMode(cmdMode);
}
//
bool isBiped(INode* node)
{
	Control* ctrlCur = node->GetTMController();
	//
	if( ctrlCur )
	{
		IBipMaster8* bipMaster = GetBipMaster8Interface(ctrlCur);
		if( bipMaster )
		{
			return true;
		}
	}
	return false;
}
// Return whether first element is greater than the second
bool UDgreater ( INode* elem1, INode* elem2 )
{
	return ((isBiped(elem1) ? 1 :0)<(isBiped(elem2) ? 1 :0));
}

// load
void AnimationBank_Account::Load(const std::set<INode*>& skipNodes,bool bKeysOnly)
{
	//
	Interface* ip = GetCOREInterface();
	//
	//
	INode* root = ip->GetRootNode();
	std::vector<INode*> nodes;
	for(int n=0;n<root->NumberOfChildren();n++)
	{
		EnumNode(root->GetChildNode(n),nodes);
	}
	//
	std::sort(nodes.begin(),nodes.end(),UDgreater);
	for(int n=0;n<nodes.size();n++)
	{
		INode* node = nodes[n];
		TCHAR* name = node->GetName();
		int y=0;
	}
	//
	//
	std::set<IBipMaster8*> bLoadedMap;
	//
	RemapDir* remap = NewRemapDir();
	//
	//
///*
	for(std::set<INode*>::const_iterator it=skipNodes.begin();it!=skipNodes.end();it++)
	{
		INode* node = *it;
		if( node )
		{
			Control* ctrl = node->GetTMController();
			if( ctrl )
			{
				IBipMaster8* bipMaster = GetBipMaster8Interface(ctrl);
				if( !bipMaster )
				{
					std::vector<Control*> ctrlListSkip;
					collectControllers(ctrl,ctrlListSkip);
					for(size_t n=0;n<ctrlListSkip.size();n++)
					{
						/*
						IBaseWireControl* wire = (IBaseWireControl*)ctrlListSkip[n]->GetInterface(WIRE_CONTROLLER_INTERFACE);
						if( wire )
						{
						if( !wire->is_two_way() )
						{
						remap->AddEntry(ctrlListSkip[n],ctrlListSkip[n]);
						}
						}
						else
						{
						*/
						remap->AddEntry(ctrlListSkip[n],ctrlListSkip[n]);
						/*
						}
						*/
					}
				}
			}
		}
	}
//*/
	//
	std::vector<Control*> ctrlListOriginal;
	std::vector<Control*> ctrlListBefore;
	std::vector<Control*> ctrlListAfter;
	std::vector<INode*> bipNodes;
	std::vector<IBipMaster8*> bipMasters;
	//
	//
	for(int n=0;n<nodes.size();n++)
	{
		INode* node = nodes[n];
		TCHAR* name = node->GetName();
		if( skipNodes.find(node)==skipNodes.end() )
		{
			Control* ctrlCur = node->GetTMController();
			//
			NodeInfo* info = getNodeInfo(node);
			if( info )
			{
				if( info->m_control )
				{
					if( bKeysOnly )
					{
						ILoadSaveAnimation* itf = static_cast<ILoadSaveAnimation*>(GetCOREInterface(ILOADSAVEANIMATION_INTERFACE));
						//
						clearKeys(ctrlCur);
						//
						std::string temFN = _tempnam("","ANBK");
						temFN+=".xaf";
						FILE* fp = fopen(temFN.c_str(),"wb");
						if( fp )
						{
							fwrite(&info->m_keysData[0],info->m_keysData.size(),1,fp);
							fclose(fp);
						}
						//
						Tab<INode*> nodes;
						nodes.Append(1,&node);
						BOOL bRes = itf->LoadAnimation((TCHAR*)temFN.c_str(),nodes,0,0,FALSE,NULL);
						if( bRes==FALSE )
						{
							MessageBox(GetCOREInterface()->GetMAXHWnd(),"Animation Load Error","Error",MB_OK);
						}
						//
						if( fp )
						{
							remove(temFN.c_str());
						}
					}
					else
					{
						collectControllers(ctrlCur,ctrlListOriginal);
						collectControllers(info->m_control,ctrlListBefore);
						//
						Control* ctrlNew = (Control*)info->m_control->Clone(*remap);//*NewRemapDir());
						Matrix3 mt;
						ctrlNew->GetValue(m_accountInfo.cur,&mt,FOREVER);
						node->SetNodeTM(m_accountInfo.cur,mt);
						node->SetTMController(ctrlNew);
						//
						collectControllers(ctrlNew,ctrlListAfter);
					}
				}
				else
				{
					IBipMaster8* bipMaster = ctrlCur ? GetBipMaster8Interface(ctrlCur) : NULL;
					// check is biped
					if( bipMaster )
					{
						bipNodes.push_back(node);
						bipMasters.push_back(bipMaster);
						NodeInfo* info = getBipMasterNodeInfo(bipMaster);
						if( info )
						{
							if( bLoadedMap.find(bipMaster)==bLoadedMap.end() )
							{
								// load bip
								std::string temFN = _tempnam("","ANB");
								temFN+=".bip";
								FILE* fp = fopen(temFN.c_str(),"wb");
								if( fp )
								{
									fwrite(&info->m_bipData[0],info->m_bipData.size(),1,fp);
									fclose(fp);
								}
								//
								int res = bipMaster->LoadBipStpfile((TCHAR*)temFN.c_str(),true,false);
								if( res==0 )
								{
									MessageBox(GetCOREInterface()->GetMAXHWnd(),"Bip Load Error","Error",MB_OK);
								}
								//
								if( fp )
								{
									remove(temFN.c_str());
								}
								//
								bLoadedMap.insert(bipMaster);
							}
						}
					}
				}
				//
				// user prop
				//
				info->m_node->SetUserPropBuffer(info->m_userPropData.c_str());
			}
		}
	}
	//
	int cmdMode = ip->GetCommandPanelTaskMode();
	ip->SetCommandPanelTaskMode(TASK_MODE_MOTION);
	//
	for(int n=0;n<nodes.size();n++)
	{
		INode* node = nodes[n];
		TCHAR* name = node->GetName();
		Control* ctrlCur = node->GetTMController();
		//
		if( ctrlCur )
		{
			NodeInfo* info = getNodeInfo(node);
			if( info )
			{
				IBipMaster8* bipMaster = GetBipMaster8Interface(ctrlCur);
				// check is biped
				if( bipMaster )
				{
					if( info->m_bipBoneInfo.m_pAttachNodeIK )
					{
						TCHAR* name = info->m_bipBoneInfo.m_pAttachNodeIK->GetName();
						theHold.Begin();
						ip->SelectNode(node);
						bipMaster->SetAttachNode(info->m_bipBoneInfo.m_pAttachNodeIK);
						theHold.Accept("Select");
					}
				}
			}
		}
	}
	//
	ip->SetCommandPanelTaskMode(cmdMode);
	//
	if( !bKeysOnly )
	{
		RestoreWireForSkipped(skipNodes,ctrlListBefore,ctrlListAfter,ctrlListOriginal);
	}
	//
	//
	ip->SetAnimRange(Interval(m_accountInfo.start,m_accountInfo.end));
	ip->SetTime(m_accountInfo.cur,1);
}

enum Chunk
{
	ACC_INFO,
	ACC_GROUP,// each this group contains data and user prop
	ACC_DATA,
	ACC_USERPROP,
	ACC_KEYSDATA,
	ACC_CS_ATTACH_IK,
};

IOResult AnimationBank_Account::SaveAccount(ISave* isave)
{
	//
	ULONG nWritten=0;
	//
	isave->BeginChunk(ACC_INFO);
	isave->Write(&m_accountInfo,sizeof(m_accountInfo),&nWritten);
	isave->EndChunk();
	//
	for(int n=0;n<nodeList.size();n++)
	{
		isave->BeginChunk(ACC_GROUP);
		//data
		{ 
			isave->BeginChunk(ACC_DATA);
			nodeList[n].m_dbRecord.m_bipDataLen = nodeList[n].m_bipData.size();
			isave->Write(&nodeList[n].m_dbRecord,sizeof(NodeInfoRecord),&nWritten);
			//
			if( nodeList[n].m_dbRecord.m_bipDataLen!=0 )
			{
				isave->Write(&nodeList[n].m_bipData[0],nodeList[n].m_dbRecord.m_bipDataLen,&nWritten);
			}
			isave->EndChunk();
		}
		// user prop data
		{ 
			isave->BeginChunk(ACC_USERPROP);
			isave->WriteCString(nodeList[n].m_userPropData.c_str());
			isave->EndChunk();
		}
		// Attach IK Node
		{ 
			if( nodeList[n].m_bipBoneInfo.m_iAttachNodeIK!=-1 )
			{
				isave->BeginChunk(ACC_CS_ATTACH_IK);
				isave->Write(&nodeList[n].m_bipBoneInfo.m_iAttachNodeIK,sizeof(int),&nWritten);
				isave->EndChunk();
			}
		}
		//keys data
		{ 
			isave->BeginChunk(ACC_KEYSDATA);
			//
			if( nodeList[n].m_keysData.size()!=0 )
			{
				isave->Write(&nodeList[n].m_keysData[0],nodeList[n].m_keysData.size(),&nWritten);
			}
			isave->EndChunk();
		}
		isave->EndChunk();
	}
	return IO_OK;
}

IOResult AnimationBank_Account::LoadAccount(ILoad* iload)
{
	nodeList.clear();
	//
	ULONG nRead=0;
	//
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) 
	{
		if( iload->CurChunkID()==ACC_INFO )
		{
			iload->Read(&m_accountInfo,sizeof(m_accountInfo),&nRead);
		}
		else if( iload->CurChunkID()==ACC_GROUP )
		{
			nodeList.push_back(NodeInfo());
			//
			IOResult resGroup;
			while (IO_OK==(resGroup=iload->OpenChunk())) 
			{
				if( iload->CurChunkID()==ACC_DATA )
				{
					iload->Read(&nodeList.back().m_dbRecord,sizeof(NodeInfoRecord),&nRead);
					//
					if( nodeList.back().m_dbRecord.m_bipDataLen )
					{
						nodeList.back().m_bipData.resize(nodeList.back().m_dbRecord.m_bipDataLen);
						iload->Read(&nodeList.back().m_bipData[0],nodeList.back().m_dbRecord.m_bipDataLen,&nRead);
					}
				}
				else if( iload->CurChunkID()==ACC_USERPROP )
				{
					TCHAR* pStr = NULL;
					iload->ReadCStringChunk(&pStr);
					nodeList.back().m_userPropData = pStr;
				}
				else if( iload->CurChunkID()==ACC_KEYSDATA )
				{
					nodeList.back().m_keysData.resize(0);
					ULONG len = iload->CurChunkLength();
					if( len)
					{
						nodeList.back().m_keysData.resize(len);
						iload->Read(&nodeList.back().m_keysData[0],nodeList.back().m_keysData.size(),&nRead);
					}
				}
				else if( iload->CurChunkID()==ACC_CS_ATTACH_IK )
				{
					ULONG len = iload->CurChunkLength();
					if( len == sizeof(int) )
					{
						iload->Read(&nodeList.back().m_bipBoneInfo.m_iAttachNodeIK,len,&nRead);
					}
				}
				iload->CloseChunk();
			}
		}
		iload->CloseChunk();
	}
	return res;
}

//! adds if not availaiblr
int getNodeIndex(IParamBlock2* pblock,INode* node)
{
	int curNode = pblock->Count(pb_nodeList);
	for(int n=0;n<curNode;n++)
	{
		INode* pnode = NULL;
		pblock->GetValue(pb_nodeList,0,pnode,FOREVER,n);
		if( pnode==node )
			return n;
	}
	//
	ReferenceTarget* pRefTarget = node;
	return pblock->Append(pb_nodeList,1,&pRefTarget);
}

void AnimationBank_Account::SaveToParamBlock(IParamBlock2* pblock)
{
	int curNode = pblock->Count(pb_nodeList);
	int curControl = pblock->Count(pb_controlList);
	//
	for(size_t n=0;n<nodeList.size();n++)
	{
		nodeList[n].m_dbRecord.m_nodeIndex = getNodeIndex(pblock,nodeList[n].m_node);
		nodeList[n].m_bipBoneInfo.m_iAttachNodeIK = -1;
		if( nodeList[n].m_bipBoneInfo.m_pAttachNodeIK )
		{
			nodeList[n].m_bipBoneInfo.m_iAttachNodeIK = getNodeIndex(pblock,nodeList[n].m_bipBoneInfo.m_pAttachNodeIK);
		}
		if( nodeList[n].m_control )
		{
			ReferenceTarget* pCtrl = nodeList[n].m_control;
			nodeList[n].m_dbRecord.m_controlIndex = pblock->Append(pb_controlList,1,&pCtrl);
		}
	}
}

void AnimationBank_Account::LoadFromParamBlock(IParamBlock2* pblock)
{
	int curNode = pblock->Count(pb_nodeList);
	//
	for(int n=0;n<nodeList.size();n++)
	{
		INode* nodeInList;
		pblock->GetValue(pb_nodeList,0,nodeInList,FOREVER,nodeList[n].m_dbRecord.m_nodeIndex);
		nodeList[n].m_node = nodeInList;
		if( nodeList[n].m_bipBoneInfo.m_iAttachNodeIK !=-1 )
		{
			INode* attachNodeList;
			pblock->GetValue(pb_nodeList,0,attachNodeList,FOREVER,nodeList[n].m_bipBoneInfo.m_iAttachNodeIK);
			nodeList[n].m_bipBoneInfo.m_pAttachNodeIK = attachNodeList;
		}
		if( nodeList[n].m_dbRecord.m_controlIndex!=-1 )
		{
			pblock->GetValue(pb_controlList,0,(ReferenceTarget*&)nodeList[n].m_control,FOREVER,nodeList[n].m_dbRecord.m_controlIndex);
		}
	}
}
