#pragma once
struct SDKMESH_CALLBACKS10;
class IMesh // Interface class to toggle between SDK MEsh & .X meshes 
{
public:	
	virtual HRESULT Create( ID3D10Device *pDev10, LPCTSTR szFileName, D3D10_INPUT_ELEMENT_DESC* playout, UINT cElements, bool bOptimize=true ) {return S_OK;}
	virtual HRESULT Create( ID3D10Device *pDev10, LPCTSTR szFileName, bool bOptimize=true, bool bCreateAdjacencyIndices=false, SDKMESH_CALLBACKS10* pLoaderCallbacks=NULL ){return S_OK;}
	     //Direct3D 10 Rendering
		virtual void Render( ID3D10Device *pd3dDevice, 
		ID3D10EffectTechnique* pTechnique, 
		ID3D10EffectShaderResourceVariable* ptxDiffuse = NULL,
		ID3D10EffectShaderResourceVariable* ptxNormal = NULL,
		ID3D10EffectShaderResourceVariable* ptxSpecular = NULL,
		ID3D10EffectVectorVariable* pvDiffuse = NULL, 
		ID3D10EffectVectorVariable* pvSpecular = NULL ) = 0;
	virtual void	Destroy		( ) = 0;

};
