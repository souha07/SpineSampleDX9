//**************************************************************************************************************************************************************
//		�C���N���[�h�t�@�C��
//**************************************************************************************************************************************************************
#include "StdAfx.h"
#include <windows.h>
#include <memory>
#include <vector>

#include <d3d9.h>

#include <spine/spine.h>
#include <spine/Extension.h>

//#include "CDX9TextureLoader.h"
#include "Utility.h"


//**************************************************************************************************************************************************************
//		�O���[�o���ϐ�
//**************************************************************************************************************************************************************


//**************************************************************************************************************************************************************
//		�֐���`
//**************************************************************************************************************************************************************
std::shared_ptr<spine::SkeletonData>	ReadSkeletonJsonData( const spine::String &filename, spine::Atlas *atlas, float scale );


void									InitSpine();
void									SkeletonUpdate( spine::Skeleton* skeleton, spine::AnimationState* state, float deltaTime );		// Spine�֘A�̍X�V����
void									SkeletonDraw( LPDIRECT3DDEVICE9 pD3DDev, spine::Skeleton* skeleton );							// Spine�֘A�̕`�揈��

void									DrawMesh( LPDIRECT3DDEVICE9 pD3DDev, D3DPRIMITIVETYPE primitiveType, const std::vector<CUSTOMVERTEX>& v, spine::BlendMode blendMode, 
												  spine::AtlasPage* page, LPDIRECT3DTEXTURE9 pTex );

