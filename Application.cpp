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

    _camera = new Camera(_WindowWidth, _WindowHeight,
        { 0.0f, 20.0f, 15.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f });
    _light = new Light();
    _debug = new Debug();

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
        if (_textures[i].name == "~Particle")
        {
            texIndex = i;
            break;
        }
    }

    _particleManager = new ParticleManager(10);

    FireParticleSystem* fireSystem = new FireParticleSystem(_meshes[meshIndex].meshData, _textures[texIndex].texture);
    fireSystem->position = { 15, 0, 5 };
    fireSystem->Initialise();
    _particleManager->AddSystem(fireSystem);

    SmokeParticleSystem* smokeSystem = new SmokeParticleSystem(_meshes[meshIndex].meshData, _textures[texIndex].texture);
    smokeSystem->position = { 15, 0, 5 };
    smokeSystem->Initialise();
    _particleManager->AddSystem(smokeSystem);

    for (int i = 0; i < _meshes.size(); ++i)
    {
        if (_meshes[i].name == "Cube")
        {
            meshIndex = i;
            break;
        }
    }
    for (int i = 0; i < _textures.size(); ++i)
    {
        if (_textures[i].name == "Stone")
        {
            texIndex = i;
            break;
        }
    }

    CubeParticleSystem* cubeSystem = new CubeParticleSystem(_meshes[meshIndex].meshData, _textures[texIndex].texture);
    cubeSystem->position = { -15, 0, 5 };
    cubeSystem->Initialise();
    _particleManager->AddSystem(cubeSystem);

    //_objects[PHYSOBJECT].CalcTorque({1,1,0});
      
	return S_OK;
}

vector<float>* Application::LoadHeightMap()
{
    // A height for each vertex 
    vector<unsigned char> in(GRID_WIDTH * GRID_DEPTH);

    // Open the file.
    ifstream inFile;
    inFile.open("TerrainAssets/terrain2.raw", ios_base::binary);

    if (inFile)
    {
        // Read the RAW bytes.
        inFile.read((char*)&in[0], (streamsize)in.size());
        inFile.close();
    }

    vector<float> *tempHeights = new vector<float>;
    // Copy the array data into a float array and scale it. mHeightmap.resize(heightmapHeight * heightmapWidth, 0);
    for (UINT i = 0; i < GRID_WIDTH * GRID_DEPTH; ++i)
    {
        tempHeights->push_back((in[i] / 255.0f) * HEIGHT_SCALE);
    }

    return tempHeights;
}

void Application::LoadObjectData()
{
    //Set up JSON
    json j;
    ifstream t("SceneData.json");
    //ifstream t("SceneDataProcedural.json");
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

    //Tri grid
    vector<XMFLOAT3> verts;
    vector<XMFLOAT3> normals;
    vector<XMFLOAT2> texCoords;
    vector<unsigned short> vertIndices;
    vector<unsigned short> normalIndices;
    vector<unsigned short> textureIndices;

    vector<float>* heights = LoadHeightMap();

    for (unsigned int i = 0; i < GRID_WIDTH; ++i)
    {
        for (unsigned int j = 0; j < GRID_DEPTH; ++j)
        {
            verts.push_back({ 1.0f * i - GRID_WIDTH / 2, heights->at(j + i * GRID_WIDTH), 1.0f * j - GRID_DEPTH / 2 });
        }
    }
    normals.push_back({0.0f, 1.0f, 0.0f});
    texCoords.push_back({ 0.0f, 0.0f });
    texCoords.push_back({ 1.0f, 0.0f });
    texCoords.push_back({ 0.0f, 1.0f });
    texCoords.push_back({ 1.0f, 1.0f });

    for (unsigned int i = 0; i < GRID_WIDTH-1; ++i)
    {
        for (unsigned int j = 0; j < GRID_DEPTH-1; ++j)
        {         
            vertIndices.push_back(i * GRID_WIDTH + j);
            vertIndices.push_back(i * GRID_WIDTH + j + 1);
            vertIndices.push_back((i + 1) * GRID_WIDTH + j);
            vertIndices.push_back((i + 1) * GRID_WIDTH + j);
            vertIndices.push_back(i * GRID_WIDTH + j + 1);
            vertIndices.push_back((i + 1) * GRID_WIDTH + j + 1);

            /*XMFLOAT3 a = verts[i * GRID_WIDTH + j];
            XMFLOAT3 b = verts[i * GRID_WIDTH + j + 1];
            XMFLOAT3 c = verts[(i + 1) * GRID_WIDTH + j];
            XMFLOAT3 d = verts[(i + 1) * GRID_WIDTH + j + 1];

            //AB cross AC
            Vector3D cross = { b.x - a.x, b.y - a.y, b.z - a.z };
            cross.cross_product({ c.x - a.x, c.y - a.y, c.z - a.z });
            cross = cross.normalization();
            normals.push_back({ cross.x, cross.y, cross.z });
            //BA cross BC
            cross = { a.x - b.x, a.y - b.y, a.z - b.z };
            cross.cross_product({ c.x - b.x, c.y - b.y, c.z - b.z });
            cross = cross.normalization();
            normals.push_back({ cross.x, cross.y, cross.z });
            //CA cross CB
            cross = { a.x - c.x, a.y - c.y, a.z - c.z };
            cross.cross_product({ b.x - c.x, b.y - c.y, b.z - c.z });
            cross = cross.normalization();
            normals.push_back({ cross.x, cross.y, cross.z });
            //CB cross CD
            cross = { b.x - c.x, b.y - c.y, b.z - c.z };
            cross.cross_product({ d.x - c.x, d.y - c.y, d.z - c.z });
            cross = cross.normalization();
            normals.push_back({ cross.x, cross.y, cross.z });
            //BC cross BD
            cross = { c.x - b.x, c.y - b.y, c.z - b.z };
            cross.cross_product({ d.x - b.x, d.y - b.y, d.z - b.z });
            cross = cross.normalization();
            normals.push_back({ cross.x, cross.y, cross.z });
            //DB cross DC
            cross = { b.x - d.x, b.y - d.y, b.z - d.z };
            cross.cross_product({ c.x - d.x, c.y - d.y, c.z - d.z });
            cross = cross.normalization();
            normals.push_back({ cross.x, cross.y, cross.z });

            normalIndices.push_back(6 * (i * (GRID_WIDTH - 1) + j));
            normalIndices.push_back(6 * (i * (GRID_WIDTH - 1) + j) + 1);
            normalIndices.push_back(6 * (i * (GRID_WIDTH - 1) + j) + 2);
            normalIndices.push_back(6 * (i * (GRID_WIDTH - 1) + j) + 3);
            normalIndices.push_back(6 * (i * (GRID_WIDTH - 1) + j) + 4);
            normalIndices.push_back(6 * (i * (GRID_WIDTH - 1) + j) + 5);*/

            normalIndices.push_back(0);
            normalIndices.push_back(0);
            normalIndices.push_back(0);
            normalIndices.push_back(0);
            normalIndices.push_back(0);
            normalIndices.push_back(0);

            textureIndices.push_back(0);
            textureIndices.push_back(1);
            textureIndices.push_back(2);
            textureIndices.push_back(2);
            textureIndices.push_back(1);
            textureIndices.push_back(3);
        }
    }

    MeshData gridMesh = OBJLoader::DataToBuffers(verts, normals, texCoords, vertIndices, normalIndices, textureIndices, _pd3dDevice, "");

    IndexedMesh triGrid = IndexedMesh();
    triGrid.meshData = gridMesh;
    triGrid.name = "GridMesh";
    _meshes.push_back(triGrid);

    //Textures
    for (size_t i = 0; i < texPaths.size(); ++i)
    {
        ID3D11ShaderResourceView* tempTex = nullptr;
        string path = texPaths.at(i);
        wstring wString = wstring(path.begin(), path.end());    //Convert from string to wstring
        CreateDDSTextureFromFile(_pd3dDevice, (L"Assets/tex" + wString + L".dds").c_str(), nullptr, &tempTex);     //Convert from wstring to wchar
        _textures.push_back({ tempTex, texPaths.at(i) });
    }

    // Terrain indices
    for (int i = 0; i < _textures.size(); ++i)
    {
        if (_textures[i].name == "DirtLight")
            terrainTexIndices[0] = i;
        else if (_textures[i].name == "Grass")
            terrainTexIndices[1] = i;
        else if (_textures[i].name == "Stone2")
            terrainTexIndices[2] = i;
        else if (_textures[i].name == "Snow")
            terrainTexIndices[3] = i;
        else if (_textures[i].name == "Blend")
            terrainTexIndices[4] = i;
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
        int matIndex = 0;
        if (texName == "Metal" || texName == "Stone")
        {
            for (int j = 0; j < _materials.size(); ++j)
            {
                if (_materials[j].name == "Shiny")
                {
                    matIndex = j;
                    break;
                }
            }
        }
        else
        {
            for (int j = 0; j < _materials.size(); ++j)
            {
                if (_materials[j].name == "Dull")
                {
                    matIndex = j;
                    break;
                }
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
        bool bill = (texName[0] == '~' ? true : false);

        //Finalise
        Transform* tempTrans = new Transform(   posMatrix, rotMatrix, scaleMatrix, bill );
        Appearance* tempApp = new Appearance(   _meshes[meshIndex].meshData,
                                                _textures[texIndex].texture,
                                                _materials[matIndex].diffuseMaterial,
                                                _materials[matIndex].ambientMaterial,
                                                _materials[matIndex].specularMaterial,
                                                trans, meshName);

        //Physics properties
        float mass = objects.at(i)["mass"];
        bool terrain = (mass == 0.0 ? true : false);

        _objects.push_back({ tempTrans, tempApp, mass, terrain });
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
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }
	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);
	if (FAILED(hr))
	{	
		pVSBlob->Release();
        return hr;
	}

    // Compile the terrain vertex shader
    ID3DBlob* pVSTBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework Terrain.fx", "VS", "vs_4_0", &pVSTBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }
    // Create the terrain vertex shader
    hr = _pd3dDevice->CreateVertexShader(pVSTBlob->GetBufferPointer(), pVSTBlob->GetBufferSize(), nullptr, &_pVertexShaderTerrain);
    if (FAILED(hr))
    {
        pVSTBlob->Release();
        return hr;
    }

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }
	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();
    if (FAILED(hr))
        return hr;

    // Compile the terrain pixel shader
    ID3DBlob* pPSTBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework Terrain.fx", "PS", "ps_4_0", &pPSTBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }
    // Create the terrain pixel shader
    hr = _pd3dDevice->CreatePixelShader(pPSTBlob->GetBufferPointer(), pPSTBlob->GetBufferSize(), nullptr, &_pPixelShaderTerrain);
    pPSTBlob->Release();
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
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &_pVertexLayout);
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

	InitShadersAndInputLayout();
    _pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear);

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
                _camera->ChangePos({ 0.0f+_objects[PLAYEROBJECT].GetTransform()->GetPos().x, 20.0f+_objects[PLAYEROBJECT].GetTransform()->GetPos().y, 0.0f+_objects[PLAYEROBJECT].GetTransform()->GetPos().z },
                    _objects[PLAYEROBJECT].GetTransform()->GetPos(),
                    { 0.0f, 0.0f, -1.0f });
                cameraMode = CAMERA_TOPDOWN;
                return true;

            case CAMERA_TOPDOWN:
                cameraMode = CAMERA_FIRST;
                ConfineCursor();
                return true;

            case CAMERA_FIRST:
                cameraMode = CAMERA_THIRD;
                return true;

            case CAMERA_THIRD:
                _camera->ChangePos({ 0.0f, 20.0f, 15.0f },
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
            _objects[PLAYEROBJECT].PlayerJump(3000.0f);
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
        _objects[PLAYEROBJECT].PlayerMove({ 0.0f, 0.0f, playerSpeed * deltaTime }, _camera->GetYaw());
    }
    if (GetAsyncKeyState('A'))
    {
        _objects[PLAYEROBJECT].PlayerMove({ -playerSpeed * deltaTime, 0.0f, 0.0f }, _camera->GetYaw());
    }
    if (GetAsyncKeyState('S'))
    {
        _objects[PLAYEROBJECT].PlayerMove({ 0.0f, 0.0f, -playerSpeed * deltaTime }, _camera->GetYaw());
    }
    if (GetAsyncKeyState('D'))
    {
        _objects[PLAYEROBJECT].PlayerMove({ playerSpeed * deltaTime, 0.0f, 0.0f }, _camera->GetYaw());
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
        _camera->AddR(-2.0f * deltaTime);
    }
    if (GetAsyncKeyState(VK_SUBTRACT))
    {
        _camera->AddR(2.0f * deltaTime);
    }
    if (GetAsyncKeyState(VK_UP))
    {
        _light->AddDirection({ 0.0f, 0.0f, -2.0f * deltaTime });
    }
    if (GetAsyncKeyState(VK_DOWN))
    {
        _light->AddDirection({ 0.0f, 0.0f, 2.0f * deltaTime });
    }
    if (GetAsyncKeyState(VK_LEFT))
    {
        _light->AddDirection({ 2.0f * deltaTime, 0.0f, 0.0f });
    }
    if (GetAsyncKeyState(VK_RIGHT))
    {
        _light->AddDirection({ -2.0f * deltaTime, 0.0f, 0.0f });
    }

    playerSpeed = (GetAsyncKeyState(VK_SHIFT) ? PLAYERSPEED * 2.0f : PLAYERSPEED);
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
    if (_pVertexShaderTerrain) _pVertexShaderTerrain->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pPixelShaderTerrain) _pPixelShaderTerrain->Release();
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

void Application::CollisionDetection(int i, float deltaTime)
{
    if (_objects[i].GetParticle()->GetCollisionType() == COLLISION_SPHERE)
    {
        for (unsigned int j = 0; j < _objects.size(); ++j)
        {
            // Collision, but only with physics objects and not with self, and make sure it doesn't happen twice
            if (i != j && !_objects[j].GetTerrain())
            {
                if (_objects[j].GetParticle()->GetCollisionType() == COLLISION_SPHERE)
                {
                    //Both spheres
                    CollisionData coll = _objects[i].GetParticle()->SphereCollision(_objects[j].GetTransform()->GetPos(), _objects[j].GetParticle()->GetRadius());
                    if (coll.isCollided)
                    {
                        CollisionResponseSphere(coll.difference, i, j, deltaTime);
                    }
                }
                else if (_objects[j].GetParticle()->GetCollisionType() == COLLISION_AABB)
                {
                    //Sphere and AABB, i is sphere
                    CollisionData coll = _objects[i].GetParticle()->SphereAABBCollision(_objects[i].GetTransform()->GetPos(), _objects[i].GetParticle()->GetRadius(), _objects[j].GetParticle()->GetCorner() + _objects[j].GetTransform()->GetPos(), _objects[j].GetParticle()->GetWidths());
                    if (coll.isCollided)
                    {
                        CollisionResponseAABBSphere(coll.difference, i, j, i, -1.0f);
                    }
                }
            }
        }
    }
    if (_objects[i].GetParticle()->GetCollisionType() == COLLISION_AABB)
    {
        for (unsigned int j = 0; j < _objects.size(); ++j)
        {
            // Collision, but only with physics objects and not with self, and make sure it doesn't happen twice
            if (i != j && !_objects[j].GetTerrain())
            { 
                if (_objects[j].GetParticle()->GetCollisionType() == COLLISION_SPHERE)
                {
                    // Sphere and AABB, i is AABB
                    CollisionData coll = _objects[i].GetParticle()->SphereAABBCollision(_objects[j].GetTransform()->GetPos(), _objects[j].GetParticle()->GetRadius(), _objects[i].GetParticle()->GetCorner() + _objects[i].GetTransform()->GetPos(), _objects[i].GetParticle()->GetWidths());
                    if (coll.isCollided)
                    {
                        CollisionResponseAABBSphere(coll.difference, i, j, j, 1.0f);
                    }
                }
                else if (_objects[j].GetParticle()->GetCollisionType() == COLLISION_AABB)
                {
                    // Both AABBs
                    CollisionData coll = _objects[i].GetParticle()->AABBCollision(_objects[i].GetParticle()->GetCorner() + _objects[i].GetTransform()->GetPos(), _objects[i].GetParticle()->GetWidths(), _objects[j].GetParticle()->GetCorner() + _objects[j].GetTransform()->GetPos(), _objects[j].GetParticle()->GetWidths());
                    if (coll.isCollided)
                    {
                        CollisionResponseAABB(coll.difference, i, j);
                    }
                }
            }
        }
    }
}

void Application::CollisionResponseSphere(Vector3D penetration, int i, int j, float deltaTime)
{
    // stackoverflow.com/questions/3232318/sphere-sphere-collision-detection-reaction
    // research.ncl.ac.uk/game/mastersdegree/gametechnologies/physicstutorials/5collisionresponse/Physics%20-%20Collision%20Response.pdf

    float massFormulaI = _objects[i].GetParticle()->GetMass() / (_objects[i].GetParticle()->GetMass() + _objects[j].GetParticle()->GetMass());
    float massFormulaJ = _objects[j].GetParticle()->GetMass() / (_objects[i].GetParticle()->GetMass() + _objects[j].GetParticle()->GetMass());

    Vector3D pene = penetration.normalization() * (_objects[i].GetParticle()->GetRadius() + _objects[j].GetParticle()->GetRadius()) - penetration;

    CollisionVelocities(i, j);

    _objects[i].GetTransform()->SetPos(_objects[i].GetTransform()->GetPos() - pene);
    _objects[j].GetTransform()->SetPos(_objects[j].GetTransform()->GetPos() + pene);
}

Vector3D Application::ProjectUOnV(Vector3D u, Vector3D v)
{
     return v * u.dot_product(v) / v.dot_product(v);
}

void Application::CollisionVelocities(int i, int j)
{
    Vector3D newI = _objects[i].GetParticle()->GetVelocity();
    newI += ProjectUOnV(_objects[j].GetParticle()->GetVelocity(), _objects[j].GetTransform()->GetPos() - _objects[i].GetTransform()->GetPos());
    newI -= ProjectUOnV(_objects[i].GetParticle()->GetVelocity(), _objects[i].GetTransform()->GetPos() - _objects[j].GetTransform()->GetPos());
    newI = (newI.magnitude() != 0.0f ? newI.normalization() : Vector3D(0.0f, 0.0f, 0.0f));

    Vector3D newJ = _objects[j].GetParticle()->GetVelocity();
    newJ += ProjectUOnV(_objects[i].GetParticle()->GetVelocity(), _objects[j].GetTransform()->GetPos() - _objects[i].GetTransform()->GetPos());
    newJ -= ProjectUOnV(_objects[j].GetParticle()->GetVelocity(), _objects[i].GetTransform()->GetPos() - _objects[j].GetTransform()->GetPos());
    newJ = (newJ.magnitude() != 0.0f ? newJ.normalization() : Vector3D(0.0f, 0.0f, 0.0f));

    _objects[i].GetParticle()->SetVelocity(newI * 16.0f / _objects[i].GetParticle()->GetMass());
    _objects[j].GetParticle()->SetVelocity(newJ * 16.0f / _objects[j].GetParticle()->GetMass());
} 

void Application::CollisionResponseAABBSphere(Vector3D penetration, int i, int j, int sphereIndex, float multiply)
{
    Directions dir = _objects[j].GetParticle()->VectorDirection(_objects[j].GetTransform()->GetPos() - _objects[i].GetTransform()->GetPos());
    float massFormulaI = _objects[i].GetParticle()->GetMass() / (_objects[i].GetParticle()->GetMass() + _objects[j].GetParticle()->GetMass());
    float massFormulaJ = _objects[j].GetParticle()->GetMass() / (_objects[i].GetParticle()->GetMass() + _objects[j].GetParticle()->GetMass());

    Vector3D norm = (penetration.magnitude() != 0.0f ? penetration.normalization() : Vector3D(0.0f, 0.0f, 0.0f));
    Vector3D pene = (norm * _objects[sphereIndex].GetParticle()->GetRadius() - penetration) * multiply;

    CollisionVelocities(i, j);

    switch (dir)
    {
    case FORWARD:
    case BACK:
        //pene = (_objects[sphereIndex].GetParticle()->GetRadius() - abs(penetration.x)) * multiply;
        //_objects[i].GetParticle()->SetVelocityX(_objects[i].GetParticle()->GetVelocity().x * -0.5f);
        if (dir == FORWARD)
        {
            _objects[i].GetTransform()->SetPosX(_objects[i].GetTransform()->GetPos().x - pene.x * massFormulaI);
            _objects[j].GetTransform()->SetPosX(_objects[j].GetTransform()->GetPos().x + pene.x * massFormulaJ);
        }
        else
        {
            _objects[i].GetTransform()->SetPosX(_objects[i].GetTransform()->GetPos().x + pene.x * massFormulaI);
            _objects[j].GetTransform()->SetPosX(_objects[j].GetTransform()->GetPos().x - pene.x * massFormulaJ);
        }
        break;
    case UP:
    case DOWN:
        //_objects[i].GetParticle()->SetVelocityY(_objects[i].GetParticle()->GetVelocity().y * -0.5f);
        if (dir == UP)
        {
            _objects[i].GetTransform()->SetPosY(_objects[i].GetTransform()->GetPos().y - pene.y * massFormulaI);
            _objects[j].GetTransform()->SetPosY(_objects[j].GetTransform()->GetPos().y + pene.y * massFormulaJ);
        }
        else
        {
            _objects[i].GetTransform()->SetPosY(_objects[i].GetTransform()->GetPos().y + pene.y * massFormulaI);
            _objects[j].GetTransform()->SetPosY(_objects[j].GetTransform()->GetPos().y - pene.y * massFormulaJ);
        }
        break;
    case LEFT:
    case RIGHT:
        //_objects[i].GetParticle()->SetVelocityZ(_objects[i].GetParticle()->GetVelocity().z * -0.5f);
        if (dir == LEFT)
        {
            _objects[i].GetTransform()->SetPosZ(_objects[i].GetTransform()->GetPos().z - pene.z * massFormulaI);
            _objects[j].GetTransform()->SetPosZ(_objects[j].GetTransform()->GetPos().z + pene.z * massFormulaJ);
        }
        else
        {
            _objects[i].GetTransform()->SetPosZ(_objects[i].GetTransform()->GetPos().z + pene.z * massFormulaI);
            _objects[j].GetTransform()->SetPosZ(_objects[j].GetTransform()->GetPos().z - pene.z * massFormulaJ);
        }
    }
}

void Application::CollisionResponseAABB(Vector3D penetration, int i, int j)
{
    Directions dir = _objects[j].GetParticle()->VectorDirection(_objects[j].GetTransform()->GetPos() - _objects[i].GetTransform()->GetPos());
    float massFormulaI = _objects[i].GetParticle()->GetMass() / (_objects[i].GetParticle()->GetMass() + _objects[j].GetParticle()->GetMass());
    float massFormulaJ = _objects[j].GetParticle()->GetMass() / (_objects[i].GetParticle()->GetMass() + _objects[j].GetParticle()->GetMass());

    CollisionVelocities(i, j);

    switch (dir)
    {
    case FORWARD:
    case BACK:
        //_objects[i].GetParticle()->SetVelocityX(_objects[i].GetParticle()->GetVelocity().x * -0.5f);
        if (dir == FORWARD)
        {
            _objects[i].GetTransform()->SetPosX(_objects[i].GetTransform()->GetPos().x - penetration.x * massFormulaI);
            _objects[j].GetTransform()->SetPosX(_objects[j].GetTransform()->GetPos().x + penetration.x * massFormulaJ);
        }
        else
        {
            _objects[i].GetTransform()->SetPosX(_objects[i].GetTransform()->GetPos().x + penetration.x * massFormulaI);
            _objects[j].GetTransform()->SetPosX(_objects[j].GetTransform()->GetPos().x - penetration.x * massFormulaJ);
        }
        break;
    case UP:
    case DOWN:
        //_objects[i].GetParticle()->SetVelocityY(_objects[i].GetParticle()->GetVelocity().y * -0.5f);
        if (dir == UP)
        {
            _objects[i].GetTransform()->SetPosY(_objects[i].GetTransform()->GetPos().y - penetration.y * massFormulaI);
            _objects[j].GetTransform()->SetPosY(_objects[j].GetTransform()->GetPos().y + penetration.y * massFormulaJ);
        }
        else
        {
            _objects[i].GetTransform()->SetPosY(_objects[i].GetTransform()->GetPos().y + penetration.y * massFormulaI);
            _objects[j].GetTransform()->SetPosY(_objects[j].GetTransform()->GetPos().y - penetration.y * massFormulaJ);
        }
        break;
    case LEFT:
    case RIGHT:
        //_objects[i].GetParticle()->SetVelocityZ(_objects[i].GetParticle()->GetVelocity().z * -0.5f);
        if (dir == LEFT)
        {
            _objects[i].GetTransform()->SetPosZ(_objects[i].GetTransform()->GetPos().z - penetration.z * massFormulaI);
            _objects[j].GetTransform()->SetPosZ(_objects[j].GetTransform()->GetPos().z + penetration.z * massFormulaJ);
        }
        else
        {
            _objects[i].GetTransform()->SetPosZ(_objects[i].GetTransform()->GetPos().z + penetration.z * massFormulaI);
            _objects[j].GetTransform()->SetPosZ(_objects[j].GetTransform()->GetPos().z - penetration.z * massFormulaJ);
        }
    }
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

        //Billboard angle
        float yaw = (cameraMode == CAMERA_FIRST || cameraMode == CAMERA_THIRD ? _camera->GetYaw() : XM_PI);
        float pitch = (cameraMode == CAMERA_FIRST || cameraMode == CAMERA_THIRD ? _camera->GetPitch() : 0);
        if (cameraMode == CAMERA_TOPDOWN)
            pitch = XM_PIDIV2;

        for (unsigned int i = 0; i < _objects.size(); ++i)
        {
            if(_objects[i].GetTerrain())
                _objects[i].GetTransform()->Update(yaw, 0.0f);
            else
            {
                _objects[i].Update(deltaTime, yaw, 0.0f);
                CollisionDetection(i, deltaTime);
            }          
        }

        _particleManager->Update(deltaTime, yaw, pitch);
        _camera->Update(cameraMode, _hWnd, _objects[PLAYEROBJECT].GetTransform()->GetPos());
        _camera->SetMonkey(_objects[PLAYEROBJECT].GetTransform()->GetPos());

        XMFLOAT3 tempFloat = { _camera->GetEye().x, _camera->GetEye().y, _camera->GetEye().z };
        _light->SetEye(tempFloat);

        _objects[PLAYEROBJECT].GetTransform()->SetRot({ _camera->GetPitch(), _camera->GetYaw(), 0.0f });

        // Sphere orbit
        /*_objects[ORBITOBJECT].SetPos({ 15*cos(orbitAngle), 15.0f+cos(orbitAngle)*2, 15*sin(orbitAngle) });
        _cb.gTime = deltaTime;
        orbitAngle += 1.0f * deltaTime;
        if (orbitAngle > XM_2PI)
            orbitAngle = 0.0f;*/

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
    float ClearColor[4] = CLEARCOLOUR;
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
    _pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    //Matrices
    _cb.mView = XMMatrixTranspose(XMLoadFloat4x4(&_camera->GetView()));
    _cb.mProjection = XMMatrixTranspose(XMLoadFloat4x4(&_camera->GetProjection()));
    _pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
    
    //Light
    _light->Draw(&_cb);

    //Shader
    _pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
    _pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);
    _pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
    _pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);

    //Wireframe mode
    _pImmediateContext->RSSetState((isSolid ? _solid : _wireFrame));

    //Don't show the monkey in 1st person camera
    int start = (cameraMode == CAMERA_FIRST ? 1 : 0);
    for (unsigned int i = start; i < _objects.size(); ++i)
    {
        if (_objects[i].GetAppearance()->GetName() == "GridMesh")
        {
            _pImmediateContext->VSSetShader(_pVertexShaderTerrain, nullptr, 0);
            _pImmediateContext->PSSetShader(_pPixelShaderTerrain, nullptr, 0);

            for (short i = 0; i < 5; ++i)
                _pImmediateContext->PSSetShaderResources(i, 1, &_textures[terrainTexIndices[i]].texture);

            if (!_objects[i].GetAppearance()->GetTransparent())
                _objects[i].Render(_cb, _pImmediateContext, _pConstantBuffer, nullptr);

            _pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
            _pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);
        }
        else
        {
            if (!_objects[i].GetAppearance()->GetTransparent())
                _objects[i].Render(_cb, _pImmediateContext, _pConstantBuffer, nullptr);
        }
    }
    _particleManager->Render(_pImmediateContext, &_cb, _pConstantBuffer);
    for (unsigned int i = start; i < _objects.size(); ++i)
    {
        if (_objects[i].GetAppearance()->GetTransparent())
            _objects[i].Render(_cb, _pImmediateContext, _pConstantBuffer, _transparency);
    }
    _pSwapChain->Present(0, 0);
}