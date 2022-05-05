//**************************************************************************************************************************************************************
//		インクルードファイル
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
//		グローバル変数
//**************************************************************************************************************************************************************


//**************************************************************************************************************************************************************
//		関数定義
//**************************************************************************************************************************************************************
std::shared_ptr<spine::SkeletonData>	ReadSkeletonJsonData( const spine::String &filename, spine::Atlas *atlas, float scale );


void									InitSpine();
void									SkeletonUpdate( spine::Skeleton* skeleton, spine::AnimationState* state, float deltaTime );		// Spine関連の更新処理
void									SkeletonDraw( LPDIRECT3DDEVICE9 pD3DDev, spine::Skeleton* skeleton );							// Spine関連の描画処理

void									DrawMesh( LPDIRECT3DDEVICE9 pD3DDev, D3DPRIMITIVETYPE primitiveType, const std::vector<CUSTOMVERTEX>& v, spine::BlendMode blendMode, 
												  spine::AtlasPage* page, LPDIRECT3DTEXTURE9 pTex );

