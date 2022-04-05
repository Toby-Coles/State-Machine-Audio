#include "Application.h"




Application::Application()
{
	//initialise variables 
	_pVertexBufferCube = nullptr;
	_pIndexBufferCube = nullptr;
	_pVertexBufferPyramid = nullptr;
	_pIndexBufferPyramid = nullptr;

	appGFX = nullptr;
	_skyMap = nullptr;
	_cube = nullptr;
	_earth = nullptr;
	_camera1 = nullptr;

}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
	//Initialise graphics object, used for referencing its functionability to be used in the application
	appGFX = new ApplicationGraphics();
	appGFX->Initialize(hInstance, nCmdShow);

	_ui = UserInterface(appGFX);
	
	//Create and set variables for the camera object and set the current active camera in the program
	_camera1 = new Camera();
	   

	_camera1->SetCameraPosition(XMFLOAT3(0.0f, 0.0f, 15.5f));
	_camera1->LookAt(_camera1->GetCameraPosition(), XMFLOAT3(0.0f, 0.0f, 1.0f), _camera1->GetCameraUp());
	_camera1->SetLens(90.0f, 1920 /1080, 0.01f, 1000.0f);

	

	//Create the object for the crate cube in the scene 
	_cube = new SceneObject(appGFX);
	_cube->LoadModelMesh("Models/cube.obj", appGFX->GetDevice());
	_cube->SetPosition(XMFLOAT3(15.3f, 0.2f, 0.1f));
	_cube->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	_cube->SetRotation(XMFLOAT3(0.1f, 0.1f, 0.1f));
	_cube->GenerateTexture(L"Textures/Crate_COLOR.dds", appGFX->GetDevice());
	_cube->GenerateTexture(L"Textures/Crate_SPEC.dds", appGFX->GetDevice());
	_worldSceneObjects.push_back(_cube);

	////Create the earth object in the scene
	//_earth = new SceneObject(appGFX);
	//_earth->LoadModelMesh("Models/sphere2.obj", appGFX->GetDevice());
	//_earth->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	//_earth->SetScale(XMFLOAT3(10.0f, 10.0f, 10.0f));
	//_earth->GenerateTexture(L"Textures/earth_color.dds", appGFX->GetDevice());
	//_earth->GenerateTexture(L"Textures/earth_spec.dds", appGFX->GetDevice());
	//_earth->GenerateTexture(L"Textures/earth_night.dds", appGFX->GetDevice());
	//_worldSceneObjects.push_back(_earth);

	_ship = new SceneObject(appGFX);
	_ship->LoadModelMesh("Models/userModel.obj", appGFX->GetDevice());
	_ship->SetPosition(XMFLOAT3(0.0f, 0.0f, - 25.0f));
	_ship->SetScale(XMFLOAT3(0.3f, 0.3f, 0.3f));
	_ship->GenerateTexture(L"Textures/shipTex.dss", appGFX->GetDevice());
	_worldSceneObjects.push_back(_ship);

	_shipPlayer = new SceneObject(appGFX);
	_shipPlayer->LoadModelMesh("Models/userModelRotated.obj", appGFX->GetDevice());
	_shipPlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 40.0f));
	_shipPlayer->SetScale(XMFLOAT3(0.3f, 0.3f, 0.3f));
	_shipPlayer->GenerateTexture(L"Textures/shipTex.dss", appGFX->GetDevice());
	_worldSceneObjects.push_back(_shipPlayer);

	_camera2 = new Camera();
	_camera2->SetCameraPosition(XMFLOAT3(0.0f, 0.0f, 15.0f));
	_camera2->LookAt(_camera2->GetCameraPosition(), XMFLOAT3(0.0f, 0.0f, 1.0f), _camera2->GetCameraUp());
	_camera2->SetLens(90.0f, 1920 / 1080, 0.01f, 1000.0f);
	_camera2->UpdateViewMatrix();

	_camera3 = new Camera();
	_camera3->SetCameraPosition(XMFLOAT3(20.0f, 22.0f, 23.0f));
	_camera3->SetLens(90.0f, 1920 / 1080, 0.01f, 1000.0f);
	_camera3->LookAt(XMFLOAT3(20.0f, 22.0f, 23.0f), XMFLOAT3(0.0f, -0.72f, -0.51f), XMFLOAT3(0.51f, 0.68f, 0.5f));
	//camera3->SetCameraUp(XMFLOAT3(0.51f, 0.68f, -0.5f));
	_camera3->UpdateViewMatrix();
	//camera right = XMFLOAT3(-0.7, -1, 0.7);

	//Initialise the view matrix for the camera
	_camera1->UpdateViewMatrix();
	appGFX->SetEyePosW(_camera1->GetCameraPosition());

	
	//Create the object and initialise the variables for the skybox(skysphere)
	_skyMap = new SceneObject(appGFX);
	_skyMap->LoadModelMesh("Models/sphere2.obj", appGFX->GetDevice());
	_skyMap->SetPosition(XMFLOAT3(0.0f, 0.0f, 5.5f));
	_skyMap->SetScale(XMFLOAT3(100.0f, 100.0f, 100.0f));
	_skyMap->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
	_skyMap->GenerateTexture(L"Textures/stars_map.dds", appGFX->GetDevice());

	//Creates the ground plane
	_plane = new GroundPlane(appGFX);
	_plane->GeneratePlane(30.0f, 30.0f, 8, 8);
	_plane->SetPosition (XMFLOAT3(0.0f, -10.0f, 0.0f));
	_plane->GenerateTexture(L"Textures/planeSurface.dds", appGFX->GetDevice());
	_showGridPlane = true;

	_warehouse = new SceneObject(appGFX);
	_warehouse->LoadModelMesh("Models/sponza.obj", appGFX->GetDevice());
	_warehouse->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	_warehouse->SetScale(XMFLOAT3(0.01, 0.01, 0.01));
	_warehouse->GenerateTexture(L"Textures/planeSurface.dds", appGFX->GetDevice());
	_worldSceneObjects.push_back(_warehouse);

	_speaker = new SceneObject(appGFX);
	_speaker->LoadModelMesh("Models/SpeakerStudio.obj", appGFX->GetDevice());
	_speaker->SetPosition(XMFLOAT3(0.0f, -10.0f, 0.0f));
	_speaker->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	_speaker->GenerateTexture(L"Textures/factorytexture.dds", appGFX->GetDevice());
	_worldSceneObjects.push_back(_speaker);
	
	//Initialise the timer in the program
	_timer = new TimeKeep();
	_timer->Reset();
	_timer->Start();

	_moveSpeed = 3.0f;
	
	audioEngine->Initialize();
	_currentAudioID = 0;
	SoundData backgroundMusic;
	backgroundMusic.fileName = ("Resources/TestSong.wav");
	Vector3 soundPos;

	soundPos.x = 0.0f; soundPos.y = 0.0f; soundPos.z = 0.0f;
	audioEngine->RegisterSound(backgroundMusic, true); //ID 0
	

	//Set rotation values
	_rotation = 0.0f;
	_rotationSpeed = 0.5f;
	_earthRotationSpeed = 0.1f;
	appGFX->SetCamera(_camera1);
	return S_OK;
}


HRESULT Application::InitVertexBuffer()
{
	//GenerateGridPlain(3.0f, 3.0f, 6, 4);

	//Hard coded vertex buffer for a cube - NOT IMPLEMENTED INTO SCENE HERE TO DEMONSTRATE UNDERSTANDING AND ABILITY
	HRESULT hrCube;
	SimpleVertex verticesCube[] = {
	
		{ XMFLOAT3(-1.0,  1.0, -1.0),		XMFLOAT3(0.0,  0.0, -1.0),		XMFLOAT2(0.0, 0.0) },	//Top Back Left
		{ XMFLOAT3(1.0,  1.0, -1.0),		XMFLOAT3(0.0,  0.0, -1.0),		XMFLOAT2(1.0, 0.0) },  	//Top Back Right
		{ XMFLOAT3(-1.0, -1.0, -1.0),		XMFLOAT3(0.0,  0.0, -1.0),		XMFLOAT2(0.0, 1.0) },  	//Bottom Back Left
		{ XMFLOAT3(-1.0, -1.0, -1.0),		XMFLOAT3(0.0,  0.0, -1.0),		XMFLOAT2(0.0, 1.0) },  	//Bottom Back Left
		{ XMFLOAT3(1.0,  1.0, -1.0),		XMFLOAT3(0.0,  0.0, -1.0),		XMFLOAT2(1.0, 0.0) },  	//Top Back Right
		{ XMFLOAT3(1.0, -1.0, -1.0),		XMFLOAT3(0.0,  0.0, -1.0),		XMFLOAT2(1.0, 1.0) },  	//Bottom Back Left
		{ XMFLOAT3(1.0,  1.0, -1.0),		XMFLOAT3(1.0,  0.0,  0.0),		XMFLOAT2(0.0, 0.0) }, 	//Top Back Right
		{ XMFLOAT3(1.0,  1.0,  1.0),		XMFLOAT3(1.0,  0.0,  0.0),		XMFLOAT2(1.0, 0.0) },  	//Top Front Right
		{ XMFLOAT3(1.0, -1.0, -1.0),		XMFLOAT3(1.0,  0.0,  0.0),		XMFLOAT2(0.0, 1.0) },  	//Bottom Back Right
		{ XMFLOAT3(1.0, -1.0, -1.0),		XMFLOAT3(1.0,  0.0,  0.0),		XMFLOAT2(0.0, 1.0) },  	//Top Back Right
		{ XMFLOAT3(1.0,  1.0,  1.0),		XMFLOAT3(1.0,  0.0,  0.0),		XMFLOAT2(1.0, 0.0) },  	//Top Front Right
		{ XMFLOAT3(1.0, -1.0,  1.0),		XMFLOAT3(1.0,  0.0,  0.0),		XMFLOAT2(1.0, 1.0) },  	//Bottom Front Right
		{ XMFLOAT3(1.0,  1.0,  1.0),		XMFLOAT3(0.0,  0.0,  1.0),		XMFLOAT2(0.0, 0.0) },  	//Top Front Right
		{ XMFLOAT3(-1.0,  1.0,  1.0),		XMFLOAT3(0.0,  0.0,  1.0),		XMFLOAT2(1.0, 0.0) },  	//Top Front Left
		{ XMFLOAT3(1.0, -1.0,  1.0),		XMFLOAT3(0.0,  0.0,  1.0),		XMFLOAT2(0.0, 1.0) },  	//Bottom Front Right
		{ XMFLOAT3(1.0, -1.0,  1.0),		XMFLOAT3(0.0,  0.0,  1.0),		XMFLOAT2(0.0, 1.0) },  	//Bottom Front Right
		{ XMFLOAT3(-1.0,  1.0,  1.0),		XMFLOAT3(0.0,  0.0,  1.0),		XMFLOAT2(1.0, 0.0) },  	//Top Front Left
		{ XMFLOAT3(-1.0, -1.0,  1.0),		XMFLOAT3(0.0,  0.0,  1.0),		XMFLOAT2(1.0, 1.0) },  	//Bottom Front Left
		{ XMFLOAT3(-1.0,  1.0,  1.0),		XMFLOAT3(-1.0,  0.0,  0.0),		XMFLOAT2(0.0, 0.0) }, 	//Top Front Left
		{ XMFLOAT3(-1.0,  1.0, -1.0),		XMFLOAT3(-1.0,  0.0,  0.0),		XMFLOAT2(1.0, 0.0) }, 	//Top Back Left
		{ XMFLOAT3(-1.0, -1.0,  1.0),		XMFLOAT3(-1.0,  0.0,  0.0),		XMFLOAT2(0.0, 1.0) }, 	//Bottom Front Left
		{ XMFLOAT3(-1.0, -1.0,  1.0),		XMFLOAT3(-1.0,  0.0,  0.0),		XMFLOAT2(0.0, 1.0) }, 	//Bottom Front Left
		{ XMFLOAT3(-1.0,  1.0, -1.0),		XMFLOAT3(-1.0,  0.0,  0.0),		XMFLOAT2(1.0, 0.0) }, 	//Top Back Left
		{ XMFLOAT3(-1.0, -1.0, -1.0),		XMFLOAT3(-1.0,  0.0,  0.0),		XMFLOAT2(1.0, 1.0) }, 	//Bottom Back Left
		{ XMFLOAT3(-1.0,  1.0,  1.0),		XMFLOAT3(0.0,  1.0,  0.0),		XMFLOAT2(0.0, 0.0) }, 	//Top Front Left
		{ XMFLOAT3(1.0,  1.0,  1.0),		XMFLOAT3(0.0,  1.0,  0.0),		XMFLOAT2(1.0, 0.0) },  	//Top Front Right
		{ XMFLOAT3(-1.0,  1.0, -1.0),		XMFLOAT3(0.0,  1.0,  0.0),		XMFLOAT2(0.0, 1.0) },  	//Top Back Left
		{ XMFLOAT3(-1.0,  1.0, -1.0),		XMFLOAT3(0.0,  1.0,  0.0),		XMFLOAT2(0.0, 1.0) },  	//Top Back Left
		{ XMFLOAT3(1.0,  1.0,  1.0),		XMFLOAT3(0.0,  1.0,  0.0),		XMFLOAT2(1.0, 0.0) },  	//Top Front Right
		{ XMFLOAT3(1.0,  1.0, -1.0),		XMFLOAT3(0.0,  1.0,  0.0),		XMFLOAT2(1.0, 1.0) },  	//Top Back Left
		{ XMFLOAT3(-1.0, -1.0, -1.0),		XMFLOAT3(0.0, -1.0,  0.0),		XMFLOAT2(0.0, 0.0) },  	//Bottom Back Left
		{ XMFLOAT3(1.0, -1.0, -1.0),		XMFLOAT3(0.0, -1.0,  0.0),		XMFLOAT2(1.0, 0.0) },  	//Bottom Back Right
		{ XMFLOAT3(-1.0, -1.0,  1.0),		XMFLOAT3(0.0, -1.0,  0.0),		XMFLOAT2(0.0, 1.0) },  	//Bottom Front Left
		{ XMFLOAT3(-1.0, -1.0,  1.0),		XMFLOAT3(0.0, -1.0,  0.0),		XMFLOAT2(0.0, 1.0) },  	//Bottom Front Left
		{ XMFLOAT3(1.0, -1.0, -1.0),		XMFLOAT3(0.0, -1.0,  0.0),		XMFLOAT2(1.0, 0.0) },  	//Bottom Back Right
		{ XMFLOAT3(1.0, -1.0,  1.0),		XMFLOAT3(0.0, -1.0,  0.0),		XMFLOAT2(1.0, 1.0) },	//Bottom Front Right


	};

	//Create the buffer for the cube
	D3D11_BUFFER_DESC bdCube;
	ZeroMemory(&bdCube, sizeof(bdCube));
	bdCube.Usage = D3D11_USAGE_DEFAULT;
	bdCube.ByteWidth = sizeof(verticesCube);
	bdCube.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bdCube.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitDataC;
	ZeroMemory(&InitDataC, sizeof(InitDataC));
	InitDataC.pSysMem = verticesCube;

	hrCube = appGFX->GetDevice()->CreateBuffer(&bdCube, &InitDataC, &_pVertexBufferCube);

	if (FAILED(hrCube))
		return hrCube;

	//Create vertex buffer for pyramid
	HRESULT hrPyramid;
	SimpleVertex verticesPyramid[]{
		//Base
		{XMFLOAT3(-1.0f, 0.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, -1.0f)},
		{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f)},
		{XMFLOAT3(1.0f,-1.0f,1.0f)	, XMFLOAT3(1.0f,-1.0f,1.0f)},
		{XMFLOAT3(1.0f,-1.0f,-1.0f)	, XMFLOAT3(1.0f,-1.0f,-1.0f)},

		//Point
		{XMFLOAT3(0.0f,1.0f,0.0f) ,XMFLOAT3(0.0f,1.0f,0.0f) },

	};

	D3D11_BUFFER_DESC bdPyramid;
	ZeroMemory(&bdPyramid, sizeof(bdPyramid));
	bdPyramid.Usage = D3D11_USAGE_DEFAULT;
	bdPyramid.ByteWidth = sizeof(verticesPyramid);
	bdPyramid.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bdPyramid.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitDataP;
	ZeroMemory(&InitDataP, sizeof(InitDataP));
	InitDataP.pSysMem = verticesPyramid;

	hrPyramid = appGFX->GetDevice()->CreateBuffer(&bdPyramid, &InitDataP, &_pVertexBufferPyramid);

	if (FAILED(hrPyramid))
		return hrPyramid;




	return S_OK;
}


HRESULT Application::InitIndexBuffer()
{
	//Hard coded index buffers - NOT IMPLEMENTED INTO SCENE HERE TO DEMONSTRATE UNDERSTANDING AND ABILITY
	HRESULT hrCube;

	// Create index buffer for Cube
	WORD indicesCube[] =
	{
		0,1,2 ,3,4,5,
		6,7,8 ,9,10,11,
		12,13,14 ,15,16,17,
		18,19,20 ,21,22,23,
		24,25,26 ,27,28,29,
		30,31,32 ,33,34,35
	};

	D3D11_BUFFER_DESC bdCube;
	ZeroMemory(&bdCube, sizeof(bdCube));

	bdCube.Usage = D3D11_USAGE_DEFAULT;
	bdCube.ByteWidth = sizeof(indicesCube);
	bdCube.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bdCube.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indicesCube;
	hrCube = appGFX->GetDevice()->CreateBuffer(&bdCube, &InitData, &_pIndexBufferCube);

	HRESULT hrPyramid;

	//Create index buffer for Pyramid
	WORD indicesPyramid[]{
		0, 2, 1,
		1, 2, 3,
		0, 1, 4,
		1, 3, 4,
		3, 2, 4,
		2, 0, 4,
	};

	D3D11_BUFFER_DESC bdPyramid;
	ZeroMemory(&bdPyramid, sizeof(bdPyramid));

	bdPyramid.Usage = D3D11_USAGE_DEFAULT;
	bdPyramid.ByteWidth = sizeof(indicesPyramid);
	bdPyramid.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bdPyramid.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitDataP;
	ZeroMemory(&InitDataP, sizeof(InitDataP));
	InitDataP.pSysMem = indicesPyramid;
	hrPyramid = appGFX->GetDevice()->CreateBuffer(&bdPyramid, &InitDataP, &_pIndexBufferPyramid);

	if (FAILED(hrPyramid))
		return hrPyramid;

	return S_OK;
}

void Application::Cleanup()
{
	//Cleanup memory
	if (_pVertexBufferCube) _pVertexBufferCube->Release();
	if (_pIndexBufferCube) _pIndexBufferCube->Release();
	if (_pVertexBufferCube) _pVertexBufferPyramid->Release();
	if (_pIndexBufferCube) _pIndexBufferPyramid->Release();
	if (p_TextureRV) p_TextureRV->Release();
	if (p_SpecularTexture) p_SpecularTexture->Release();
	
}

void Application::Update()
{
	appGFX->UpdateLighting();

	// Update our time
	_timer->Tick();
	float deltaTime = _timer->DeltaTime();


	//Updates the rotation values so they are constant
	//_rotation += (_rotationSpeed * deltaTime);
	//_earthRotation += (_earthRotationSpeed * deltaTime);

	audioEngine->Update(deltaTime);

	//Sets the EyePosw for rendering to that of the active camera
	appGFX->SetEyePosW(appGFX->GetCurrentCamera()->GetCameraPosition());

	//_cube->SetRotation(XMFLOAT3(_rotation, 0.3f, 0.3f));

	//_earth->SetRotation (XMFLOAT3(0.0f, _earthRotation, 0.0f ));

	//Update Scene Objects
	for each (SceneObject* object in _worldSceneObjects)
	{
		object->Update();
	}

	//Set camera 2/'s position to the ship object with a reletive offset
	_camera2->SetPosition(
		_shipPlayer->GetPosition().x + _offset.x,
		_shipPlayer->GetPosition().y + _offset.y,
		_shipPlayer->GetPosition().z + _offset.z );

	//Rotates the ship thats flying around earth
	_ship->SetRotation(XMFLOAT3(0.0f, _rotation, 0.0f));

	//Constantly sets the skymaps position reletive to the active camera to give the illusion of it never moving
	_skyMap->SetPosition(appGFX->GetCurrentCamera()->GetCameraPosition());
	_skyMap->Update();

	_plane->Update();

	_isWireFrame = appGFX->UpdateWireFrame();

	//Updates all camera control inputs
	UpdateCameraControlls(deltaTime);

	//Updates ship control inputs
	UpdateShipControlls(deltaTime);
	
}

void Application::UpdateShipControlls(float deltaTime) {
	XMFLOAT3 shipPosition = _shipPlayer->GetPosition();
	XMFLOAT3 shipRotation = _shipPlayer->GetRotation();

	if (GetAsyncKeyState('I'))
	{
		shipPosition.z += -_moveSpeed * deltaTime;
	}
	else if (GetAsyncKeyState('K'))
	{
		shipPosition.z += _moveSpeed * deltaTime;
	}
	if (GetAsyncKeyState('J'))
	{
		shipPosition.x += _moveSpeed * deltaTime;
	}
	else if (GetAsyncKeyState('L'))
	{
		shipPosition.x += -_moveSpeed * deltaTime;
	}
	if (GetAsyncKeyState('O'))
	{
		shipRotation.y += _moveSpeed * deltaTime;
	}

	//Position update
	if (shipPosition.x != _shipPlayer->GetPosition().x
		|| shipPosition.z != _shipPlayer->GetPosition().z)
		_shipPlayer->SetPosition(shipPosition);

	//Rotation Update
	if (shipRotation.x != _shipPlayer->GetRotation().x
		|| shipRotation.y != _shipPlayer->GetRotation().y ||
		shipRotation.z != _shipPlayer->GetRotation().z)
		_shipPlayer->SetRotation(shipRotation);

}
void Application::UpdateCameraControlls(float deltaTime)
{
	//Camera controlls for W, A, S and D

	//W - S
	if (GetAsyncKeyState('W')) _camera1->MoveFowardBack(5.0f * deltaTime);
	else if (GetAsyncKeyState('S')) _camera1->MoveFowardBack(-5.0f * deltaTime);
	
	//A - D
	if (GetAsyncKeyState('A')) _camera1->Strafe(-5.0f * deltaTime);
	else if (GetAsyncKeyState('D')) _camera1->Strafe(5.0f * deltaTime);
	
	//Q-E
	if (GetAsyncKeyState('Q')) _camera1->RotateY(-5.0f * deltaTime);
	else if (GetAsyncKeyState('E')) _camera1->RotateY(5.0f * deltaTime);
	
	//R-F
	if (GetAsyncKeyState('R')) _camera1->Pitch(-5.0f * deltaTime);
	else if (GetAsyncKeyState('F')) _camera1->Pitch(5.0f * deltaTime);

	// ================= Camera Selection ================= //

	if (GetAsyncKeyState('1')) appGFX->SetCamera(_camera1);
	appGFX->SetEyePosW(_camera1->GetCameraPosition());

	if (GetAsyncKeyState('2')) appGFX->SetCamera(_camera2);
	appGFX->SetEyePosW(_camera2->GetCameraPosition());

	if (GetAsyncKeyState('3')) appGFX->SetCamera(_camera3);
	appGFX->SetEyePosW(_camera3->GetCameraPosition());


	_camera1->UpdateViewMatrix();
	_camera2->UpdateViewMatrix();
	_camera3->UpdateViewMatrix();
}



void Application::ShowSceneUI()
{
	// The definition of the scene UI
	//XMFLOAT3 earthScale = XMFLOAT3(_earth->GetScale());
	//XMFLOAT3 earthPosition = XMFLOAT3(_earth->GetPosition());
	XMFLOAT3 shipOrbiterScale = XMFLOAT3(_ship->GetScale());
	XMFLOAT3 shipPosition = XMFLOAT3(_ship->GetPosition());

	

	Vector3 soundPos;
	soundPos.x = 0.0f; soundPos.y = 0.0f; soundPos.z = 0.0f;

	ImGui::Begin("Scene Object Control Panel");
	ImGui::Text("Jukebox Controls");
	if (ImGui::Button("Play")) 
	{
		_jukeboxChannelID = audioEngine->PlayAudio(_currentAudioID, soundPos, 0.5);
	}
	if (ImGui::Button("Stop")) {
		audioEngine->StopBreaksChannel(_jukeboxChannelID);
	}

	ImGui::SliderFloat("JukeboxVolume", &jukeBoxVolume, 0.0f, 1.0f);
	audioEngine->SetBreaksChannelVolume(_jukeboxChannelID, jukeBoxVolume);

	//ImGui::SliderFloat("Earth Scale X", &earthScale.x, 0.0f, 50.0f);
	/*ImGui::SliderFloat("Earth Scale Y", &earthScale.y, 0.0f, 50.0f);
	ImGui::SliderFloat("Earth Scale Z", &earthScale.z, 0.0f, 50.0f);
	ImGui::SliderFloat("Earth Rotation", &_earthRotationSpeed, 0.0f, 2.0f);
	ImGui::SliderFloat("Position X", &earthPosition.x, -50.0f, 50.0f);
	ImGui::SliderFloat("Position Y", &earthPosition.y, -50.0f, 50.0f);
	ImGui::SliderFloat("Position Z", &earthPosition.z, -50.0f, 50.0f);*/
	ImGui::Text("SpaceShip(Orbiter)");
	ImGui::SliderFloat("Ship Scale X", &shipOrbiterScale.x, 0.0f, 50.0f);
	ImGui::SliderFloat("Ship Scale Y", &shipOrbiterScale.y, 0.0f, 50.0f);
	ImGui::SliderFloat("Ship Scale Z", &shipOrbiterScale.z, 0.0f, 50.0f);
	ImGui::SliderFloat("Ship Rotation", &_rotationSpeed, 0.0f, 2.0f);
	ImGui::SliderFloat("Ship Position X", &shipPosition.x, -50.0f, 50.0f);
	ImGui::SliderFloat("Ship Position Y", &shipPosition.y, -50.0f, 50.0f);
	ImGui::SliderFloat("ship Position Z", &shipPosition.z, -50.0f, 50.0f);

	ImGui::Text("Grid Plane");
	if (ImGui::Button("Show Grid Plane"))
	{
		if (_showGridPlane == false)
		{
			_showGridPlane = true;
		}
		else
		{
			_showGridPlane = false;
		}
	}

	XMFLOAT3 planeScale = _plane->GetScale();


	ImGui::SliderFloat("Grid Plane Scale X", &planeScale.x, 0.0f, 50.0f);
	
	ImGui::SliderFloat("Grid Plane Scale Z", &planeScale.z, 0.0f, 50.0f);
	ImGui::End();

	_plane->SetScale(planeScale);
	ImGui::Begin("Controls");
	ImGui::Text("===/ Camera \===");
	ImGui::Text("W: Fly Fowards");
	ImGui::Text("A: Fly Left");
	ImGui::Text("S: Fly Backwards");
	ImGui::Text("D: Fly Right");
	ImGui::Text("Q: Rotate Left");
	ImGui::Text("E: Rotate Right");
	ImGui::Text("R: Pitch Up");
	ImGui::Text("F: Pitch Down");
	ImGui::Text("===/ Switch Camera \===");
	ImGui::Text("1: Flying Camera");
	ImGui::Text("2: Third Person follow");
	ImGui::Text("3: Top-Down Camera");
	ImGui::Text("F2: Wireframe");
	ImGui::Text("Z: Flashlight (SpotLight)");
	ImGui::Text("===/ Ship Controls \===");
	ImGui::Text("I: Forwards");
	ImGui::Text("K: Backwards");
	ImGui::Text("J: Left");
	ImGui::Text("L: Right");
	
	ImGui::End();

	////Sets the data that may have been altered by the UI
	//_earth->SetScale(earthScale);
	//_earth->SetPosition(earthPosition);
	//_ship->SetScale(shipOrbiterScale);
	//_ship->SetPosition(shipPosition);
}

void Application::Draw()
{
	//Change the rasterizer state to render textures on the outside of the objects
	_ui.FrameBegin();
	
	appGFX->SetSkyboxRasterizerState(false);
	
	ShowSceneUI();
	appGFX->ClearBackBuffer();
	//Draw objects
	appGFX->SetPixelShader(appGFX->GetScenePixelShader());

	appGFX->SetWireFrame(_isWireFrame);

	for each (SceneObject* object in _worldSceneObjects)
	{
		object->Draw();
	}

	if (_showGridPlane)
	{
		_plane->Draw(_plane->GetPlaneVb(), _plane->GetPlaneIb(), _plane->GetPlaneIndexCount());

	}

	appGFX->RunLightingControls();

	//Set the rasterizer state to draw the textures on the inside of the model for the skybox

	appGFX->SetSkyboxRasterizerState(true);
	

	appGFX->SetPixelShader(appGFX->GetSkyboxPixelShader());

	_skyMap->Draw();

	//skyMap->Draw();
	_ui.FrameEnd();

	appGFX->Present();



	
}