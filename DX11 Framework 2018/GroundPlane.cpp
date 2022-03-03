#include "GroundPlane.h"

void GroundPlane::GeneratePlane(float width, float depth, UINT m, UINT n)
{
	UINT vertexCount = m * n;
	UINT faceCount = (m - 1) * (n - 1) * 2;

	//Create Vertices
	float halfWidth = 0.5f * width;
	float halfDepth = 0.5f * depth;

	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);

	std::vector<SimpleVertex> gridVertices;
	gridVertices.resize(vertexCount);

	for (UINT i = 0; i < m; i++)
	{
		float z = halfDepth - i * dz;
		for (UINT j = 0; j < n; j++)
		{
			float x = -halfWidth + j * dx;
			gridVertices[i * n + j].Pos = XMFLOAT3(x, 0.0f, z);

		}
	}
	//Create vertex buffer for grid
	D3D11_BUFFER_DESC bdGridV;
	ZeroMemory(&bdGridV, sizeof(bdGridV));
	bdGridV.Usage = D3D11_USAGE_DEFAULT;
	bdGridV.ByteWidth = sizeof(SimpleVertex) * gridVertices.size();// *(gridVertices.size() * 2);
	bdGridV.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bdGridV.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitDataGridV;
	ZeroMemory(&InitDataGridV, sizeof(InitDataGridV));
	InitDataGridV.pSysMem = &gridVertices[0];

	appGFX->GetDevice()->CreateBuffer(&bdGridV, &InitDataGridV, &_pVertexBufferGrid);

	////////////////////////////////////////////////////////////////////////////////////////////////
	//Generate Indices//
	////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<WORD> gridIndices;
	gridIndices.resize(faceCount * 3);

	//iterate over each quad and compute indices
	UINT k = 0;
	for (UINT i = 0; i < m - 1; i++)
	{
		for (UINT j = 0; j < n - 1; j++)
		{
			gridIndices[k] = i * n + j;
			gridIndices[k + 1] = i * n + j + 1;
			gridIndices[k + 2] = (i + 1) * n + j;

			gridIndices[k + 3] = (i + 1) * n + j;
			gridIndices[k + 4] = i * n + j + 1;
			gridIndices[k + 5] = (i + 1) * n + j + 1;

			k += 6;
		}
	}

	D3D11_BUFFER_DESC bdGridI;
	ZeroMemory(&bdGridI, sizeof(bdGridI));
	indexCount = gridIndices.size();
	bdGridI.Usage = D3D11_USAGE_DEFAULT;
	bdGridI.ByteWidth = sizeof(WORD) * gridIndices.size();
	bdGridI.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bdGridI.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitDataGridI;
	ZeroMemory(&InitDataGridI, sizeof(InitDataGridI));
	InitDataGridI.pSysMem = &gridIndices[0];
	appGFX->GetDevice()->CreateBuffer(&bdGridI, &InitDataGridI, &_pIndexBufferGrid);
}

ID3D11Buffer* GroundPlane::GetPlaneVb()
{
	return _pVertexBufferGrid;
}

ID3D11Buffer* GroundPlane::GetPlaneIb()
{
	return _pIndexBufferGrid;
}

UINT GroundPlane::GetPlaneIndexCount()
{
	return indexCount;
}


