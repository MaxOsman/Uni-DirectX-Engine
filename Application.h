#pragma once

#define GRID_WIDTH 64
#define GRID_DEPTH 64
#define HEIGHT_SCALE 5.0f

#include "DDSTextureLoader.h"
#include <vector>
#include "OBJLoader.h"
#include "json.hpp"
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

#include "Structs.h"
#include "Camera.h"
#include "Light.h"
#include "WorldObject.h"
#include "Debug.h"
#include "ParticleManager.h"
#include "FireParticleSystem.h"

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
	ID3D11VertexShader*		_pVertexShaderTerrain;
	ID3D11PixelShader*		_pPixelShaderTerrain;

	ID3D11InputLayout*      _pVertexLayout;
	ID3D11Buffer*			_pConstantBuffer;
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D*		_depthStencilBuffer;
	ID3D11RasterizerState*	_wireFrame;
	ID3D11RasterizerState*	_solid;
	ConstantBuffer			_cb;
	ID3D11SamplerState*		_pSamplerLinear;
	ID3D11BlendState*		_transparency;

	Camera* _camera;
	Light* _light;
	Debug* _debug;
	ParticleManager* _particleManager;
	vector<IndexedMesh> _meshes;
	vector<IndexedTex> _textures;
	vector<IndexedMat> _materials;
	vector<WorldObject> _objects;
	int terrainTexIndices[5];

	CameraMode cameraMode;
	bool isSolid;
	float orbitAngle;
	float playerSpeed;

	UINT _WindowHeight;
	UINT _WindowWidth;
	DWORD dwTimeStart = 0;

private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();

	void LoadObjectData();
	vector<float> *LoadHeightMap();

	void ConfineCursor();
	void FreeCursor();
	void CollisionDetection(int i, float deltaTime);
	void CollisionResponseAABB(Vector3D penetration, int i, int j);
	void CollisionResponseSphere(Vector3D penetration, int i, int j, float deltaTime);
	Vector3D ProjectUOnV(Vector3D u, Vector3D v);
	void CollisionResponseAABBSphere(Vector3D penetration, int i, int j, int sphereIndex, float multiply);

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);
	bool HandleKeyboard(MSG msg);
	void HandlePerFrameInput(float deltaTime);
	
	void Update();
	void Draw();
};