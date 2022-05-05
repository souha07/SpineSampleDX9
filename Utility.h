#pragma once

//**************************************************************************************************************************************************************
//		インクルードファイル
//**************************************************************************************************************************************************************
#include <windows.h>
#include <string.h>
#include <vector>
#include <stdint.h>


// デバッグ表示関数
template<typename ...Args>
inline void DebugOut( const char* pString, Args&& ...args )
{
	char buf[1024];
	snprintf( buf, sizeof(buf), pString, args... );
	strcat_s( buf, sizeof(buf), "\n" );

	OutputDebugString( buf );
	printf( "%s", buf );
}


// 頂点情報
struct CUSTOMVERTEX
{
	float x, y, z;		// 頂点座標
	float rhw;			// 除算数
	DWORD dwColor;		// 頂点の色
	float u, v;			// テクスチャ座標
};
#define FVF_CUSTOM			( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )


struct LineVertex
{
	float x, y, z, w;
	DWORD color;
};
#define FVF_CUSTOM_LINE		( D3DFVF_XYZRHW | D3DFVF_DIFFUSE )


// グリッド描画
inline void DrawGrid( IDirect3DDevice9* pD3DDev )
{
	pD3DDev->SetFVF( FVF_CUSTOM_LINE );
	pD3DDev->SetTexture( 0, nullptr );
	LineVertex line[2] =
	{
		{ 0, 0, 0, 1.0f, 0xff888888 },
		{ 0, 0, 0, 1.0f, 0xff888888 },
	};
	for( int x = 0; x <= 640; x+= 20 )
	{
		line[0].x = (float)x; line[0].y =   0.0f;
		line[1].x = (float)x; line[1].y = 480.0f;
		pD3DDev->DrawPrimitiveUP( D3DPT_LINELIST, 1, line, sizeof( LineVertex ) );
	}

	for( int y = 0; y <= 480; y+= 20 )
	{
		line[0].x =   0.0f; line[0].y = (float)y;
		line[1].x = 640.0f; line[1].y = (float)y;
		pD3DDev->DrawPrimitiveUP( D3DPT_LINELIST, 1, line, sizeof( LineVertex ) );
	}

	line[0].x =   0; line[0].y = 239.5f; line[0].color = 0xffff0000;
	line[1].x = 640; line[1].y = 239.5f; line[1].color = 0xffff0000;
	pD3DDev->DrawPrimitiveUP( D3DPT_LINELIST, 1, line, sizeof( LineVertex ) );

	line[0].x = 320; line[0].y =   0; line[0].color = 0xffff0000;
	line[1].x = 320; line[1].y = 480; line[1].color = 0xffff0000;
	pD3DDev->DrawPrimitiveUP( D3DPT_LINELIST, 1, line, sizeof( LineVertex ) );
}


// 描画範囲表示
inline void DrawBoundingBox( IDirect3DDevice9* pD3DDev, const std::vector<CUSTOMVERTEX>& v )
{
	if( v.empty() )
		return;


	float left		= FLT_MAX;
	float top		= FLT_MAX;
	float right		= FLT_MIN;
	float bottom	= FLT_MIN;

	for( size_t i = 0 ; i < v.size() ; ++i )
	{
		if( left > v[i].x )
			left = v[i].x;
		if( right < v[i].x )
			right = v[i].x;

		if( top > v[i].y )
			top = v[i].y;
		if( bottom < v[i].y )
			bottom = v[i].y;
	}


	pD3DDev->SetFVF( FVF_CUSTOM_LINE );
	pD3DDev->SetTexture( 0, nullptr );


	LineVertex line[2] =
	{
		{ 0, 0, 0, 1.0f, 0xffff0000 },
		{ 0, 0, 0, 1.0f, 0xffff0000 },
	};


	// ul -> ur
	line[0].x = left;
	line[0].y = top;
	line[1].x = right;
	line[1].y = top;
	pD3DDev->DrawPrimitiveUP( D3DPT_LINELIST, 1, line, sizeof(LineVertex) );


	// ur -> dr
	line[0].x = right;
	line[0].y = top;
	line[1].x = right;
	line[1].y = bottom;
	pD3DDev->DrawPrimitiveUP( D3DPT_LINELIST, 1, line, sizeof(LineVertex) );


	// dr -> dl
	line[0].x = right;
	line[0].y = bottom;
	line[1].x = left;
	line[1].y = bottom;
	pD3DDev->DrawPrimitiveUP( D3DPT_LINELIST, 1, line, sizeof(LineVertex) );


	// dl -> ul
	line[0].x = left;
	line[0].y = bottom;
	line[1].x = left;
	line[1].y = top;
	pD3DDev->DrawPrimitiveUP( D3DPT_LINELIST, 1, line, sizeof(LineVertex) );
}





