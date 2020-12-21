#pragma once

#include "Structs.h"
#include "Camera.h"
#include "Light.h"
#include "DDSTextureLoader.h"
#include <vector>
#include "OBJLoader.h"
#include "json.hpp"
#include <iostream>
#include "WorldObject.h"

using namespace std;
using json = nlohmann::json;

class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*			_pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11InputLayout*      _pVertexLayout;
	ID3D11Buffer*			_pConstantBuffer;
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D*		_depthStencilBuffer;
	ID3D11RasterizerState*	_wireFrame;
	ID3D11RasterizerState*	_solid;
	ConstantBuffer			_cb;
	ID3D11SamplerState*		_pSamplerLinear;
	ID3D11BlendState*		_transparency;

	Camera* camera;
	Light* light;
	vector<IndexedMesh> _meshes;
	vector<IndexedTex> _textures;
	vector<IndexedMat> _materials;
	vector<WorldObject> _objects;

	CameraMode cameraMode;
	bool isSolid;

	UINT _WindowHeight;
	UINT _WindowWidth;

private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();
	void LoadObjectData();

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);
	bool HandleKeyboard(MSG msg);

	void ConfineCursor();
	void FreeCursor();
	
	void Update();
	void Draw();
};