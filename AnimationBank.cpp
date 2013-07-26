#include "StdAfx.h"
#include "AnimationBank_Account.h"
#include "AnimationBank_AccountManager.h"
#include "AnimationBank.h"

#define PBLOCK_REF	0

static AnimationBankClassDesc AnimationBankDesc;
ClassDesc2* GetAnimationBankDesc() { return &AnimationBankDesc; }

class PSCM_Accessor : public PBAccessor
{
	void Set(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t)
	{
		AnimationBank* pBank = (AnimationBank*)owner;
		if( strlen(v.s)!=0 )
		{
			if( !pBank->m_bRolloutActivated )
			{
				pBank->m_accountManager->LoadFromParamBlock(pBank->pblock);
			}
			std::set<INode*> skipNodes;
			int count = pBank->GetParamBlock(obj_params)->Count(pb_skipNodes);
			for(int n=0;n<count;n++)
			{
				INode* node = NULL;
				pBank->GetParamBlock(obj_params)->GetValue(pb_skipNodes,0,node,FOREVER,n);
				skipNodes.insert(node);
			}
			pBank->m_accountManager->Load(v.s,skipNodes,pBank->GetParamBlock(obj_params));
		}
	}
};
static PSCM_Accessor pscm_accessor;

static ParamBlockDesc2 animationbank_param_blk ( obj_params, _T("AnimationBank"),  0, &AnimationBankDesc, P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF, 
	 //rollout
	 IDD_PANEL_OBJ, IDS_PANEL_OBJ, 0, 0, NULL, 
	 pb_size, 		_T("Icon size"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_SIZE, 
		p_default, 		0.0f, 
		p_range, 		0.0f,100000.0f, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_UNIVERSE, IDC_EDIT,	IDC_SPIN, 0.1f, 
	 end,
	 pb_nodeList, 		_T(""), 		TYPE_REFTARG_TAB, 	0,	0, 	IDS_NODES, 
	 end,
	 pb_controlList, 		_T("ControlList"), 		TYPE_REFTARG_TAB, 	0,	P_VARIABLE_SIZE, 	IDS_CONTROLS, 
	 end,
	 pb_animationList, 		_T("AccountList"), 		TYPE_STRING_TAB, 	0,	P_VARIABLE_SIZE, 	IDS_ANIMATIONSETLIST, 
	 end,
	 pb_selectedAccount, 		_T("ActiveAccount"), 		TYPE_STRING, 	0,	IDS_SELECTEDANIMATIONACCOUNT, 
		 p_accessor,		&pscm_accessor,
	 end,
	 pb_controlListTemp, 		_T("ControlListTemp"), 		TYPE_REFTARG_TAB, 	0,	P_VARIABLE_SIZE, 	IDS_CONTROLS_TEMP, 
	 end,

	 /*
	 pb_userPropTable, 		_T("UserPropTable"), 		TYPE_STRING_TAB, 	0,	P_VARIABLE_SIZE, 	IDS_USERPROPTABLE, 
	 end,

	 pb_headersBlob, 		_T("HeadersBlob"), 		TYPE_INT_TAB, 	0,	P_VARIABLE_SIZE, 	IDS_HEADERSBLOB, 
	 end,

	 pb_bipBlob, 		_T("BipBlob"), 		TYPE_INT_TAB, 	0,	P_VARIABLE_SIZE, 	IDS_BIPBLOB, 
	 end,

	 pb_nodeInfoBlob, 		_T("NodeInfoBlob"), 		TYPE_INT_TAB, 	0,	P_VARIABLE_SIZE, 	IDS_NODEINFOBLOB, 
	 end,

	 pb_mgrAccInfoBlob, 		_T("mgrAccInfo"), 		TYPE_INT_TAB, 	0,	P_VARIABLE_SIZE, 	IDS_ACCINFOBLOB, 
	 end,
	 */
	 pb_skipNodes, 		_T("SkipNodes"), 		TYPE_INODE_TAB, 	0,	P_VARIABLE_SIZE, 	IDS_SKIPNODES_CONTROLS, 
		 p_ui,			TYPE_NODELISTBOX, IDC_SKIPNODES_LIST,IDC_SKIPNODES_PICK,0,IDC_SKIPNODES_REMOVE,
	 end,

	 pb_saveloadMethod,	_T("saveloadMethod"), 		TYPE_INT, 	0,	IDS_SAVELOADMETHOOD, 
	 p_default, 		0, 
	 p_range, 		0,2, 
	 end,

	 end
	 );
// Current version
// static ParamVersionDesc curVersion(descVer0, PB_FOG_LENGTH, 0);
class AnimationBankParamDlgProc : public ParamMap2UserDlgProc 
{
public:
	AnimationBank *ob;

	AnimationBankParamDlgProc() 
	{ 
	}
	BOOL DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void DeleteThis() 
	{
		//delete this;
	}
	HWND m_hWnd;
};

BOOL CALLBACK RollupDialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam , AnimationBank* th)
{
	if ( !th && message != WM_INITDIALOG ) return FALSE;

	switch ( message ) 
	{
	case WM_INITDIALOG: 
		{
			return TRUE;
		}

	default:
		return FALSE;
	}

	return FALSE;
}



char szItemName[80]; // receives name of item to delete. 

BOOL CALLBACK NameDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
	switch (message) 
	{ 
	case WM_INITDIALOG:
		SetDlgItemText(hwndDlg, IDC_EDIT_NAME, szItemName);
		break;
	case WM_COMMAND: 
		switch (LOWORD(wParam)) 
		{ 
		case IDC_EDIT_NAME: 
			if( HIWORD(wParam)==EN_CHANGE )
			{
				if (!GetDlgItemText(hwndDlg, IDC_EDIT_NAME, szItemName, 80)) 
				{
					*szItemName=0; 
				}
				EnableWindow(GetDlgItem(hwndDlg,IDOK),strlen(szItemName)!=0);
			}
			break;
		case IDOK: 
			if (!GetDlgItemText(hwndDlg, IDC_EDIT_NAME, szItemName, 80)) 
			{
				*szItemName=0; 
			}
			// Fall through. 
		case IDCANCEL: 
			EndDialog(hwndDlg, wParam); 
			return TRUE; 
		} 
	} 
	return FALSE; 
} 

BOOL CALLBACK CopyToDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
	switch (message) 
	{ 
	case WM_INITDIALOG:
		//SetDlgItemText(hwndDlg, IDC_EDIT_NAME, szItemName);
		break;
	case WM_COMMAND: 
		switch (LOWORD(wParam)) 
		{ 
			/*
		case IDC_EDIT_NAME: 
			if( HIWORD(wParam)==EN_CHANGE )
			{
				if (!GetDlgItemText(hwndDlg, IDC_EDIT_NAME, szItemName, 80)) 
				{
					*szItemName=0; 
				}
				EnableWindow(GetDlgItem(hwndDlg,IDOK),strlen(szItemName)!=0);
			}
			break;
			*/
		case IDOK: 
//			if (!GetDlgItemText(hwndDlg, IDC_EDIT_NAME, szItemName, 80)) 
//			{
//				*szItemName=0; 
//			}
			// Fall through. 
		case IDCANCEL: 
			EndDialog(hwndDlg, wParam); 
			return TRUE; 
		} 
	} 
	return FALSE; 
} 

const char* getSelectedAccount(AnimationBank* ob)
{
#if (MAX_PRODUCT_VERSION_MAJOR != 14)
	const TCHAR* pVal = NULL;
#else
	const MCHAR* pVal = NULL;
#endif
	ob->pblock->GetValue(pb_selectedAccount,0,pVal,FOREVER);
	return pVal;
}

void UpdateComboBox(HWND hWnd,const char* selString,AnimationBank* ob)
{
	HWND hComboList = GetDlgItem(hWnd,IDC_COMBO_LIST);
	//
	SendMessage(hComboList,CB_RESETCONTENT,0,0);
	//
	const AnimationBank_AccountManager::t_accountList& accMap = ob->m_accountManager->getAccountMap();
	AnimationBank_AccountManager::t_accountList::const_iterator it = accMap.begin();
	for(;it!=accMap.end();it++)
	{
		SendMessage(hComboList,CB_ADDSTRING,0,(LPARAM)it->first.c_str());
	}
	//
	int selIndex = CB_ERR;
	if( selString )
	{
		int index = SendMessage(hComboList,CB_FINDSTRINGEXACT,-1,(LPARAM)selString);
		selIndex = SendMessage(hComboList,CB_SETCURSEL,index,0);
	}
	else
	{
		selIndex = SendMessage(hComboList,CB_SETCURSEL,CB_ERR,0);
	}
	// rename button
	EnableWindow(GetDlgItem(hWnd,IDC_BUTTON_RENAME),selIndex!=CB_ERR);
	// update button
	EnableWindow(GetDlgItem(hWnd,IDC_BUTTON_UPDATE),selIndex!=CB_ERR);
	// delete button
	EnableWindow(GetDlgItem(hWnd,IDC_BUTTON_DELETE),selIndex!=CB_ERR);
	// 
	{
		HWND hComboList = GetDlgItem(hWnd,IDC_COMBO_METHOD);
		EnableWindow(hComboList,accMap.size()==0);
	}
}

static AnimationBankParamDlgProc dlgProc;

//
//
//
//static HWND hAnimationBankWnd = NULL;
//static bool bPosInitialized = false;
//static WINDOWPLACEMENT AnimationBankDlgPlacement;
static BOOL bWorkInProgress = false;
//
//
//
static INode* beginWorkSelNode = NULL;
void BeginWork()
{
	bWorkInProgress = true;
	//
	theHold.SuperBegin();
	GetCOREInterface()->DisableSceneRedraw();
	beginWorkSelNode = GetCOREInterface()->GetSelNodeCount() ? GetCOREInterface()->GetSelNode(0) : NULL;
	TCHAR* name = beginWorkSelNode->GetName();
	int y=0;
}

void EndWork(TCHAR* name)
{
	//
	GetCOREInterface()->EnableSceneRedraw();
	//
	if( beginWorkSelNode )
	{
		TCHAR* name = beginWorkSelNode->GetName();
		theHold.Begin();
		GetCOREInterface()->SelectNode(beginWorkSelNode);
		theHold.Accept("Select");
		beginWorkSelNode = NULL;
	}
	//
	//
	theHold.SuperAccept(name);
	//
	bWorkInProgress = false;
	//
}


void New(HWND hWnd,AnimationBank* ob)
{
	BeginWork();
	//
	IParamBlock2* pblock = ob->pblock;
	//
	int curSel = SendMessage(GetDlgItem(hWnd,IDC_COMBO_LIST),CB_GETCURSEL,0,0);
	bool bChanged = false;
	//
	*szItemName=0; 
	INT_PTR dlgRes = DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG_NAME),GetCOREInterface()->GetMAXHWnd(),NameDlgProc);
	if( dlgRes==IDOK )
	{
		if(ob->m_accountManager->isExist(szItemName))
		{
			char buffer[1024];
			sprintf(buffer,"Account with name:%s already exist.\nOverwrite?",szItemName);
			dlgRes = MessageBox(GetCOREInterface()->GetMAXHWnd(),buffer,"Warning",MB_ICONQUESTION | MB_YESNO);
			if( dlgRes==IDYES )
			{
				ob->m_accountManager->Create(szItemName,ob->pblock);
				bChanged = true;
			}
		}
		else
		{
			ob->m_accountManager->Create(szItemName,ob->pblock);
			bChanged = true;
		}
	}
	//
	if( bChanged )
	{
		pblock->SetValue(pb_selectedAccount,0,szItemName);
		UpdateComboBox(hWnd,szItemName,ob);
	}
	//
	EndWork("NewAccount");
}

void Select(HWND hWnd,AnimationBank* ob)
{
	BeginWork();
	//
	int curSel = SendMessage(GetDlgItem(hWnd,IDC_COMBO_LIST),CB_GETCURSEL,0,0);
	if( curSel!=CB_ERR )
	{
		char buffer[1024];
		SendMessage(GetDlgItem(hWnd,IDC_COMBO_LIST),CB_GETLBTEXT,curSel,(LPARAM)buffer);
		//
		bool bReload = true;
		// save current account
		const char* pStr = getSelectedAccount(ob);
		if( pStr && strlen(pStr)!=0 )
		{
			int dlgRes = MessageBox(GetCOREInterface()->GetMAXHWnd(),"Save current account?","Warning",MB_ICONQUESTION | MB_YESNO);
			if( dlgRes==IDYES )
			{
				ob->m_accountManager->Create(pStr,ob->pblock);
			}
		}
		//
		if( stricmp(buffer,getSelectedAccount(ob))==0 )
		{
			if( MessageBox(GetCOREInterface()->GetMAXHWnd(),"The same account was selected.\nChanges(if was) from last account activation will not be saved.\nDo you want to reload account?","Warning",MB_YESNO)==IDYES )
			{
				bReload = true;
			}
			else
			{
				bReload = false;
			}
		}
		else
		{
			bReload = true;
		}
		//
		if( bReload )
		{
			//							ob->m_accountManager->Load(buffer);
			ob->pblock->SetValue(pb_selectedAccount,0,buffer);
		}
	}
	//
	EndWork("SelectAccount");
}

/*
void SetAnimationBankDialogPos()
{
	if (bPosInitialized!=false) 
	{
		HWND maxHWND = GetCOREInterface()->GetMAXHWnd();
		SetWindowPlacement(hAnimationBankWnd,&AnimationBankDlgPlacement);
	}
}

void SaveAnimationBankDialog()
{
	AnimationBankDlgPlacement.length = sizeof(WINDOWPLACEMENT); 
	GetWindowPlacement(hAnimationBankWnd,&AnimationBankDlgPlacement);
	bPosInitialized = true;
}
//Windows proc to control  the Weight Table Window
INT_PTR CALLBACK AnimationBankDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	AnimationBank *ob = (AnimationBank*)GetWindowLongPtr(hWnd,GWLP_USERDATA);
	switch (msg) 
	{
	case WM_INITDIALOG:
		{
			//initialized custom control GLP_USERDATE to point to the owning skin mod for each custom window
			ob = (AnimationBank*)lParam;
			SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)ob);
			//
			GetCOREInterface()->RegisterDlgWnd(hWnd);
			//
			dlgProc.m_hWnd = hWnd;
			//
			UpdateComboBox(hWnd,getSelectedAccount(ob),ob);
			{
				HWND hComboList = GetDlgItem(hWnd,IDC_COMBO_METHOD);
				//
				SendMessage(hComboList,CB_RESETCONTENT,0,0);
				//
				SendMessage(hComboList,CB_ADDSTRING,0,(LPARAM)"Controls");
				SendMessage(hComboList,CB_ADDSTRING,0,(LPARAM)"Keys");
				//
				int aword;
				ob->pblock->GetValue(pb_saveloadMethod,0,aword,FOREVER,0);
				SendMessage(hComboList,CB_SETCURSEL,aword,0);
				//
			}
		}
		break;
	case WM_DESTROY:
		SaveAnimationBankDialog();
		hAnimationBankWnd = NULL;
		GetCOREInterface()->UnRegisterDlgWnd(hWnd);
		EndDialog(hWnd, 0);
		break;
	default:
		return FALSE;//DefDlgProc(hWnd,msg,wParam,lParam);
	}
	return TRUE;
}


void ShowDialog(AnimationBank* ob)
{
	if (hAnimationBankWnd == NULL)
	{
		HWND hWndMax = GetCOREInterface()->GetMAXHWnd();
		hAnimationBankWnd = CreateDialogParam(hInstance,MAKEINTRESOURCE(IDD_PANEL_OBJ),
			hWndMax,AnimationBankDlgProc,(LPARAM)ob);
		SetAnimationBankDialogPos();
		ShowWindow(hAnimationBankWnd,SW_SHOW);
	}
}

void HideDialog()
{
	if (hAnimationBankWnd != NULL)
	{
		DestroyWindow(hAnimationBankWnd);
	}
}
*/


static BOOL TRUEVAL = TRUE;

BOOL AnimationBankParamDlgProc::DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
	{
	case WM_INITDIALOG:
		{
			//
			dlgProc.m_hWnd = hWnd;
			//
			UpdateComboBox(hWnd,getSelectedAccount(ob),ob);
			{
				HWND hComboList = GetDlgItem(hWnd,IDC_COMBO_METHOD);
				//
				SendMessage(hComboList,CB_RESETCONTENT,0,0);
				//
				SendMessage(hComboList,CB_ADDSTRING,0,(LPARAM)"Controls");
				SendMessage(hComboList,CB_ADDSTRING,0,(LPARAM)"Keys");
				//
				int aword;
				ob->pblock->GetValue(pb_saveloadMethod,0,aword,FOREVER,0);
				SendMessage(hComboList,CB_SETCURSEL,aword,0);
				//
			}
		}
		break;
	case WM_DESTROY:
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
			/*
			case IDC_BUTTON_COPY:
			{
			int word = 0xffea14ff;
			DWORD word1 = (DWORD)word;
			TimeValue* pData = (int*)&word;
			ob->pblock->Append(pb_bipBlob,1,pData);
			//
			int aword;
			ob->pblock->GetValue(pb_bipBlob,0,aword,FOREVER,0);
			DWORD aword1 = (DWORD)aword;
			int hh=0;
			}
			break;
			*/
			//		case IDC_BUTTON_PASTE:
			//			{
			//				track.Load();
			//			}
			//			break;
		case IDC_COMBO_LIST:
			{
				if( HIWORD(wParam)==CBN_SELCHANGE )
				{
					Select(hWnd,ob);
					return TRUEVAL;
				}
			}
			break;
		case IDC_COMBO_METHOD:
			{
				if( HIWORD(wParam)==CBN_SELCHANGE )
				{
					int curSel = SendMessage(GetDlgItem(hWnd,IDC_COMBO_METHOD),CB_GETCURSEL,0,0);
					ob->pblock->SetValue(pb_saveloadMethod,0,curSel);
				}
			}
			break;
		case IDC_BUTTON_NEW:
			{
				if( HIWORD(wParam)==BN_CLICKED )
				{
					New(hWnd,ob);
					return TRUEVAL;
				}
			}
			break;
		case IDC_BUTTON_NORMALIZE:
			{
				if( HIWORD(wParam)==BN_CLICKED )
				{
					IParamBlock2* pblock = ob->pblock;
					//
					// first remove deleted
					bool bChanged = false;
					do
					{
						bChanged = false;
						int count = pblock->Count(pb_skipNodes);
						for(int n=0;n<count;n++)
						{
							INode* node = NULL;
							pblock->GetValue(pb_skipNodes,0,node,FOREVER,n);
							if( node==NULL )
							{
								bChanged = true;
								pblock->Delete(pb_skipNodes,n,1);
								break;
							}
						}
					}
					while(bChanged);
					// first remove duplicates
					bChanged = false;
					do
					{
						bChanged = false;
						int count = pblock->Count(pb_skipNodes);
						for(int n=0;n<count;n++)
						{
							INode* node = NULL;
							pblock->GetValue(pb_skipNodes,0,node,FOREVER,n);
							for(int n1=n+1;n1<count;n1++)
							{
								INode* node1 = NULL;
								pblock->GetValue(pb_skipNodes,0,node1,FOREVER,n1);
								if( node==node1 )
								{
									bChanged = true;
									pblock->Delete(pb_skipNodes,n1,1);
									break;
								}
							}
						}
					}
					while(bChanged);
					//
					map->Invalidate(pb_skipNodes);
					//
					return TRUEVAL;
				}
			}
			break;
		case IDC_BUTTON_RENAME:
			{
				if( HIWORD(wParam)==BN_CLICKED )
				{
					BeginWork();
					{
						IParamBlock2* pblock = ob->pblock;
						//
						std::string OldName = getSelectedAccount(ob);
						int dlgRes = MessageBox(GetCOREInterface()->GetMAXHWnd(),"Save current account?","Warning",MB_ICONQUESTION | MB_YESNO);
						if( dlgRes==IDYES )
						{
							ob->m_accountManager->Create(szItemName,ob->pblock);
						}
						strcpy(szItemName,OldName.c_str());
						//
						bool bChanged = false;
						dlgRes = DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG_NAME),GetCOREInterface()->GetMAXHWnd(),NameDlgProc);
						if( dlgRes==IDOK )
						{
							if( stricmp(szItemName,OldName.c_str())!=0 )
							{
								if(ob->m_accountManager->isExist(szItemName))
								{
									char buffer[1024];
									sprintf(buffer,"Account with name:%s already exist.\nOverwrite existing?",szItemName);
									dlgRes = MessageBox(GetCOREInterface()->GetMAXHWnd(),buffer,"Warning",MB_ICONQUESTION | MB_YESNO);
									if( dlgRes==IDYES )
									{
										ob->m_accountManager->Rename(OldName.c_str(),szItemName,ob->pblock);
										bChanged = true;
									}
								}
								else
								{
									ob->m_accountManager->Rename(OldName.c_str(),szItemName,ob->pblock);
									bChanged = true;
								}
							}
						}
						//
						if( bChanged )
						{
							pblock->SetValue(pb_selectedAccount,0,szItemName);
							UpdateComboBox(this->m_hWnd,szItemName,ob);
						}
					}
					EndWork("RenameAccount");
				}
			}
			break;
		case IDC_BUTTON_UPDATE:
			{
				if( HIWORD(wParam)==BN_CLICKED )
				{
					BeginWork();
					//
					{
						std::string OldName = getSelectedAccount(ob);
						ob->m_accountManager->Create(OldName.c_str(),ob->pblock);
					}
					//
					EndWork("UpdateAccount");
				}
			}
			break;
		case IDC_BUTTON_DELETE:
			{
				if( HIWORD(wParam)==BN_CLICKED )
				{
					std::string OldName = getSelectedAccount(ob);
					char buffer[1024];
					sprintf(buffer,"Are you you want to delete Account with name:%s?",OldName.c_str());
					int dlgRes = MessageBox(GetCOREInterface()->GetMAXHWnd(),buffer,"Warning",MB_ICONQUESTION | MB_OKCANCEL);
					if( dlgRes==IDCANCEL )
					{

					}
					else if( dlgRes==IDOK )
					{
						BeginWork();
						{
							ob->m_accountManager->Delete(OldName.c_str());
							ob->pblock->SetValue(pb_selectedAccount,0,"");
							UpdateComboBox(this->m_hWnd,NULL,ob);
						}
						EndWork("RenameAccount");
					}
				}
			}
			break;
		case IDC_BUTTON_COPYTO:
			{
				if( HIWORD(wParam)==BN_CLICKED )
				{
					BeginWork();
					{
						int dlgRes = DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG_COPYTO),GetCOREInterface()->GetMAXHWnd(),CopyToDlgProc);
						if( dlgRes==IDOK )
						{
							IParamBlock2* pblock = ob->pblock;
						}
					}
					EndWork("CopyToAccount");
				}
			}
			break;
		}
		break;
	default:
		return RollupDialogProc( hWnd, msg, wParam, lParam, ob );
	}
	return RollupDialogProc( hWnd, msg, wParam, lParam, ob );
}

/*
 void NOTIFYPROC_LOAD(void *param, NotifyInfo *info)
 {

 }
*/
AnimationBank::AnimationBank()
{
	m_bRolloutActivated = false;
	m_accountManager = new AnimationBank_AccountManager();
	BuildMesh();
	pblock = 0;
	AnimationBankDesc.MakeAutoParamBlocks(this);

}

AnimationBank::~AnimationBank()
{
	if( m_accountManager )
		delete m_accountManager;
}

void AnimationBank::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
{
	if( !bWorkInProgress )
	{
	//
	m_accountManager->LoadFromParamBlock(this->pblock);
	//
	m_bRolloutActivated = true;
	// throw up all the appropriate auto-rollouts
	AnimationBankDesc.BeginEditParams(ip, this, flags, prev);
	//
	//static AnimationBankParamDlgProc proc;
	//proc.ob = proc;
	dlgProc.ob = this;
	dlgProc.m_hWnd = NULL;
	animationbank_param_blk.SetUserDlgProc(&dlgProc);
	//ShowDialog(this);
	}
}

void AnimationBank::EndEditParams( IObjParam *ip, ULONG flags,Animatable *prev)
{
	dlgProc.m_hWnd = NULL;
	//
	m_bRolloutActivated = false;
	// tear down the appropriate auto-rollouts
	if( !bWorkInProgress )
	{
		AnimationBankDesc.EndEditParams(ip, this, flags, prev);
		ClearAFlag(A_OBJ_CREATING);
		//HideDialog();
	}
}


IOResult AnimationBank::Save(ISave *isave)
{
	return m_accountManager->SaveAccount(isave);
}

IOResult AnimationBank::Load(ILoad *iload)
{
	return m_accountManager->LoadAccount(iload);
}

class AnimationBankHelperCreateCallBack : public CreateMouseCallBack 
{
private:
	IPoint2 sp0;
	Point3 p0;
	AnimationBank *m_obj;

public:
	int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	void SetObj(AnimationBank *obj)
	{
		m_obj = obj;
	}

};


int AnimationBankHelperCreateCallBack::proc(ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat)
{	
	Point3 p1,center;

	switch ( msg ) 
	{
	case MOUSE_POINT:
	case MOUSE_MOVE:
		switch ( point ) {
	case 0:  // only happens with MOUSE_POINT msg
		sp0 = m;
		p0 = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
		mat.SetTrans(p0);
		break;
	case 1:
		mat.IdentityMatrix();
		p1 = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
		mat.SetTrans(p0);
		float radius = Length(p1-p0);
		m_obj->pblock->SetValue(pb_size, 0, radius);
		animationbank_param_blk.InvalidateUI();										
		if (msg==MOUSE_POINT) {
			return (Length(m-sp0)<3)?CREATE_ABORT:CREATE_STOP;
		}
		break;					   
		}			
		break;
	case MOUSE_ABORT:
		return CREATE_ABORT;
	}

	return TRUE;
}

// A single instance of the callback object.
static AnimationBankHelperCreateCallBack theAnimationBankHelperCreateCallBack;

// This method allows MAX to access and call our proc method to 
// handle the user input.
CreateMouseCallBack* AnimationBank::GetCreateMouseCallBack() 
{
	theAnimationBankHelperCreateCallBack.SetObj(this);
	return(&theAnimationBankHelperCreateCallBack);
}


ObjectState
AnimationBank::Eval(TimeValue time)
{
	return ObjectState(this);
}

// This is only called if the object MAKES references to other things.
RefResult
AnimationBank::NotifyRefChanged(Interval changeInt,
									RefTargetHandle hTarget, PartID& partID,
									RefMessage message ) 
{
	//     int i;
	//     switch (message) {
	//     }
	return REF_SUCCEED;
}

RefTargetHandle AnimationBank::Clone(RemapDir& remap)
{
	AnimationBank* ni = new AnimationBank();
	ni->ReplaceReference(0, pblock->Clone(remap));
	BaseClone(this, ni, remap);
	return ni;
}
