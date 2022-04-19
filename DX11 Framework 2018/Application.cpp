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

	audioEngine = new BreaksEngine();
	audioEngine->Initialize();

	_camera1->SetCameraPosition(XMFLOAT3(0.0f, -3.0f, 15.5f));
	_camera1->LookAt(_camera1->GetCameraPosition(), XMFLOAT3(0.0f, 0.0f, 0.0f), _camera1->GetCameraUp());
	_camera1->SetLens(90.0f, 1920 /1080, 0.01f, 1000.0f);

	//Create the object for the crate cube in the scene 
	_cube = new SceneObject(appGFX);
	_cube->LoadModelMesh("Models/cube.obj", appGFX->GetDevice(), audioEngine);
	_cube->SetPosition(XMFLOAT3(15.3f, 0.2f, 0.1f));
	_cube->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	_cube->SetRotation(XMFLOAT3(0.1f, 0.1f, 0.1f));
	_cube->GenerateTexture(L"Textures/Crate_COLOR.dds", appGFX->GetDevice());
	_cube->GenerateTexture(L"Textures/Crate_SPEC.dds", appGFX->GetDevice());
	_worldSceneObjects.push_back(_cube);

	//Initialise the view matrix for the camera
	_camera1->UpdateViewMatrix();
	appGFX->SetEyePosW(_camera1->GetCameraPosition());

	
	//Create the object and initialise the variables for the skybox(skysphere)
	_skyMap = new SceneObject(appGFX);
	_skyMap->LoadModelMesh("Models/sphere2.obj", appGFX->GetDevice(), audioEngine);
	_skyMap->SetPosition(XMFLOAT3(0.0f, 0.0f, 5.5f));
	_skyMap->SetScale(XMFLOAT3(100.0f, 100.0f, 100.0f));
	_skyMap->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
	_skyMap->GenerateTexture(L"Textures/Skymap.dds", appGFX->GetDevice());

	_warehouse = new SceneObject(appGFX);
	_warehouse->LoadModelMesh("Models/testscene.obj", appGFX->GetDevice(), audioEngine);
	_warehouse->SetPosition(XMFLOAT3(0.0f, -5.0f, 0.0f));
	_warehouse->SetScale(XMFLOAT3(0.8f,0.8f,0.8f));
	_warehouse->GenerateTexture(L"Textures/planeSurface.dds", appGFX->GetDevice());
	_worldSceneObjects.push_back(_warehouse);
	
	_speaker = new SceneObject(appGFX);
	_speaker->LoadModelMesh("Models/SpeakerSoloTri.obj", appGFX->GetDevice(), audioEngine);
	_speaker->SetPosition(XMFLOAT3(25.0f, -5.1f, -20.0f));
	_speaker->SetScale(XMFLOAT3(1.2f, 1.2f, 1.2f));
	_speaker->GenerateTexture(L"Textures/white.dds", appGFX->GetDevice());
	_worldSceneObjects.push_back(_speaker);
	
	_speaker2 = new SceneObject(appGFX);
	_speaker2->LoadModelMesh("Models/SpeakerSoloTri.obj", appGFX->GetDevice(), audioEngine);
	_speaker2->SetPosition(XMFLOAT3(25.0f, -5.1f, -30.0f));
	_speaker2->SetScale(XMFLOAT3(1.2f, 1.2f, 1.2f));
	_speaker2->GenerateTexture(L"Textures/white.dds", appGFX->GetDevice());
	_worldSceneObjects.push_back(_speaker2);

	//Initialise the timer in the program
	_timer = new TimeKeep();
	_timer->Reset();
	_timer->Start();

	_moveSpeed = 3.0f;


	hallReverbZone = audioEngine->CreateReverb(Vector3{ -70.0f, -5.1f, -30.0f }, FMOD_PRESET_ALLEY);
	_currentAudioID = 1;
	

	audioEngine->CreateFmodGeometry(wallGeometry, 200, 1200);

	FMOD_VECTOR rectangle[4] = {{ -100, -100, 0}, { -100, 100, 0}, { 100,100, 0}, { 10, -100, 0} };
	//FMOD_VECTOR* verts = new FMOD_VECTOR[_warehouse->mMeshData->fmodVerts.size()];
	//verts = _warehouse->mMeshData.fmodVerts;
	int index = 0;
	
	FMOD_RESULT  result = wallGeometry->addPolygon(1.0f, 1.0f, true, 4, rectangle, &index);
		
	
	FMOD_VECTOR worldPos = { 25.0f, -5.0f, -10.0f };
	wallGeometry->setPosition(&worldPos);

	FMOD_VECTOR occlusionRotate = { 0, 0, -1 }; // rotation object geometry
	FMOD_VECTOR occlusionUp = { 0, 1, 0 };

	wallGeometry->setRotation(&occlusionRotate, &occlusionUp);
	wallGeometry->setActive(true);

	SoundData backgroundMusic;
	backgroundMusic.fileName = ("Resources/TestSong.wav");
	Vector3 soundPos;
	soundPos.x = 0.0f; soundPos.y = 0.0f; soundPos.z = 0.0f;

	SoundData speakerMusic;
	speakerMusic.fileName = ("Resources/Brasil.wav");
	
	brasilPos.x = (25.0f); brasilPos.y = (-5.1f); brasilPos.z = (-25.0f);
	audioEngine->RegisterSound(speakerMusic, true);
	audioEngine->PlayAudio(0, brasilPos, 10);

	
	audioEngine->RegisterSound(backgroundMusic, true); //ID 1
	

	for (int i = 0; i < _worldSceneObjects.size(); i++) {
		FMOD::Geometry* g = _worldSceneObjects[i]->mMeshData->fmodGeometry;
		FMOD_VECTOR v = { _worldSceneObjects[i]->GetPosition().x,_worldSceneObjects[i]->GetPosition().y,_worldSceneObjects[i]->GetPosition().z };
		FMOD_VECTOR s = { _worldSceneObjects[i]->GetScale().x,_worldSceneObjects[i]->GetScale().y ,_worldSceneObjects[i]->GetScale().z };
		FMOD_VECTOR r = { _worldSceneObjects[i]->GetRotation().x,_worldSceneObjects[i]->GetRotation().y,_worldSceneObjects[i]->GetRotation().z };

		g->setPosition(&v);
		g->setScale(&s);
		g->setRotation(&r, new FMOD_VECTOR{ 0.0f,0.0f,0.0f });
		g->setActive(true);
	}

	//Set rotation values

	appGFX->SetCamera(_camera1);
	return S_OK;
}




void Application::Update()
{
	appGFX->UpdateLighting();

	// Update our time
	_timer->Tick();
	float deltaTime = _timer->DeltaTime();

	audioEngine->SetEarPos(_camera1->GetCameraVectorPos(), false, _camera1->GetCameraForwardVec(), _camera1->GetCameraUpVector());

	audioEngine->Update(deltaTime);
	FMOD_VECTOR listenerPos = { _camera1->GetCameraVectorPos().x,_camera1->GetCameraVectorPos().y, _camera1->GetCameraVectorPos().z };
	FMOD_VECTOR brasilVector = { brasilPos.x, brasilPos.y, brasilPos.z };
	audioEngine->GetOcclusion(&listenerPos, &brasilVector, 1.0f, 1.0f);
	
	//Sets the EyePosw for rendering to that of the active camera
	appGFX->SetEyePosW(appGFX->GetCurrentCamera()->GetCameraPosition());

	//Update Scene Objects
	for each (SceneObject* object in _worldSceneObjects)
	{
		object->Update();
	}

	//Constantly sets the skymaps position reletive to the active camera to give the illusion of it never moving
	_skyMap->SetPosition(appGFX->GetCurrentCamera()->GetCameraPosition());
	_skyMap->Update();



	_isWireFrame = appGFX->UpdateWireFrame();

	//Updates all camera control inputs
	UpdateCameraControlls(deltaTime);
	UpdateSoundtackPosition();
}

void Application::UpdateSoundtackPosition() {
	audioEngine->SetBreaksChannelPosition(_jukeboxChannelID, _camera1->GetCameraVectorPos(), false);
}


void Application::UpdateCameraControlls(float deltaTime)
{
	//Camera controlls for W, A, S and D
	if (GetAsyncKeyState('C')) _camMoveSpeed = 10.0f;
	//W - S
	if (GetAsyncKeyState('W')) _camera1->MoveFowardBack(_camMoveSpeed * deltaTime);
	else if (GetAsyncKeyState('S')) _camera1->MoveFowardBack(-_camMoveSpeed * deltaTime);
	
	//A - D
	if (GetAsyncKeyState('A')) _camera1->Strafe(-_camMoveSpeed * deltaTime);
	else if (GetAsyncKeyState('D')) _camera1->Strafe(_camMoveSpeed * deltaTime);

	//Q-E
	if (GetAsyncKeyState('Q')) _camera1->RotateY(-_camMoveSpeed * deltaTime);
	else if (GetAsyncKeyState('E')) _camera1->RotateY(_camMoveSpeed * deltaTime);

	//R-F
	if (GetAsyncKeyState('R')) _camera1->Pitch(-_camMoveSpeed * deltaTime);
	else if (GetAsyncKeyState('F')) _camera1->Pitch(_camMoveSpeed * deltaTime);

	// ================= Camera Selection ================= //
	if (GetAsyncKeyState('1')) appGFX->SetCamera(_camera1);
	appGFX->SetEyePosW(_camera1->GetCameraPosition());
	_camera1->UpdateViewMatrix();
}



void Application::ShowSceneUI()
{
	
	Vector3 soundPos;
	soundPos.x = 0.0f; soundPos.y = 0.0f; soundPos.z = 0.0f;

	ImGui::Begin("Scene Object Control Panel");
	ImGui::Text("Player Soundtrack Controls");
	if (ImGui::Button("Play")) 
	{
		_jukeboxChannelID = audioEngine->PlayAudio(_currentAudioID, soundPos, 0.5);
	}
	if (ImGui::Button("Stop")) {
		audioEngine->StopBreaksChannel(_jukeboxChannelID);
	}

	ImGui::SliderFloat("SoundtrackVolume", &jukeBoxVolume, 0.0f, 1.0f);
	audioEngine->SetBreaksChannelVolume(_jukeboxChannelID, jukeBoxVolume);

	/////////////////////////////////////////////

	ImGui::Text("Rig Speaker Controls - Virtualisation ");
	if (ImGui::Button("Virtualise"))
	{
		 audioEngine->VirtualiseBreaksChannel(0);
	}
	if (ImGui::Button("Devirtualise")) {
		audioEngine->DeVirtualiseBreaksChannel(0);
	}
	ImGui::Text("Change Virtualisation Setting");
	if (ImGui::Button("RESTART")) {
		audioEngine->ChangeVirtualSetting(0, 0);
	}
	if (ImGui::Button("PAUSE")) {
		audioEngine->ChangeVirtualSetting(0, 1);
	}
	if (ImGui::Button("MUTE")) {
		audioEngine->ChangeVirtualSetting(0, 2);
	}
	/*ImGui::SliderFloat("Sound System Volume", &brasilVolume, 0.0f, 20.0f);
	audioEngine->SetBreaksChannelVolume(_brasilChannel, brasilVolume);*/


	ImGui::End();


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


	appGFX->RunLightingControls();

	//Set the rasterizer state to draw the textures on the inside of the model for the skybox

	appGFX->SetSkyboxRasterizerState(true);
	

	appGFX->SetPixelShader(appGFX->GetSkyboxPixelShader());

	_skyMap->Draw();

	//skyMap->Draw();
	_ui.FrameEnd();

	appGFX->Present();

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