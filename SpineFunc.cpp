//**************************************************************************************************************************************************************
//		インクルードファイル
//**************************************************************************************************************************************************************
#include "StdAfx.h"
#include "SpineFunc.h"
#include <algorithm>


//**************************************************************************************************************************************************************
//		グローバル変数
//**************************************************************************************************************************************************************
spine::Vector<float>			g_worldVertices;
spine::Vector<float>			g_tempUvs;
spine::Vector<spine::Color>		g_tempColors;
spine::Vector<unsigned short>	g_quadIndices;
spine::SkeletonClipping			g_clipper;
bool							g_usePremultipliedAlpha			= true;


// デフォルトアロケータ設定
spine::SpineExtension* spine::getDefaultExtension()
{
	return NEW spine::DefaultSpineExtension();
}


std::shared_ptr<spine::SkeletonData> ReadSkeletonJsonData( const spine::String &filename, spine::Atlas *atlas, float scale )
{
	spine::SkeletonJson json( atlas );
	json.setScale( scale );
	auto skeletonData = json.readSkeletonDataFile( filename );
	if( !skeletonData )
	{
		printf( "%s\n", json.getError().buffer() );
		exit( 0 );
	}
	return std::shared_ptr<spine::SkeletonData>( skeletonData );
}


// Spine関連の更新処理
void SkeletonUpdate( spine::Skeleton* skeleton, spine::AnimationState* state, float deltaTime )
{
	skeleton->update( deltaTime );
	state->update( deltaTime * 1.0f );
	state->apply( *skeleton );
	skeleton->updateWorldTransform();
}


// Spineの初期化
void InitSpine()
{
	// 必要情報初期化
	spine::Bone::setYDown( true );
	g_worldVertices.ensureCapacity( 1000 );

	g_tempUvs.ensureCapacity( 16 );
	g_tempColors.ensureCapacity( 16 );

	g_quadIndices.add( 0 );
	g_quadIndices.add( 1 );
	g_quadIndices.add( 2 );
	g_quadIndices.add( 2 );
	g_quadIndices.add( 3 );
	g_quadIndices.add( 0 );
}


// メッシュ描画
void DrawMesh( LPDIRECT3DDEVICE9 pD3DDev, D3DPRIMITIVETYPE primitiveType, const std::vector<CUSTOMVERTEX>& v, spine::BlendMode blendMode, spine::AtlasPage* pPage, LPDIRECT3DTEXTURE9 pTex )
{
	if( v.empty() )
		return;

	pD3DDev->SetRenderState( D3DRS_LIGHTING, false );

	HRESULT hr = pD3DDev->SetTexture( 0, pTex );


	pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );


	// ブレンディングモード取得
	// sfmlサンプルから算出しているが、sfml -> DX9のブレンディングモード変換が正しい値なのかは不明（調査する必要がある）
	bool isSkip = false;
	if( !g_usePremultipliedAlpha )
	{
		switch( blendMode )
		{
			case spine::BlendMode_Normal:
				pD3DDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
				pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
				pD3DDev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
				break;

			case spine::BlendMode_Additive:
				pD3DDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
				pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
				pD3DDev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
				break;

			case spine::BlendMode_Multiply:
				pD3DDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
				pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
				pD3DDev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );

				// 未確認状態であるため、スキップ扱いとしている
				DebugOut( "blendMode : 未実装状態(%d)", blendMode );
				isSkip = true;
				break;

			case spine::BlendMode_Screen:
				pD3DDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
				pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
				pD3DDev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );

				// 未確認状態であるため、スキップ扱いとしている
				DebugOut( "blendMode : 未実装状態(%d)", blendMode );
				isSkip = true;
				break;


			default:
				// unknownのSpineブレンドモードだった場合、
				// 通常のブレンドモードにフォールバックする
				//engineBlendMode = BLEND_NORMAL;
				break;
		}
	}
	else
	{
		switch( blendMode )
		{
			case spine::BlendMode_Normal:
				pD3DDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
				pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
				pD3DDev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
				break;

			case spine::BlendMode_Additive:
				pD3DDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
				pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
				pD3DDev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
				break;

			case spine::BlendMode_Multiply:
				pD3DDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
				pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
				pD3DDev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );

				// 未確認状態であるため、スキップ扱いとしている
				DebugOut( "blendMode : 未実装状態(%d)", blendMode );
				isSkip = true;
				break;

			case spine::BlendMode_Screen:
				pD3DDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
				pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
				pD3DDev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );

				// 未確認状態であるため、スキップ扱いとしている
				DebugOut( "blendMode : 未実装状態(%d)", blendMode );
				isSkip = true;
				break;



		}
	}

	if( isSkip )
		return;


	//// アルファ関連の設定
	pD3DDev->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, FALSE );
	//pD3DDev->SetRenderState( D3DRS_SRCBLENDALPHA, D3DBLEND_ONE );
	//pD3DDev->SetRenderState( D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA );
	//pD3DDev->SetRenderState( D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD );



	//if( states.blendMode != blend || states.texture != texture )
	//{
	//	target.draw( *vertexArray, states );
	//	vertexArray->clear();
	//	states.blendMode = blend;
	//	states.texture = texture;
	//}


	//テクスチャ色をそのまま使う
	pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );


	// フィルタ情報設定
	if( pPage != nullptr )
	{
		switch( pPage->uWrap )
		{
			case spine::TextureWrap_MirroredRepeat:
				pD3DDev->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR );
				break;

			case spine::TextureWrap_ClampToEdge:
				pD3DDev->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
				break;

			case spine::TextureWrap_Repeat:
				pD3DDev->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
				break;
		}

		switch( pPage->vWrap )
		{
			case spine::TextureWrap_MirroredRepeat:
				pD3DDev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR );
				break;

			case spine::TextureWrap_ClampToEdge:
				pD3DDev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
				break;

			case spine::TextureWrap_Repeat:
				pD3DDev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
				break;
		}


		switch( pPage->magFilter )
		{
			case spine::TextureFilter_Linear:
				pD3DDev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
				break;
		}

		switch( pPage->minFilter )
		{
			case spine::TextureFilter_Linear:
				pD3DDev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
				break;
		}
	}


	pD3DDev->SetFVF( FVF_CUSTOM );
	pD3DDev->DrawPrimitiveUP( primitiveType, v.size() / 3, v.data(), sizeof(CUSTOMVERTEX) );
	//pD3DDev->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, indicesCount / 3, indicesCount, &indices[0], D3DFMT_INDEX16, v.data(), sizeof(CUSTOMVERTEX) );
}


void SkeletonDraw( LPDIRECT3DDEVICE9 pD3DDev, spine::Skeleton* skeleton )
{
	// Early out if skeleton is invisible
	if( skeleton->getColor().a == 0 )
		return;


	std::vector<CUSTOMVERTEX> v;


	spine::AtlasPage* pPage = NULL;
	LPDIRECT3DTEXTURE9 pNowTexture = NULL;
	spine::BlendMode nowBlendMode = spine::BlendMode_Normal;

	uint32_t slotSize = skeleton->getSlots().size();
	for( unsigned i = 0; i < slotSize ; ++i )
	{
		// スロット取得
		spine::Slot* slot = skeleton->getDrawOrder()[i];


		// 現在アクティブなアタッチメント取得
		spine::Attachment *attachment = slot->getAttachment();
		if( !attachment )
		{
			g_clipper.clipEnd( *slot );
			continue;
		}


		// Early out if the slot color is 0 or the bone is not active
		if( slot->getColor().a == 0 || !slot->getBone().isActive() )
		{
			g_clipper.clipEnd( *slot );
			continue;
		}


		spine::Vector<float>* vertices = &g_worldVertices;
		int verticesCount = 0;
		spine::Vector<float>* uvs = NULL;
		spine::Vector<unsigned short>* indices = NULL;
		int indicesCount = 0;


		//normal( GL_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE ), //
		//additive( GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE ), //
		//multiply( GL_DST_COLOR, GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ), //
		//screen( GL_ONE, GL_ONE, GL_ONE_MINUS_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR );


		// 描画カラー算出(スケルトンカラーとスロットカラーから必要カラーを算出(0.0~1.0)
		spine::Color skeletonColor = skeleton->getColor();
		spine::Color slotColor = slot->getColor();
		spine::Color tint( skeletonColor.r * slotColor.r, skeletonColor.g * slotColor.g, skeletonColor.b * slotColor.b, skeletonColor.a * slotColor.a );

		int r, g, b, a;
		r = std::min<int>( 255, tint.r * 256 );
		g = std::min<int>( 255, tint.g * 256 );
		b = std::min<int>( 255, tint.b * 256 );
		a = std::min<int>( 255, tint.a * 256 );


		// アタッチメントのタイプに応じて、頂点配列、インデックス、テクスチャを埋める
		LPDIRECT3DTEXTURE9 texture = NULL;
		spine::BlendMode blendMode = spine::BlendMode_Normal;
		if( attachment->getRTTI().isExactly(spine::RegionAttachment::rtti) )
		{
			//unsigned short quadIndices[] ={ 0, 1, 2, 2, 3, 0 };

			// spRegionAttachmentにキャストして、rendererObjectを取得し、ワールド頂点を計算できるようにする。
			spine::RegionAttachment* regionAttachment = (spine::RegionAttachment*)attachment;


			// Early out if the slot color is 0
			if( regionAttachment->getColor().a == 0 )
			{
				g_clipper.clipEnd( *slot );
				continue;
			}


			// エンジン固有のTextureは、ロード時にアタッチメントに割り当てられた
			// AtlasRegionに格納されています。これは、領域アタッチメントがマッピングされた画像を含む
			// テクスチャアトラスページを表します。
			texture = (LPDIRECT3DTEXTURE9)((spine::AtlasRegion*)regionAttachment->getRendererObject())->page->getRendererObject();
			pPage = ((spine::AtlasRegion*)regionAttachment->getRendererObject())->page;


			// 頂点のための十分なスペースがあることを確認します。
			g_worldVertices.setSize( 8, 0.0f );

			// 矩形領域のアタッチメントを構成する4つの頂点のワールド頂点位置を計算する。
			// これは、スロット (とアタッチメント) がアタッチされているボーンのワールドトランスフォームが、
			// レンダリング前にSkeleton::updateWorldTransform() によって計算されていることを
			// 想定しています。頂点の位置は、sizeof (Vertex) のストライドで、
			// 頂点配列に直接書き込まれます。
			regionAttachment->computeWorldVertices( slot->getBone(), g_worldVertices, 0, 2 );
			verticesCount = 4;


			uvs = &regionAttachment->getUVs();
			indices = &g_quadIndices;
			indicesCount = 6;
		}
		else if( attachment->getRTTI().isExactly( spine::MeshAttachment::rtti ) )
		{
			spine::MeshAttachment *mesh = (spine::MeshAttachment *)attachment;
			//attachmentColor = &mesh->getColor();

			// Early out if the slot color is 0
			if( mesh->getColor().a == 0 )
			{
				//clipper.clipEnd( slot );
				continue;
			}

			g_worldVertices.setSize( mesh->getWorldVerticesLength(), 0 );


			texture = (LPDIRECT3DTEXTURE9)((spine::AtlasRegion*)mesh->getRendererObject())->page->getRendererObject();
			pPage = ((spine::AtlasRegion*)mesh->getRendererObject())->page;


			mesh->computeWorldVertices( *slot, 0, mesh->getWorldVerticesLength(), g_worldVertices, 0, 2 );
			verticesCount = mesh->getWorldVerticesLength() / 2;

			uvs = &mesh->getUVs();

			indices = &mesh->getTriangles();
			indicesCount = mesh->getTriangles().size();


			//// 実際の描画処理
			//std::vector<CUSTOMVERTEX> v( verticesCount );


			//// カラーとUVを頂点にコピーする
			//for( size_t j = 0, l = 0; j < verticesCount; j++, l+=2 )
			//{
			//	v[j].x = vertices[j * 2 + 0];
			//	v[j].y = vertices[j * 2 + 1];
			//	v[j].z = 0.0;
			//	v[j].rhw = 1.0;

			//	v[j].dwColor = (a << 24) || (r << 16) || (g << 8) || (b << 0);
			//	//v[j].dwColor = (255 << 24) || (r << 16) || (g << 8) || (b << 0);

			//	v[j].u = (*uvs)[l];
			//	v[j].v = (*uvs)[l + 1];
			//}
		}
		else if (attachment->getRTTI().isExactly(spine::ClippingAttachment::rtti))
		{
			spine::ClippingAttachment *clip = (spine::ClippingAttachment*)slot->getAttachment();
			g_clipper.clipStart( *slot, clip );
			continue;
		}
		else
		{
			//g_clipper.clipEnd( *slot );
			continue;
		}


		// 描画パラメータが異なる場合はそれまでの頂点を描画
		blendMode = slot->getData().getBlendMode();
		if( nowBlendMode != blendMode || pNowTexture != texture )
		{
			DrawMesh( pD3DDev, D3DPT_TRIANGLELIST, v, nowBlendMode, pPage, pNowTexture );
			v.clear();
			nowBlendMode = blendMode;
			pNowTexture = texture;
		}


		// クリッピング処理(Spineのクリッピングはポリゴンを分割することで表現している)
		if( g_clipper.isClipping() )
		{
			g_clipper.clipTriangles( g_worldVertices, *indices, *uvs, 2 );
			vertices = &g_clipper.getClippedVertices();
			verticesCount = g_clipper.getClippedVertices().size() >> 1;
			uvs = &g_clipper.getClippedUVs();

			indices = &g_clipper.getClippedTriangles();
			indicesCount = g_clipper.getClippedTriangles().size();
		}


		// インデックスを元に頂点を生成(インデックスを直接描画する方法もありそうなのだが、上手く行かず)
		std::vector<CUSTOMVERTEX> testV;
		for( int ii = 0; ii < indicesCount; ++ii )
		{
			CUSTOMVERTEX vertex = {};

			int index = (*indices)[ii] << 1;
			vertex.x = (*vertices)[index];
			vertex.y = (*vertices)[index + 1];
			vertex.z = 0.0;
			vertex.rhw = 1.0;

			vertex.dwColor = (a << 24) | (r << 16) | (g << 8) | (b << 0);

			vertex.u = (*uvs)[index];
			vertex.v = (*uvs)[index + 1];

			v.push_back( vertex );
			testV.push_back( vertex );
		}
		g_clipper.clipEnd( *slot );


		DrawBoundingBox( pD3DDev, testV );
	}

	DrawMesh( pD3DDev, D3DPT_TRIANGLELIST, v, nowBlendMode, pPage, pNowTexture );
	v.clear();

	g_clipper.clipEnd();
}
