#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include "DDSTextureLoader.h"
#include <windowsx.h>
#include "Structures.h"
#include "Camera.h"
#include "LightStructs.h"


using namespace DirectX;

//Define constant buffer variables, these will relate to those in the shader file
struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;

	XMFLOAT4 DiffuseMtrl;
	XMFLOAT4 DiffuseLight;
	XMFLOAT3 LightVecW;

	float dataBreak;

	XMFLOAT4 AmbientLight;
	XMFLOAT4 AmbientMtrl;

	XMFLOAT4 SpecularMtrl;
	XMFLOAT4 SpecularLight;
	float SpecularPower;
	XMFLOAT3 EyePosW;

	Material material;
	PointLight PointLight;
	SpotLight SpotLight;
};



class ApplicationGraphics
{
public:
	ApplicationGraphics();
	~ApplicationGraphics();

	//Initialization
	HRESULT Initialize(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitShadersAndInputLayout();

	HRESULT CompileAndSetPixelShader(HRESULT hr, WCHAR* fileName, ID3D11PixelShader** pShader);

	//Buffers
	void SetVertexBuffer(ID3D11Buffer* buffer);
	void SetIndexBuffer(ID3D11Buffer* buffer);
	void UpdateConstantBufferVariables(XMFLOAT4X4& position);
	void ClearBackBuffer();
	

	//Drawing
	void Draw(unsigned int indexCount);
	void Present();
	void BindTextures(int initSlot, int count, std::vector<ID3D11ShaderResourceView*> textures);


	//Wireframe
	bool UpdateWireFrame();
	void SetWireFrame(bool state);
	
	float TimerUpdate(float t);


	//Shaders
	//void SetShaders(ID3D11VertexShader* vs, ID3D11PixelShader* ps);
	void SetVertexShader(ID3D11VertexShader* vs);
	void SetPixelShader(ID3D11PixelShader* ps);

	//Getters
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetContext();
	void SetCamera(Camera* camera);

	Camera* GetCurrentCamera();

	void RunLightingControls();
	
	ConstantBuffer GetCB();
	XMFLOAT4X4 GetWorld();
	HWND getWnd();


	ID3D11PixelShader* GetScenePixelShader();
	ID3D11PixelShader* GetSkyboxPixelShader();

	
	

	void SetEyePosW(XMFLOAT3 eyePosW);

	//ID3D11PixelShader* scenePixelShader;
	ID3D11PixelShader* skyBoxPixelShader;
	void SkyboxRasterizerState();
	void SetSkyboxRasterizerState(bool cull_front);
	//Lighting
	void UpdateLighting();
	//Point Light
	PointLight _pointLight;
	SpotLight _spotLight;

private:
	void Cleanup();

	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);


	//Initialisation
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();


	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*			_pd3dDevice;
	ID3D11DeviceContext*	_pImmediateContext;
	IDXGISwapChain*			_pSwapChain;
	ID3D11RenderTargetView*	_pRenderTargetView;
	ID3D11Buffer*			_pConstantBuffer;
	XMFLOAT4X4              _world;
	XMFLOAT4X4				_world2;
	XMFLOAT4X4              _view;
	XMFLOAT4X4              _projection;
	ID3D11VertexShader*		_pVertexShader;
	ID3D11PixelShader*		_pPixelShader;
	ID3D11InputLayout*	    _pVertexLayout;
	Camera*			        _activeCamera;

	ID3D11RasterizerState* _cull_front_state = nullptr;
	ID3D11RasterizerState* _cull_back_state = nullptr;

	

	//Diffuse Light
	XMFLOAT3 _lightDirection;
	XMFLOAT4 _diffuseMaterial;
	XMFLOAT4 _diffuseLight;

	//Ambient Light
	XMFLOAT4 _ambientLight;
	XMFLOAT4 _ambientMateral;

	//specular Light
	XMFLOAT4 _specularMaterial;
	XMFLOAT4 _specularLight;
	float _specularPower;
	XMFLOAT3 _EyePosW;

	Material _material;

	bool isWireFrame;
	float _keyPressTimer;
	float _keyCoolDown;

	//Depth buffer variables
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D* _depthStencilBuffer;

	UINT _WindowHeight;
	UINT _WindowWidth;

	//Wireframe
	ID3D11RasterizerState* _wireFrame;

	//Texture Sampler
	ID3D11SamplerState* _pSamplerLinear = nullptr;

	//Pixel Shaders
	



};

