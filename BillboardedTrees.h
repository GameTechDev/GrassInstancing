/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Billboard.h"
#include "InstancedBillboard.h"
struct SimpleVertex;

#define MAX_TREE_TEX 3
#define DEFAULT_NUM_TREES  10

// TODO float HeightField( FLOAT x, FLOAT y );

struct TREE_INSTANCE_DATA
{
	D3DXVECTOR3 Pos;
	D3DXVECTOR2 scale;
};
class BillboardedTrees : public InstancedBillboard <SimpleVertex, TREE_INSTANCE_DATA> 
{

public: 

	int									m_treeCnt;
	SimpleVertex*						m_pTreeVerts;
	TREE_INSTANCE_DATA*					m_pTreeData;

	// tree orientation
	D3DXMATRIX							m_TreeOrientMatrix; 
	D3DXVECTOR3							m_Orientation;
	ID3D10EffectMatrixVariable*			m_pfxOrientation;

	ID3D10EffectTechnique*              m_pRenderTechnique;


	HRESULT		Initialize    ( D3DXVECTOR2 offPos, int numTrees = DEFAULT_NUM_TREES, float ht = 30.0f, 
								float wd = 16.0f, float spread = 30.0f);
	
	HRESULT		CreateDevice  ( ID3D10Device* m_pd3dDevice, ID3D10Effect* m_pEffect );
	
	void		DestroyDevice ( );
	
	void		Destroy		  ( );
	
	void		Render		  ( ID3D10Device* m_pd3dDevice, ID3D10EffectShaderResourceVariable* m_pDiffuseVariable );

	void		OrientTreesTo ( D3DXVECTOR3);
	

};
