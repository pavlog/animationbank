#ifndef __AnimationBank__H
#define __AnimationBank__H


#define AnimationBank_CLASS_ID	Class_ID(0x1136e47c, 0x53a428dc)

enum {obj_params};

enum 
{
	pb_size,
	pb_animationList,
	pb_selectedAccount,
	pb_nodeList,
	pb_controlList,
	pb_controlListTemp,
	//pb_userPropTable,
	//pb_headersBlob,
	//pb_bipBlob,
	//pb_nodeInfoBlob,
	pb_skipNodes,
	pb_saveloadMethod,
};

class AnimationBank_AccountManager;

class AnimationBank : public HelperObject 
{
public:
	//Constructor/Destructor
	AnimationBank();
	~AnimationBank();		
public:
	//From Animatable
	Class_ID ClassID() {return AnimationBank_CLASS_ID;}		
	SClass_ID SuperClassID() { return HELPER_CLASS_ID; }
	void GetClassName(TSTR& s) {s = GetString(IDS_CLASS_NAME);}
	virtual void InitNodeName(TSTR& s) { s = GetString(IDS_CLASS_NAME); }

	RefTargetHandle Clone( RemapDir &remap );
	RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
		PartID& partID,  RefMessage message);


	void DeleteThis() { delete this; }		

	void BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev );
	void EndEditParams( IObjParam *ip, ULONG flags,Animatable *prev);

	CreateMouseCallBack* GetCreateMouseCallBack();


protected:
	// Loading/Saving
	IOResult Save(ISave *isave);
	IOResult Load(ILoad *iload);
public:
	Mesh mesh;
	Box3 mesh_box;
	void BuildMesh();
	virtual int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
	void GetWorldBoundBox(TimeValue t, INode *mat, ViewExp *vpt, Box3& box );
	void GetLocalBoundBox(TimeValue t, INode *mat, ViewExp *vpt, Box3& box );
	int HitTest(TimeValue t, INode *inode, int type, int crossing,
		int flags, IPoint2 *p, ViewExp *vpt);
	//
	ObjectState Eval(TimeValue time);
public:
	// Parameter block
	IParamBlock2	*pblock;	//ref 0
	//
	int NumSubs() { return 1; }
	TSTR SubAnimName(int i) { return GetString(IDS_PARAMS); }				
	Animatable* SubAnim(int i) { return pblock; }
	//
	int NumRefs() { return 1; }
	RefTargetHandle GetReference(int i) { return pblock; }
	void SetReference(int i, RefTargetHandle rtarg) { pblock=(IParamBlock2*)rtarg; }
	//
	int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
	IParamBlock2* GetParamBlock(int i) { return pblock; } // return i'th ParamBlock
	IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock
	//
	//
	AnimationBank_AccountManager* m_accountManager;
	//
	bool m_bRolloutActivated;
};

class AnimationBankClassDesc : public ClassDesc2 
{
public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new AnimationBank(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return HELPER_CLASS_ID; }
	Class_ID		ClassID() { return AnimationBank_CLASS_ID; }
	// The Skin modifier checks the category to decide whether the modifier is a Skin Gizmo.  This 
	// must not be changed
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("AnimationBank"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }					// returns owning module handle
};



#endif
