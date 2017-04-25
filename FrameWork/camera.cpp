/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

#include "DXUT.h"
#include "camera.h"

camera::camera()
{
	position = D3DXVECTOR3(0,0,0);
	fov = 45;
	aspect = 1.0f;
	znear = 0.1f;
	zfar = 10;
	rotx = 0;
	roty = 0;
	rotz = 0;
	this->update();
}

camera::camera(D3DXVECTOR3 pos, float rotx, float roty, float rotz, float fov, float aspect, float nearz, float farz)
{
	this->position = pos;
	this->fov = fov;
	this->aspect = aspect;
	this->znear = nearz;
	this->zfar = farz;
	this->rotx = rotx;
	this->roty = roty;
	this->rotz = rotz;
	this->update();
}

camera::camera(const camera *src)
{
	this->position	= src->position;
	this->fov		= src->fov;
	this->aspect	= src->aspect;
	this->znear		= src->znear;
	this->zfar		= src->zfar;
	this->rotx		= src->rotx;
	this->roty		= src->roty;
	this->rotz		= src->rotz;
	this->update();
}

camera::~camera()
{

}

void camera::update()
{
	D3DXMATRIXA16 rotatex,rotatey,rotatez,translation;
	
	// perspective matrix
	D3DXMatrixPerspectiveFovLH(&proj, fov, aspect, znear, zfar);
	// view matrix
	D3DXMatrixRotationX(&rotatex,rotx);
	D3DXMatrixRotationY(&rotatey,roty);
	D3DXMatrixRotationZ(&rotatez,rotz);
	D3DXMatrixTranslation(&translation,-position.x,-position.y,-position.z);
	view = translation * rotatey*rotatex*rotatez;
	// and finally the combined viewproj
	viewproj = view*proj;
	// and all the inverses
	D3DXMatrixInverse(&invproj,NULL,&proj);
	D3DXMatrixInverse(&invview,NULL,&view);
	D3DXMatrixInverse(&invviewproj,NULL,&viewproj);
	// and the direction vectors
	D3DXVECTOR3 tmp = D3DXVECTOR3(0,0,1); // forward vector
	D3DXVec3TransformNormal(&forward,&tmp,&invview);
	D3DXVec3Normalize( &forward, &forward );
	tmp.y = 1; tmp.z = 0; // up vector
	D3DXVec3TransformNormal(&up,&tmp,&invview);
	tmp.x = 1; tmp.y = 0; //right vector
	D3DXVec3TransformNormal(&right,&tmp,&invview);
}

// set all parameters assuming position, forward & all the perspective shit are correct
void camera::update_lookat()
{
	// perspective matrix
	D3DXMatrixPerspectiveFovLH(&proj, fov, aspect, znear, zfar);
	// view matrix
	D3DXVECTOR3 tmpUp = D3DXVECTOR3(0,1,0); //right
	lookAt = position+forward;
	D3DXMatrixLookAtLH( &view, &position,&lookAt, &tmpUp);

	// and finally the combined viewproj
	viewproj = view*proj;
	// and all the inverses
	D3DXMatrixInverse(&invproj,NULL,&proj);
	D3DXMatrixInverse(&invview,NULL,&view);
	D3DXMatrixInverse(&invviewproj,NULL,&viewproj);
	D3DXVECTOR3 tmpRight = D3DXVECTOR3(1,0,0); //right
	D3DXVec3TransformNormal(&right,&tmpRight ,&invview);
}

