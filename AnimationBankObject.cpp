#include "StdAfx.h"
#include "AnimationBank.h"
namespace { 
	namespace { 
		float vertices[]= { 
			-0.224220f,-0.306578f,-0.000050f,-0.177534f,-0.500753f,-0.000050f,-0.021922f,-0.399603f,-0.000050f,-0.255354f,0.549623f,-0.000050f,-0.395414f,0.565175f,-0.000050f,-0.784459f,-0.431317f,-0.000050f,-0.605489f,-0.478033f,-0.000050f,-0.512116f,-0.151227f,-0.000050f,-0.465459f,0.019967f,-0.000050f,-0.379862f,0.315639f,-0.000050f,-0.270936f,-0.120093f,-0.000050f,0.053056f,-0.369833f,-0.000050f,0.255354f,-0.510038f,-0.000050f,0.729996f,-0.120586f,-0.000050f,0.535474f,0.089692f,-0.000050f,0.784459f,0.440204f,-0.000050f,0.130876f,0.572604f,-0.000050f,0.084190f,0.378052f,-0.000050f,0.504340f,0.331336f,-0.000050f,0.302041f,-0.011080f,-0.000050f,0.481011f,-0.127840f,-0.000050f,0.270907f,-0.314616f,-0.000050f,0.216444f,-0.213466f,-0.000050f,0.286488f,0.276728f,-0.000050f,0.099742f,0.276728f,-0.000050f,-0.000000f,1.115426f,0.000000f,-0.000000f,1.115426f,0.156551f,-0.000000f,0.960327f,0.156551f,-0.000000f,0.960327f,0.000000f,0.557713f,0.965987f,0.000000f,
			0.557713f,0.965987f,0.156551f,0.480163f,0.831668f,0.156551f,0.480163f,0.831668f,0.000000f,0.965987f,0.557713f,0.000000f,0.965987f,0.557713f,0.156551f,0.831668f,0.480164f,0.156551f,0.831668f,0.480164f,0.000000f,1.115426f,0.000000f,0.000000f,1.115426f,0.000000f,0.156551f,0.960327f,0.000000f,0.156551f,0.960327f,0.000000f,0.000000f,0.965987f,-0.557713f,0.000000f,0.965987f,-0.557713f,0.156551f,0.831668f,-0.480163f,0.156551f,0.831668f,-0.480163f,0.000000f,0.557713f,-0.965987f,0.000000f,0.557713f,-0.965987f,0.156551f,0.480163f,-0.831668f,0.156551f,0.480163f,-0.831668f,0.000000f,-0.000000f,-1.115426f,0.000000f,-0.000000f,-1.115426f,0.156551f,-0.000000f,-0.960327f,0.156551f,-0.000000f,-0.960327f,0.000000f,-0.557713f,-0.965987f,0.000000f,-0.557713f,-0.965987f,0.156551f,-0.480164f,-0.831668f,0.156551f,-0.480164f,-0.831668f,0.000000f,-0.965987f,-0.557713f,0.000000f,-0.965987f,-0.557713f,0.156551f,-0.831668f,-0.480163f,0.156551f,
			-0.831668f,-0.480163f,0.000000f,-1.115426f,0.000000f,0.000000f,-1.115426f,0.000000f,0.156551f,-0.960327f,0.000000f,0.156551f,-0.960327f,0.000000f,0.000000f,-0.965987f,0.557713f,0.000000f,-0.965987f,0.557713f,0.156551f,-0.831667f,0.480164f,0.156551f,-0.831667f,0.480164f,0.000000f,-0.557713f,0.965987f,0.000000f,-0.557713f,0.965987f,0.156551f,-0.480163f,0.831668f,0.156551f,-0.480163f,0.831668f,0.000000f,};
			float triangles[]= { 
				0,1,2,0,2,3,5,8,9,4,5,9,3,4,9,3,9,10,0,3,10,0,10,8,5,6,7,8,5,7,0,8,7,22,23,24,22,24,11,16,17,18,15,16,18,14,15,18,14,18,19,14,19,20,13,14,20,13,20,21,12,13,21,11,12,21,22,11,21,25,26,30,30,29,25,26,27,31,31,30,26,27,28,32,32,31,27,28,25,29,29,32,28,29,30,34,34,33,29,30,31,35,35,34,30,31,32,36,36,35,31,32,29,33,33,36,32,33,34,38,38,37,33,34,35,39,39,38,34,35,36,40,40,39,35,36,33,37,37,40,36,37,38,42,42,41,37,38,39,43,43,42,38,39,40,44,44,43,39,40,37,41,41,44,40,41,42,46,46,45,41,42,43,47,47,46,42,43,44,48,
				48,47,43,44,41,45,45,48,44,45,46,50,50,49,45,46,47,51,51,50,46,47,48,52,52,51,47,48,45,49,49,52,48,49,50,54,54,53,49,50,51,55,55,54,50,51,52,56,56,55,51,52,49,53,53,56,52,53,54,58,58,57,53,54,55,59,59,58,54,55,56,60,60,59,55,56,53,57,57,60,56,57,58,62,62,61,57,58,59,63,63,62,58,59,60,64,64,63,59,60,57,61,61,64,60,61,62,66,66,65,61,62,63,67,67,66,62,63,64,68,68,67,63,64,61,65,65,68,64,65,66,70,70,69,65,66,67,71,71,70,66,67,68,72,72,71,67,68,65,69,69,72,68,69,70,26,26,25,69,70,71,27,27,26,70,71,72,28,28,27,71,72,69,25,25,28,72,};
				int faceflags[]= { 
					3,2,2,1,1,2,0,2,3,0,4,3,2,3,1,1,2,2,1,2,1,1,4,131075,131075,3,3,196611,196611,65539,65539,131075,131075,3,3,196611,196611,65539,65539,131075,131075,3,3,196611,196611,65539,65539,131075,131075,3,3,196611,196611,65539,65539,131075,131075,3,3,196611,
					196611,65539,65539,131075,131075,3,3,196611,196611,65539,65539,131075,131075,3,3,196611,196611,65539,65539,131075,131075,3,3,196611,196611,65539,65539,131075,131075,3,3,196611,196611,65539,65539,131075,131075,3,3,196611,196611,65539,65539,131075,131075,3,3,196611,196611,65539,65539,131075,131075,3,3,196611,196611,65539,65539,};
	}; //namespace  
}; //namespace  

void AnimationBank::BuildMesh()
{
	float size = 1.0f;
	mesh.setNumVerts(73);
	mesh.setNumFaces(119);
	for(int v=0;v<mesh.getNumVerts();v++)
	{
		mesh.setVert(v, size * Point3(vertices[v*3+0],vertices[v*3+1],vertices[v*3+2]));
	}
	for(int f=0;f<mesh.getNumFaces();f++)
	{
		mesh.faces[f].setVerts(triangles[f*3+0],triangles[f*3+1],triangles[f*3+2]);
		mesh.faces[f].flags = faceflags[f];
		mesh.faces[f].setSmGroup(1);
	}
	mesh_box = mesh.getBoundingBox();
}
int AnimationBank::Display(TimeValue t, INode* inode, ViewExp *vpt, int flags)
{
	float radius;
	pblock->GetValue(pb_size, t, radius, FOREVER);
	if (radius <= 0.0)
		return 0;
	//
	Matrix3 tm = inode->GetObjectTM(t);
	tm.PreScale(Point3(radius,radius,radius));
	//
	GraphicsWindow *gw = vpt->getGW();
	Material *mtl = gw->getMaterial();
	DWORD rlim = gw->getRndLimits();
	gw->setRndLimits(GW_WIREFRAME|GW_EDGES_ONLY|GW_BACKCULL);
	gw->setTransform(tm);
	// color
	if (inode->Selected()) 
	{
		gw->setColor( LINE_COLOR, 1.0f, 1.0f, 1.0f);
	}
	else if(!inode->IsFrozen())
	{
		gw->setColor( LINE_COLOR, 0.5f, 0.5f, 1.0f);
	}
	// render
	mesh.render( gw, mtl, NULL, COMP_ALL);
	gw->setRndLimits(rlim);
	return(0);
}

void AnimationBank::GetLocalBoundBox(TimeValue t, INode* inode, ViewExp* vpt,
									Box3& box )
{
	float radius;
	pblock->GetValue(pb_size, t, radius, FOREVER);
	//
	box = mesh_box;
	box.pmin*=radius;
	box.pmax*=radius;
}

void AnimationBank::GetWorldBoundBox(TimeValue t, INode* inode, ViewExp* vpt,
									Box3& box )
{
	float radius;
	pblock->GetValue(pb_size, t, radius, FOREVER);
	//
	Matrix3 tm = inode->GetObjectTM(t);
	tm.PreScale(Point3(radius,radius,radius));
	//
	int nv = mesh.getNumVerts();
	box.Init();
	for (int i=0; i<nv; i++) 
	{
		box += tm*mesh.getVert(i);
	}
}

int AnimationBank::HitTest(TimeValue t, INode *inode, int type, int crossing,
						   int flags, IPoint2 *p, ViewExp *vpt)
{
	HitRegion hitRegion;
	int res = FALSE;
	//
	GraphicsWindow *gw = vpt->getGW();	
	Material *mtl = gw->getMaterial();
	MakeHitRegion(hitRegion,type,crossing,4,p);	
	DWORD	savedLimits= gw->getRndLimits();
	gw->setRndLimits(GW_WIREFRAME|GW_EDGES_ONLY|GW_BACKCULL|GW_PICK);
	//
	float radius;
	pblock->GetValue(pb_size, t, radius, FOREVER);
	Matrix3 tm = inode->GetObjectTM(t);
	tm.PreScale(Point3(radius,radius,radius));
	gw->setTransform(tm);
	//
	gw->clearHitCode();
	if (mesh.select( gw, mtl, &hitRegion, flags & HIT_ABORTONHIT )) 
		return TRUE;
	gw->setRndLimits(savedLimits);
	return res;
}
