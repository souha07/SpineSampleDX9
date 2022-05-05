//**************************************************************************************************************************************************************
//		インクルードファイル
//**************************************************************************************************************************************************************
#include "StdAfx.h"
#include <windows.h>
#include <tchar.h>
#include <d3d9.h>
#include <d3dx9tex.h>
#include <math.h>
#include <memory>
#include <vector>
#include <algorithm>

#include <spine/spine.h>
#include <spine/Extension.h>

#include "CDX9TextureLoader.h"
#include "SpineFunc.h"
#include "Utility.h"


//**************************************************************************************************************************************************************
//		インポートライブラリ
//**************************************************************************************************************************************************************
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")


using namespace spine;


//**************************************************************************************************************************************************************
//		グローバル定数
//**************************************************************************************************************************************************************
const TCHAR						gName[100]		= _T( "Spine DX9描画サンプルプログラム" );			// ウィンドウタイトル


//**************************************************************************************************************************************************************
//		グローバル変数
//**************************************************************************************************************************************************************
LPDIRECT3D9						g_pD3D;
LPDIRECT3DDEVICE9				g_pD3DDev;

CDX9TextureLoader				g_textureLoader;


// ウィンドウプロシージャ
LRESULT CALLBACK WndProc(HWND hWnd, UINT mes, WPARAM wParam, LPARAM lParam)
{
	if(mes == WM_DESTROY || mes == WM_CLOSE )
	{
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, mes, wParam, lParam);
}


int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif


	// アプリケーションの初期化
	MSG msg; HWND hWnd;
	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		NULL,
		(HBRUSH)(COLOR_WINDOW+1),
		NULL,
		(TCHAR*)gName,
		NULL
	};
	if( !RegisterClassEx(&wcex) )
		return 0;


	hWnd = CreateWindow( gName, gName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL );
	if( !hWnd )
		return 0;

	// Direct3Dの初期化
	g_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
	if( !g_pD3D )
		return 0;

	D3DPRESENT_PARAMETERS d3dpp = 
	{
		0,
		0,
		D3DFMT_UNKNOWN,
		0,
		D3DMULTISAMPLE_NONE,
		0,
		D3DSWAPEFFECT_DISCARD,
		NULL,
		TRUE,
		0,
		D3DFMT_UNKNOWN,
		0,
		0
	};

	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pD3DDev ) ) )
	{
		if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pD3DDev ) ) )
		{
			if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pD3DDev ) ) )
			{
				if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pD3DDev ) ) )
				{
					g_pD3D->Release();
					return 0;
				}
			}
		}
	}


	// ウィンドウ表示
	ShowWindow(hWnd, nCmdShow);


	// テクスチャローダーの初期化
	g_textureLoader.Init( g_pD3DDev );


	// Spine関連の処理(http://ja.esotericsoftware.com/spine-cpp参照)

	std::string atlasName = "data/spineboy-pma.atlas";
	std::string jsonName  = "data/spineboy-pro.json";
	std::vector<std::pair<int, std::string> > animeList = 
	{
		//{ 0, "idle" },
		{ 0, "walk" },
		{ 1, "aim" },
	};

	//std::string atlasName = "data/coin-pma.atlas";
	//std::string jsonName  = "data/coin-pro.json";
	//std::vector<std::pair<int, std::string> > animeList = 
	//{
	//	//{ 0, "idle" },
	//	{ 0, "animation" },
	//};


	// テクスチャアトラスのロード
	auto atlas = std::make_unique<Atlas>( atlasName.c_str(), &g_textureLoader );


	// スケルトンデータのロード(JSON)
	auto skeletonData = ReadSkeletonJsonData( jsonName.c_str(), atlas.get(), 0.5f );


	// スケルトン生成
	Bone::setYDown( true );
	auto skeleton = NEW Skeleton( skeletonData.get() );


	// アニメーションステートデータの生成
	//ownsAnimationStateData = stateData == 0;
	AnimationStateData* stateData = nullptr;
	bool ownsAnimationStateData = true;
	if( ownsAnimationStateData )
		stateData = NEW AnimationStateData( skeletonData.get() );

	AnimationState* state = NEW AnimationState( stateData );

	skeleton->setPosition( 480, 320 );
	skeleton->updateWorldTransform();

	for( const auto& data : animeList )
	{
		state->setAnimation( data.first, data.second.c_str(), true );
	}


	// Spine関連の初期化
	InitSpine();


	// メッセージ ループ
	int i;
	do
	{
		Sleep(1);

		if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
		{
			DispatchMessage(&msg);
		}

		
		SkeletonUpdate( skeleton, state, 1.0f / 60.0f );


		// Direct3Dの処理
		g_pD3DDev->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );
		g_pD3DDev->BeginScene();


		SkeletonDraw( g_pD3DDev, skeleton );


		g_pD3DDev->EndScene();
		g_pD3DDev->Present( NULL, NULL, NULL, NULL );
	}
	while( msg.message != WM_QUIT );


	delete state;
	delete stateData;
	delete skeleton;

	skeletonData.reset();
	atlas.reset();


	g_pD3DDev->Release();
	g_pD3D->Release();

	return 0;
}
