#pragma once
#include "SceneObject.h"

class GroundPlane : public SceneObject
{
public:
	GroundPlane(ApplicationGraphics* appGFX) : SceneObject(appGFX) {};
	~GroundPlane();

	void GeneratePlane(float width, float depth, UINT m, UINT n);
	ID3D11Buffer* GetPlaneVb();
	ID3D11Buffer* GetPlaneIb();
	UINT GetPlaneIndexCount();
	
private:
	UINT indexCount;
	ID3D11Buffer* _pVertexBufferGrid;
	ID3D11Buffer* _pIndexBufferGrid;
};
