#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "DDSTextureLoader.h"
#include "OBJLoader.h"
#include "ApplicationGraphics.h"
//#include "Application.h"

#include "BreaksEngine.h"

#include <vector>

using namespace DirectX;

class SceneObject
{
public:
	SceneObject(ApplicationGraphics* graphics);
	SceneObject();
	~SceneObject();

	void SetPosition(XMFLOAT3 position);
	void SetRotation(XMFLOAT3 rotation);
	void SetScale(XMFLOAT3 scale);
	void SetScale(float x, float y, float z);
	void SetTransform(XMFLOAT4X4 transform);
	void SetTransform(XMMATRIX transform);
	void GenerateTexture(wchar_t* texturePath, ID3D11Device* device);
	void LoadModelMesh(char* filepath, ID3D11Device* device, BreaksEngine* audioEngine);
	//void SetDevice(ID3D11Device* device);
	XMMATRIX UpdateTransforms();

	XMFLOAT4X4 GetTransform();
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	XMFLOAT3 GetScale();

	virtual void Draw();
	virtual void Draw(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, UINT indexCount);
	void Update(); 
	void Initialise();

	HRESULT LoadTexture(wchar_t* path, ID3D11ShaderResourceView** texture, ID3D11Device* device);
	ApplicationGraphics* appGFX;

	MeshData* mMeshData;

private:
	//ID3D11Device* _pd3dDevice;

protected:
	

	XMFLOAT4X4 mTransform;
	XMFLOAT3 mPosition;
	XMFLOAT3 mRotation;
	XMFLOAT3 mScale;

	std::vector<ID3D11ShaderResourceView*> mTextures;


	

};

