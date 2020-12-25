/**************************
 file name: SkyBox.h

*/

#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#pragma once

struct SkyBoxVertex
{
	float x, y, z;	//vertex position 
	float u, v;		//texture position
};

class SkyBox
{
	public:
		SkyBox();
		~SkyBox();

		bool InitialSkyBox(float length);
		bool InitialSkyBoxTexture();
};

#endif 
