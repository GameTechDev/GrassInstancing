/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

// the intersection function is based on Sphere Cone intersection test described in Geometric tools 
// by Philip J. Schneider and David H. Eberly, 
// The Morgan Kaufmann Series in Computer Graphics and Geometric Modeling
// http://www.geometrictools.com/Documentation/IntersectionSphereCone.pdf


#include "DXUT.h"
BOOL SphereConeTest ( D3DXVECTOR3 sphereCenter, float radius, float fov, D3DXVECTOR3 eyePt, D3DXVECTOR3 lookAt)
{
	float fSin = sinf(fov);
	float fCos = cosf(fov);
	float fInvSin = 1.0f/fSin;
	float fCosSqr = fCos*fCos;
	float fSinSqr = fSin*fSin;
	D3DXVECTOR3 vAxis = lookAt - eyePt;

	D3DXVECTOR3 vCmV =  sphereCenter - eyePt;
	D3DXVECTOR3 vD = vCmV + (radius*fInvSin)*vAxis;
	float fDSqrLen = D3DXVec3Dot(&vD, &vD);
	float fE = D3DXVec3Dot(&vD,&vAxis);
	if (fE > 0.0f && fE*fE >= fDSqrLen*fCosSqr)
	{
		fDSqrLen = D3DXVec3Dot(&vCmV, &vCmV);
		fE = -D3DXVec3Dot(&vCmV, &vAxis);
		if (fE > 0.0f && fE*fE >= fDSqrLen*fSinSqr)
		{
			float fRSqr = radius*radius;
			return fDSqrLen <= fRSqr;
		}
		return TRUE;
	}
	return FALSE;
}

