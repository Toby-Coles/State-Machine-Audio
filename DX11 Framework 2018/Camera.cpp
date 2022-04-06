#include "Camera.h"
Camera::Camera() {
	momentum = 0.0f;
	InitializeCamera();
}

Camera::~Camera()
{
}



void Camera::SetLens(float fovY, float aspect, float zNear, float zFar) {
	//Cache properties
	_FovY = fovY;
	_Aspect = aspect;

	_NearZ = zNear;
	_FarZ = zFar;

	_NearWindowHeight = 2.0f * _NearZ * tanf(0.5f * _FovY);
	_FarWindowHeight = 2.0f * _FarZ * tanf(0.5f * _FovY);

	XMMATRIX proj = XMMatrixPerspectiveFovLH(_FovY, _Aspect, _NearZ, _FarZ);
	XMStoreFloat4x4(&_projection, proj);


}


void Camera::UpdateViewMatrix()
{
	XMVECTOR R = XMLoadFloat3(&_right);
	XMVECTOR U = XMLoadFloat3(&_up);
	XMVECTOR L = XMLoadFloat3(&_look);
	XMVECTOR P = XMLoadFloat3(&_position);
	
	// Make look vector unit length.
	L = XMVector3Normalize(L);

	// Compute a new corrected "up" vector and normalize it.
	U = XMVector3Normalize(XMVector3Cross(L, R));

	// Compute a new corrected "right" vector. U and L are


	R = XMVector3Cross(U, L);

	// Fill in the view matrix entries.
	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	XMStoreFloat3(&_right, R);
	XMStoreFloat3(&_up, U);
	XMStoreFloat3(&_look, L);

	_view(0, 0) = _right.x;
	_view(1, 0) = _right.y;
	_view(2, 0) = _right.z;
	_view(3, 0) = x;
	_view(0, 1) = _up.x;
	_view(1, 1) = _up.y;
	_view(2, 1) = _up.z;
	_view(3, 1) = y;
	_view(0, 2) = _look.x;
	_view(1, 2) = _look.y;
	_view(2, 2) = _look.z;
	_view(3, 2) = z;
	_view(0, 3) = 0.0f;
	_view(1, 3) = 0.0f;
	_view(2, 3) = 0.0f;
	_view(3, 3) = 1.0f;
}

float Camera::GetFovX()const
{
	float halfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f * atan(halfWidth / _NearZ);
}
float Camera::GetNearWindowWidth()const
{
	return _Aspect * _NearWindowHeight;
}
float Camera::GetNearWindowHeight()const
{
	return _NearWindowHeight;
}
float Camera::GetFarWindowWidth()const
{
	return _Aspect * _FarWindowHeight;
}
float Camera::GetFarWindowHeight()const
{
	return _FarWindowHeight;
}


void Camera::SetCameraPosition(XMFLOAT3 position)
{
	_position = position;

}
void Camera::SetPosition(float x, float y, float z) {
	_position.x = x;
	_position.y = y;
	_position.z = z;
}


void Camera::SetCameraLookAtPoint(XMFLOAT3 lookAt)
{
	_look = lookAt;
}

void Camera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up) {
	XMVECTOR p = XMLoadFloat3(&pos);
	XMVECTOR t = XMLoadFloat3(&target);
	XMVECTOR u = XMLoadFloat3(&up);

	LookAt(p, t, u);
}
void Camera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp) {
	XMVECTOR l = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR r = XMVector3Normalize(XMVector3Cross(worldUp, l));
	XMVECTOR u = XMVector3Cross(l, r);

	XMStoreFloat3(&_position, pos);
	XMStoreFloat3(&_look, l);
	XMStoreFloat3(&_right, r);
	XMStoreFloat3(&_up, u);
}


void Camera::SetCameraUp(XMFLOAT3 up)
{
	_up = up;
}

XMFLOAT3 Camera::GetCameraPosition()
{
	return _position;
}
XMVECTOR Camera::GetCameraVecPosition() {
	XMFLOAT3 position = XMFLOAT3(_position.x, _position.y, _position.z);
	return XMLoadFloat3(&position);
}

Vector3 Camera::GetCameraVectorPos()
{
	Vector3 position;
	position.x = _position.x;
	position.y = _position.y;
	position.z = _position.z;

	return position;
}


XMFLOAT3 Camera::GetCameraLookAtPoint()
{
	return _look;
}

XMVECTOR Camera::GetLookAtVec()
{
	return XMLoadFloat3(&_look);
}

Vector3 Camera::GetCameraForwardVec() {
	Vector3 forward;
	forward.x = _look.x;
	forward.y = _look.y;
	forward.z = _look.z;

	return forward;

}

XMFLOAT3 Camera::GetCameraUp()
{
	return _up;
}

Vector3 Camera::GetCameraUpVector() {
	Vector3 up;
	up.x = _up.x;
	up.y = _up.y;
	up.z = _up.z;

	return up;
}

void Camera::MoveFowardBack(float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&_look);
	XMVECTOR p = XMLoadFloat3(&_position);

	XMStoreFloat3(&_position, XMVectorMultiplyAdd(s, l, p));
}

void Camera::Strafe(float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&_right);
	XMVECTOR p = XMLoadFloat3(&_position);
	XMStoreFloat3(&_position, XMVectorMultiplyAdd(s, r, p));

}
void Camera::Pitch(float angle)
{
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&_right), angle);
	XMStoreFloat3(&_up, XMVector3TransformNormal(XMLoadFloat3(&_up), R));
	XMStoreFloat3(&_look, XMVector3TransformNormal(XMLoadFloat3(&_look), R));

}

void Camera::RotateY(float angle)
{
	// Rotate the basis vectors about the world y-axis.
	XMMATRIX R = XMMatrixRotationY(angle);
	XMStoreFloat3(&_right, XMVector3TransformNormal(XMLoadFloat3(&_right), R));
	XMStoreFloat3(&_up, XMVector3TransformNormal(XMLoadFloat3(&_up), R));
	XMStoreFloat3(&_look, XMVector3TransformNormal(XMLoadFloat3(&_look), R));

}


XMFLOAT4X4 Camera::GetViewMatrix()
{
	return _view;
}

XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return _projection;
}



void Camera::InitializeCamera()
{
	_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	_up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	_look = XMFLOAT3(0.0f, 0.0f, 1.0f);
}