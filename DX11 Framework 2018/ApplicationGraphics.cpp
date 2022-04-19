#include "ApplicationGraphics.h"
#include "Imgui\imgui.h"
#include "Imgui\imgui_impl_win32.h"


ApplicationGraphics::ApplicationGraphics()
{
	//Initialise Variables
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;

	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pConstantBuffer = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	skyBoxPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_wireFrame = false;
}

ApplicationGraphics::~ApplicationGraphics()
{
}

HRESULT ApplicationGraphics::Initialize(HINSTANCE hInstance, int nCmdShow)
{
	//initialise the window
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
		

		return E_FAIL;
	}

	// Initialize the world matrix
	XMStoreFloat4x4(&_world, XMMatrixIdentity());
	SkyboxRasterizerState();

	//Initialise Lighting

	//Light Direction from surface
	_lightDirection = XMFLOAT3(0.25f, 0.5f, 1.0f);

	//Diffuse material properties (RGBA)
	_diffuseMaterial = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	//Diffuse Light Colour (RGBA)
	_diffuseLight = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	//Ambient Light
	_ambientLight = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	_ambientMateral = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);

	//Specular Light
	_specularLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	_specularMaterial = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	_specularPower = 10.0f;
	//Point Light

	_pointLight.Ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	_pointLight.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_pointLight.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	_pointLight.Attenuation = XMFLOAT3(0.0f, 0.03f, 0.0f);
	_pointLight.Position = XMFLOAT3(0.0f, 0.0f, 45.0f);

	_pointLight.Range = 6000.0f;

	

	//SpotLight
	_spotLight.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	_spotLight.Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	_spotLight.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	_spotLight.Attenuation = XMFLOAT3(1.0f, 0.0f, 0.0f);
	_spotLight.Spot = 96.0f;
	_spotLight.Range = 50.0f;

	//local Material values
	_material.Ambient = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	_material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_material.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);



	return S_OK;
}

void ApplicationGraphics::UpdateLighting() {
	
	//Keeps the spotlights position and direction reletive to the active camera
	_spotLight.Position = _activeCamera->GetCameraPosition();

	_spotLight.Direction = _activeCamera->GetCameraLookAtPoint();

	XMStoreFloat3(&_spotLight.Direction, XMVector3Normalize(_activeCamera->GetLookAtVec() - _activeCamera->GetCameraVecPosition()));


	

}

void ApplicationGraphics::RunLightingControls()
{
	//Creation of the Lighting control pannel, enabling the user to change graphics settings on the fly
	ImGui::Begin("Lighting Control Panel");
	ImGui::Text("Point Light (sunlight)");
	ImGui::SliderFloat("Position X", &_pointLight.Position.x, -50.0f, 50.0f);
	ImGui::SliderFloat("Position Y", &_pointLight.Position.y, -50.0f, 50.0f);
	ImGui::SliderFloat("Position Z", &_pointLight.Position.z, -50.0f, 50.0f);
	ImGui::SliderFloat("Ambient X", &_pointLight.Ambient.x, 0.0f, 1.0f);
	ImGui::SliderFloat("Ambient Y", &_pointLight.Ambient.y, 0.0f, 1.0f);
	ImGui::SliderFloat("Ambient Z", &_pointLight.Ambient.z, 0.0f, 1.0f);
	ImGui::Spacing;
	ImGui::SliderFloat("Diffuse X", &_pointLight.Diffuse.x, 0.0f, 1.0f);

	ImGui::SliderFloat("Diffuse Y", &_pointLight.Diffuse.y, 0.0f, 1.0f);
	ImGui::SliderFloat("Diffuse Z", &_pointLight.Diffuse.z, 0.0f, 1.0f);
	ImGui::Separator;
	ImGui::SliderFloat("Specular X", &_pointLight.Specular.x, 0.0f, 1.0f);
	ImGui::SliderFloat("Specular Y", &_pointLight.Specular.y, 0.0f, 1.0f);
	ImGui::SliderFloat("Specular Z", &_pointLight.Specular.z, 0.0f, 1.0f);

	ImGui::End();

	

	ImGui::Begin("Material Value Control Panel");
	ImGui::Text("Material Values");
	ImGui::SliderFloat("Ambient X", &_material.Ambient.x, 0.0f, 1.0f);
	ImGui::SliderFloat("Ambient Y", &_material.Ambient.y, 0.0f, 1.0f);
	ImGui::SliderFloat("Ambient Z", &_material.Ambient.z, 0.0f, 1.0f);
	ImGui::SliderFloat("Diffuse X", &_material.Diffuse.x, 0.0f, 1.0f);
	ImGui::SliderFloat("Diffuse Y", &_material.Diffuse.y, 0.0f, 1.0f);
	ImGui::SliderFloat("Diffuse Z", &_material.Diffuse.z, 0.0f, 1.0f);
	ImGui::SliderFloat("Specular X", &_material.Specular.x, 0.0f, 1.0f);
	ImGui::SliderFloat("Specular Y", &_material.Specular.y, 0.0f, 1.0f);
	ImGui::SliderFloat("Specular Z", &_material.Specular.z, 0.0f, 1.0f);
	ImGui::End();
}

//External handler to make the user interface work with mouse inputs
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;
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

	return 0;
}


void ApplicationGraphics::SetVertexBuffer(ID3D11Buffer* buffer)
{
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	_pImmediateContext->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
}

void ApplicationGraphics::SetIndexBuffer(ID3D11Buffer* buffer)
{
	_pImmediateContext->IASetIndexBuffer(buffer, DXGI_FORMAT_R16_UINT, 0);
}

void ApplicationGraphics::UpdateConstantBufferVariables(XMFLOAT4X4& position)
{
	//Sets constant buffer variables for object rendering
	ConstantBuffer cb;
	XMMATRIX world =XMLoadFloat4x4(&position);
	XMMATRIX view = XMLoadFloat4x4(&_activeCamera->GetViewMatrix());
	XMMATRIX projection = XMLoadFloat4x4(&_activeCamera->GetProjectionMatrix());


	
	cb.mWorld = XMMatrixTranspose(world);
	cb.mView = XMMatrixTranspose(view);
	cb.mProjection = XMMatrixTranspose(projection);

	cb.DiffuseMtrl = _diffuseMaterial;
	cb.DiffuseLight = _diffuseLight;
	cb.LightVecW = _lightDirection;
	cb.AmbientLight = _ambientLight;
	cb.AmbientMtrl = _ambientMateral;
	cb.SpecularMtrl = _specularMaterial;
	cb.SpecularLight = _specularLight;
	cb.SpecularPower = _specularPower;
	cb.EyePosW = _activeCamera->GetCameraPosition();
	cb.PointLight.Position = _activeCamera->GetCameraPosition();

	cb.material.Ambient = _material.Ambient;
	cb.material.Diffuse = _material.Diffuse;
	cb.material.Specular = _material.Specular;


	cb.PointLight = _pointLight;
	cb.PointLight.Position = _pointLight.Position;

	
	cb.SpotLight = _spotLight;
	
	

	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
}

void ApplicationGraphics::ClearBackBuffer()
{
	// Clear the back buffer
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
	_pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

}

void ApplicationGraphics::Draw(unsigned int indexCount)
{
	_pImmediateContext->DrawIndexed(indexCount, 0, 0);
}

void ApplicationGraphics::Present()
{
	_pSwapChain->Present(0, 0);
}

void ApplicationGraphics::BindTextures(int initSlot, int count, std::vector<ID3D11ShaderResourceView*> textures)
{
	
		_pImmediateContext->PSSetShaderResources(initSlot, count, &textures[initSlot]);
	
	
}

//Key polling for wireframe 
bool ApplicationGraphics::UpdateWireFrame()
{
	if (GetAsyncKeyState(VK_F1))
	{

		if (isWireFrame == false)
		{
			//_pImmediateContext->RSSetState(_wireFrame);
			isWireFrame = true;
			return true;
			
		}
	}

	if (GetAsyncKeyState(VK_F2))
	{
		if (isWireFrame == true)
		{
			
			isWireFrame = false;
			return false;
		}
	}

	
	
}

void ApplicationGraphics::SetWireFrame(bool state) {

	if (state)
	{
		_pImmediateContext->RSSetState(_wireFrame);
	}
	else
	{
		_pImmediateContext->RSSetState(nullptr);
	}
	
}

float ApplicationGraphics::TimerUpdate(float t)
{
	if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
	{
		t += (float)XM_PI * 0.0125f;
		return t;
	}
	else
	{
		static DWORD dwTimeStart = 0;
		DWORD dwTimeCur = GetTickCount();

		if (dwTimeStart == 0)
			dwTimeStart = dwTimeCur;

		t = (dwTimeCur - dwTimeStart) / 1000.0f;
		return t;
	}
	
}



void ApplicationGraphics::SetVertexShader(ID3D11VertexShader* vs)
{
	_pImmediateContext->VSSetShader(vs, nullptr, 0);
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
}

void ApplicationGraphics::SetPixelShader(ID3D11PixelShader* ps)
{
	_pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetShader(ps, nullptr, 0);
}

ID3D11Device* ApplicationGraphics::GetDevice()
{
	return _pd3dDevice;
}

ID3D11DeviceContext* ApplicationGraphics::GetContext()
{
	return _pImmediateContext;
}

void ApplicationGraphics::SetCamera(Camera* camera)
{
	_activeCamera = camera;
}

Camera* ApplicationGraphics::GetCurrentCamera() {

	return _activeCamera;
}




XMFLOAT4X4 ApplicationGraphics::GetWorld()
{
	return _world;
}

HWND ApplicationGraphics::getWnd()
{
	return _hWnd;
}

ID3D11PixelShader* ApplicationGraphics::GetScenePixelShader()
{
	return _pPixelShader;
}

ID3D11PixelShader* ApplicationGraphics::GetSkyboxPixelShader()
{
	return skyBoxPixelShader;
}




HRESULT ApplicationGraphics::InitWindow(HINSTANCE hInstance, int nCmdShow)
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
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"TutorialWindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// Create window
	_hInst = hInstance;
	RECT rc = { 0, 0, 1920, 1080 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	_hWnd = CreateWindow(L"TutorialWindowClass", L"DX11 Framework", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
		nullptr);
	if (!_hWnd)
		return E_FAIL;

	ShowWindow(_hWnd, nCmdShow);

	return S_OK;
}




HRESULT ApplicationGraphics::InitDevice()
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

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	if (FAILED(hr))
		return hr;

	hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
	pBackBuffer->Release();

	if (FAILED(hr))
		return hr;
	//Define the depth/stencil buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = _WindowWidth;
	depthStencilDesc.Height = _WindowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	//Create the depth/Stencil Buffer
	_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
	_pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

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

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

	//Set shaders
	SetVertexShader(_pVertexShader);
	SetPixelShader(_pPixelShader);

	// Set primitive topology
	_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Set up Wireframe
	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_WIREFRAME;
	wfdesc.CullMode = D3D11_CULL_NONE;
	hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame);


	//Create Sampler
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	_pd3dDevice->CreateSamplerState(&samplerDesc, &_pSamplerLinear);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);




	if (FAILED(hr))
		return hr;

	return S_OK;
}



void ApplicationGraphics::SetEyePosW(XMFLOAT3 eyePosW)
{
	_EyePosW = eyePosW;
}

void ApplicationGraphics::SkyboxRasterizerState()
{
	//Define the rasterizer states for changing in order to render the skybox properly
	D3D11_RASTERIZER_DESC desc = {};
	desc.CullMode = D3D11_CULL_FRONT;
	desc.DepthClipEnable = true;
	desc.FillMode = D3D11_FILL_SOLID;

	_pd3dDevice->CreateRasterizerState(&desc, &_cull_front_state);

	desc.CullMode = D3D11_CULL_BACK;
	_pd3dDevice->CreateRasterizerState(&desc, &_cull_back_state);

}


void ApplicationGraphics::SetSkyboxRasterizerState(bool cull_front)
{
	if (cull_front)
	{
		_pImmediateContext->RSSetState(_cull_front_state);
	}
	else
	{
		_pImmediateContext->RSSetState(_cull_back_state);
	}
}

void ApplicationGraphics::Cleanup()
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
	//if (_wireFrame) _wireFrame->Release();
}

HRESULT ApplicationGraphics::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DCompileFromFile(szFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, szEntryPoint, szShaderModel,
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

HRESULT ApplicationGraphics::InitShadersAndInputLayout()
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

	CompileAndSetPixelShader(hr, L"DX11 Framework.fx", &_pPixelShader);
	CompileAndSetPixelShader(hr, L"SkyBoxShader.fx", &skyBoxPixelShader);
	

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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

HRESULT ApplicationGraphics::CompileAndSetPixelShader(HRESULT hr, WCHAR* fileName, ID3D11PixelShader** pShader) {
	
	

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
	hr = CompileShaderFromFile(fileName, "PS", "ps_4_0", &pPSBlob);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, pShader);
	pPSBlob->Release();


	return hr;
}