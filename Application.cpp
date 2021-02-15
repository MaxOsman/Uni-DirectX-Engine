#include "Application.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pConstantBuffer = nullptr;
    _pSamplerLinear = nullptr;
    _transparency = nullptr;
    RAWINPUTDEVICE rid;
    rid.usUsagePage = 0x01;
    rid.usUsage = 0x02;
    rid.dwFlags = 0;
    rid.hwndTarget = nullptr;
    RegisterRawInputDevices(&rid, 1, sizeof(rid));
    srand(time(0));
}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    RECT rc;
    GetClientRect(_hWnd, &rc);
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }

    cameraMode = CAMERA_ANGLED;
    isSolid = true;
    orbitAngle = 0.0f;
    playerSpeed = 10.0f;

    camera = new Camera(_WindowWidth, _WindowHeight, 
        { 0.0f, 20.0f, 15.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f });
    light = new Light();
    debug = new Debug();

    //Load games assets from JSON file
    LoadObjectData();

    //Get right mesh
    int meshIndex = 0;
    for (int i = 0; i < _meshes.size(); ++i)
    {
        if (_meshes[i].name == "Plane")
        {
            meshIndex = i;
            break;
        }
    }
    //Get right texture
    int texIndex = 0;
    for (int i = 0; i < _textures.size(); ++i)
    {
        if (_textures[i].name == "Particle")
        {
            texIndex = i;
            break;
        }
    }

    particleManager = new ParticleManager(10);

    FireParticleSystem* fireSystem = new FireParticleSystem(_meshes[meshIndex].meshData, _textures[texIndex].texture);
    fireSystem->Initialise();
    particleManager->AddSystem(fireSystem);
      
	return S_OK;
}

void Application::LoadObjectData()
{
    //Set up JSON
    json j;
    ifstream t("SceneData.json");
    t >> j;

    //Get stuff from JSON
    json meshPaths = j["meshPaths"];
    json texPaths = j["texPaths"];
    json materials = j["materials"];
    json objects = j["objects"];

    //Meshes
    for (size_t i = 0; i < meshPaths.size(); ++i)
    {
        _meshes.push_back({ OBJLoader::Load((char*)("Assets/mesh" + (string)meshPaths.at(i) + ".obj").c_str(), _pd3dDevice), meshPaths.at(i) });      //Mesh
    }

    //Textures
    for (size_t i = 0; i < texPaths.size(); ++i)
    {
        ID3D11ShaderResourceView* tempTex = nullptr;
        string path = texPaths.at(i);
        wstring wString = wstring(path.begin(), path.end());    //Convert from string to wstring
        CreateDDSTextureFromFile(_pd3dDevice, (L"Assets/tex" + wString + L".dds").c_str(), nullptr, &tempTex);     //Convert from wstring to wchar
        _textures.push_back({ tempTex, texPaths.at(i) });
    }

    //Materials
    for (size_t i = 0; i < materials.size(); ++i)
    {
        json diff = materials.at(i)["diff"];
        json amb = materials.at(i)["amb"];
        json spec = materials.at(i)["spec"];
        _materials.push_back({ { diff.at(0), diff.at(1), diff.at(2), 0.0f },
                               { amb.at(0), amb.at(1), amb.at(2), 0.0f },
                               { spec.at(0), spec.at(1), spec.at(2), 0.0f },
                                materials.at(i)["name"] });
    }

    //Objects
    for (size_t i = 0; i < objects.size(); ++i)
    {
        //Find correct mesh
        string meshName = objects.at(i)["mesh"];
        int meshIndex = 0;
        for (int j = 0; j < _meshes.size(); ++j)
        {
            if (meshName == "BasicCube")
            {
                meshIndex = j;
                break;
            }
            if (meshName == _meshes[j].name)
            {
                meshIndex = j;
                break;
            }
        }

        //Find correct texture
        string texName = objects.at(i)["tex"];
        int texIndex = 0;
        for (int j = 0; j < _textures.size(); ++j)
        {
            if (texName == _textures[j].name)
            {
                texIndex = j;
                break;
            }
        }

        //Find correct material
        string matName = objects.at(i)["mat"];
        int matIndex = 0;
        for (int j = 0; j < _materials.size(); ++j)
        {
            if (matName == _materials[j].name)
            {
                matIndex = j;
                break;
            }
        }

        //Find matrix info
        json pos = objects.at(i)["pos"];
        Vector3D posMatrix = { pos.at(0), pos.at(1), pos.at(2) };
        json rot = objects.at(i)["rot"];
        Vector3D rotMatrix = { rot.at(0) / 180.0f * XM_PI, rot.at(1) / 180.0f * XM_PI, rot.at(2) / 180.0f * XM_PI };
        json scale = objects.at(i)["scale"];
        Vector3D scaleMatrix = { scale.at(0), scale.at(1), scale.at(2) };

        //Transparency
        bool trans = objects.at(i)["trans"];

        //Billboard
        bool bill = false;
        if (texName[0] == '~')
            bill = true;

        //Finalise
        Transform* tempTrans = new Transform( posMatrix, rotMatrix, scaleMatrix );
        Appearance* tempApp = new Appearance(   _meshes[meshIndex].meshData,
                                                _textures[texIndex].texture,
                                                _materials[matIndex].diffuseMaterial,
                                                _materials[matIndex].ambientMaterial,
                                                _materials[matIndex].specularMaterial,
                                                trans,
                                                bill);
        bool terrain;
        if (i == PLAYEROBJECT)
            terrain = false;
        else
            terrain = true;

        _objects.push_back({ tempTrans, tempApp, { 0.0f, 0.0f, 0.0f }, 20.0f, terrain });

    }
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{	
		pVSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

    
    
	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	return hr;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, 0, 960, 540};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"DX11 Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT Application::InitDevice()
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _WindowWidth;
    sd.BufferDesc.Height = _WindowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    D3D11_TEXTURE2D_DESC depthStencilDesc;
    depthStencilDesc.Height = _WindowHeight;
    depthStencilDesc.Width = _WindowWidth;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

    _pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
    _pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    if (FAILED(hr))
        return hr;
    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr))
        return hr;
    _pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_WindowWidth;
    vp.Height = (FLOAT)_WindowHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

	InitShadersAndInputLayout();

    // Set index buffer
    //_pImmediateContext->IASetIndexBuffer(_pIndexBuffers[0], DXGI_FORMAT_R16_UINT, 0);

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

    //Wireframes
    D3D11_RASTERIZER_DESC wfdesc;
    ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
    wfdesc.FillMode = D3D11_FILL_WIREFRAME;
    wfdesc.CullMode = D3D11_CULL_NONE;
    hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame);

    //Solid
    D3D11_RASTERIZER_DESC sldesc;
    ZeroMemory(&sldesc, sizeof(D3D11_RASTERIZER_DESC));
    sldesc.FillMode = D3D11_FILL_SOLID;
    sldesc.CullMode = D3D11_CULL_BACK;
    hr = _pd3dDevice->CreateRasterizerState(&sldesc, &_solid);

    // Create the sample state
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    _pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear);

    //Blend equation
    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));

    D3D11_RENDER_TARGET_BLEND_DESC rtbd;
    ZeroMemory(&rtbd, sizeof(rtbd));

    rtbd.BlendEnable = true;
    rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
    rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
    rtbd.BlendOp = D3D11_BLEND_OP_ADD;
    rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
    rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
    rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.RenderTarget[0] = rtbd;

    _pd3dDevice->CreateBlendState(&blendDesc, &_transparency);

    if (FAILED(hr))
        return hr;

    return S_OK;
}

bool Application::HandleKeyboard(MSG msg)
{
    if (!(msg.lParam & 0x40000000))
    {
        switch (msg.wParam)
        {
        case 0x43:      //C key
            switch (cameraMode)
            {
            case CAMERA_ANGLED:
                camera->ChangePos({ 0.0f+_objects[PLAYEROBJECT].GetPos().x, 20.0f+_objects[PLAYEROBJECT].GetPos().y, 0.0f+_objects[PLAYEROBJECT].GetPos().z },
                    _objects[PLAYEROBJECT].GetPos(),
                    { 0.0f, 0.0f, -1.0f });
                cameraMode = CAMERA_TOPDOWN;
                return true;

            case CAMERA_TOPDOWN:
                cameraMode = CAMERA_FIRST;
                camera->ChangePos(_objects[PLAYEROBJECT].GetPos(),
                    { 0.0f, 1.0f, 0.0f },
                    { 0.0f, 1.0f, 0.0f });
                ConfineCursor();
                return true;

            case CAMERA_FIRST:
                cameraMode = CAMERA_THIRD;
                return true;

            case CAMERA_THIRD:
                camera->ChangePos({ 0.0f, 20.0f, 15.0f },
                    { 0.0f, 0.0f, 0.0f },
                    { 0.0f, 1.0f, 0.0f });
                cameraMode = CAMERA_ANGLED;
                FreeCursor();
                return true;
            }

        case 0x4D:      //M key
            isSolid = !isSolid;
            return true;

        case VK_SPACE:
            _objects[PLAYEROBJECT].PlayerJump(5000.0f);
            return true;
        }
    }

    return false;
}

void Application::HandlePerFrameInput(float deltaTime)
{
    _objects[PLAYEROBJECT].GetParticle()->SetThrust({0.0f, 0.0f, 0.0f});
    if (GetAsyncKeyState('W'))
    {
        _objects[PLAYEROBJECT].PlayerMove({ 0.0f, 0.0f, playerSpeed * deltaTime }, camera);
    }
    if (GetAsyncKeyState('A'))
    {
        _objects[PLAYEROBJECT].PlayerMove({ -playerSpeed * deltaTime, 0.0f, 0.0f }, camera);
    }
    if (GetAsyncKeyState('S'))
    {
        _objects[PLAYEROBJECT].PlayerMove({ 0.0f, 0.0f, -playerSpeed * deltaTime }, camera);
    }
    if (GetAsyncKeyState('D'))
    {
        _objects[PLAYEROBJECT].PlayerMove({ playerSpeed * deltaTime, 0.0f, 0.0f }, camera);
    }
    /*if (GetAsyncKeyState('Q'))
    {
        _objects[PLAYEROBJECT].GetTransform()->Translate({ 0.0f, playerSpeed * deltaTime, 0.0f });
    }
    if (GetAsyncKeyState('E'))
    {
        _objects[PLAYEROBJECT].GetTransform()->Translate({ 0.0f, -playerSpeed * deltaTime, 0.0f });
    }*/
    if (GetAsyncKeyState(VK_ADD))
    {
        camera->AddR(-2.0f * deltaTime);
    }
    if (GetAsyncKeyState(VK_SUBTRACT))
    {
        camera->AddR(2.0f * deltaTime);
    }
    if (GetAsyncKeyState(VK_UP))
    {
        light->AddDirection({ 0.0f, 0.0f, -2.0f * deltaTime });
    }
    if (GetAsyncKeyState(VK_DOWN))
    {
        light->AddDirection({ 0.0f, 0.0f, 2.0f * deltaTime });
    }
    if (GetAsyncKeyState(VK_LEFT))
    {
        light->AddDirection({ 2.0f * deltaTime, 0.0f, 0.0f });
    }
    if (GetAsyncKeyState(VK_RIGHT))
    {
        light->AddDirection({ -2.0f * deltaTime, 0.0f, 0.0f });
    }

    if (GetAsyncKeyState(VK_SHIFT))
    {
        playerSpeed = PLAYERSPEED * 2.0f;
    }
    else
    {
        playerSpeed = PLAYERSPEED;
    }
}

void Application::ConfineCursor()
{
    while (::ShowCursor(FALSE) >= 0);
    RECT rc;
    GetClientRect(_hWnd, &rc);
    MapWindowPoints(_hWnd, nullptr, reinterpret_cast<POINT*>(&rc), 2);
    ClipCursor(&rc);
}

void Application::FreeCursor()
{
    while (::ShowCursor(TRUE) < 0);
    ClipCursor(nullptr);
}

void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();
    if (_pConstantBuffer) _pConstantBuffer->Release();
    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();
    if (_depthStencilView) _depthStencilView->Release();
    if (_depthStencilBuffer) _depthStencilBuffer->Release();
    if (_wireFrame) _wireFrame->Release();
    if (_solid) _solid->Release();
    if (_pSamplerLinear) _pSamplerLinear->Release();
    if (_transparency) _transparency->Release();
}

void Application::Update()
{
    // Update our time
    static float deltaTime = 0.0f;

    if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        deltaTime += (float) XM_PI * 0.0125f;
    }
    else
    {
        //static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount();

        if (dwTimeStart == 0)
            dwTimeStart = dwTimeCur;

        deltaTime = (dwTimeCur - dwTimeStart) / 1000.0f;

        if (deltaTime < FPS60)
            return;

        //Once per frame
        for (size_t i = 0; i < _objects.size(); ++i)
        {
            if(!_objects[i].GetTerrain())
                _objects[i].Update(deltaTime);
        }

        particleManager->Update(deltaTime);
        camera->Update(cameraMode, _hWnd, _objects[PLAYEROBJECT].GetPos());
        camera->SetMonkey(_objects[PLAYEROBJECT].GetPos());
        XMFLOAT3 tempFloat = { camera->GetEye().x, camera->GetEye().y, camera->GetEye().z };
        light->SetEye(tempFloat);

        //Sphere orbit
        _objects[PLAYEROBJECT].SetRot({ camera->GetPitch(), camera->GetYaw(), 0.0f });
        _objects[ORBITOBJECT].SetPos({ 15*cos(orbitAngle), 15.0f+cos(orbitAngle)*2, 15*sin(orbitAngle) });

        //_cb.gTime = deltaTime;
        orbitAngle += 1.0f * deltaTime;
        if (orbitAngle > XM_2PI)
            orbitAngle = 0.0f;

        //Hold controls
        HandlePerFrameInput(deltaTime);

        //Reset frame
        deltaTime -= FPS60;
        dwTimeStart = dwTimeCur;
    } 
}

void Application::Draw()
{
    // Clear the back buffer
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // Red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
    _pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    //Matrices
    _cb.mView = XMMatrixTranspose(XMLoadFloat4x4(&camera->GetView()));
    _cb.mProjection = XMMatrixTranspose(XMLoadFloat4x4(&camera->GetProjection()));
    _pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
    
    //Light
    light->Draw(&_cb);

    //Shader
    _pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
    _pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
    _pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
    _pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0); 

    //Wireframe mode
    if (isSolid)
        _pImmediateContext->RSSetState(_solid);
    else
        _pImmediateContext->RSSetState(_wireFrame);

    //Billboard angle
    float yaw = XM_PI;
    if (cameraMode == CAMERA_FIRST || cameraMode == CAMERA_THIRD)
        yaw = camera->GetYaw();

    //Don't show the monkey in 1st person camera
    int start = 0;
    if (cameraMode == CAMERA_FIRST)
        start = 1;
    for (size_t i = start; i < _objects.size(); ++i)
    {
        if(!_objects[i].GetTransparent())
            _objects[i].Render(_cb, _pImmediateContext, _pConstantBuffer, nullptr, yaw);
    }
    particleManager->Render(_pImmediateContext, _cb, _pConstantBuffer);
    for (size_t i = start; i < _objects.size(); ++i)
    {
        if (_objects[i].GetTransparent())
            _objects[i].Render(_cb, _pImmediateContext, _pConstantBuffer, _transparency, yaw);
    }

    _pSwapChain->Present(0, 0);
}