#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include <vector>
#include "DDSTextureLoader.h"
#include "Structures.h"
#include "SceneObject.h"
#include "ApplicationGraphics.h"
#include "Time.h"
#include "Camera.h"
#include "CubeObject.h"
#include "GroundPlane.h"
#include "SetUpGUI.h"
#include "BreaksEngine.h"


using namespace DirectX;

class Application
{
private:
	
	//Hard Coded shape values
	ID3D11Buffer*           _pVertexBufferCube;
	ID3D11Buffer*           _pIndexBufferCube;
	ID3D11Buffer*			_pVertexBufferPyramid;
	ID3D11Buffer*			_pIndexBufferPyramid;
	
	SimpleVertex simpleVertex;

	//Texturing
	ID3D11ShaderResourceView* p_TextureRV = nullptr;
	ID3D11ShaderResourceView* p_SpecularTexture = nullptr;

	//Update Controls
	void UpdateShipControlls(float deltaTime);
	void UpdateCameraControlls(float deltaTime);
	
	ApplicationGraphics* appGFX;
	SceneObject* _skyMap;

	//Scene Objects (old, for hard coded values)
	//std::vector<XMFLOAT4X4> sceneObjects;

	XMFLOAT4X4 _gridPlane;

	std::vector<SceneObject*> _worldSceneObjects;

	//Scene Objects
	GroundPlane* _plane;
	SceneObject* _cube;
	SceneObject* _earth;
	SceneObject* _ship;
	SceneObject* _shipPlayer;
	SceneObject* _warehouse;
	SceneObject* _speaker;
	SceneObject* _speaker2;
	Vector3 brasilPos;

	FMOD::Geometry *wallGeometry = 0;
	

	FMOD::Reverb3D* hallReverbZone;
	UserInterface _ui;

	Camera* _camera1;
	Camera* _camera2;
	Camera* _camera3;

	float _camMoveSpeed = 5.0f;
private:
	
	bool _isWireFrame = false;
	void Cleanup();
	bool _showGridPlane;

	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();
	TimeKeep* _timer;

	//Scene Objects
	std::vector<SceneObject> _applicationObjects;

	float _rotationSpeed;
	float _earthRotationSpeed;

	float _earthRotation = 0.0f;
	float _rotation = 0.0f;
	XMFLOAT3 _offset = XMFLOAT3(0.0f, 3.0f, 5.0f);

	float _moveSpeed;
	BreaksEngine* audioEngine;

	int _currentAudioID;
	int _jukeboxChannelID;
	float jukeBoxVolume = 0.5f;

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void ShowSceneUI();

	

	void Update();
	void Draw();
};

