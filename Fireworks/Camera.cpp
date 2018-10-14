#include "Camera.h"

Camera::Camera()
{
	position = XMVectorSet(-300.0f, 0.0f, 0.0f, 0.0f);
	target = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	upWorld = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	angle.set(0.0f, 0.5f, 0.0f);
}

void Camera::Translate(v3& vec)
{
	position += vec.x*target;
	position += vec.z*rightVector;
}

void Camera::Rotate(v3& vec)
{
	angle.x += vec.x;
	angle.y += vec.y;
}

const float PI = 3.14159265359F;
void Camera::Update()
{
	angle.y = (angle.y > 89.0f) ? 89.0f : angle.y;
	angle.y = (angle.y < -89.0f) ? -89.0f : angle.y;

	target = XMVectorSet(cosf(angle.x) * cosf(angle.y), sinf(angle.x) * cosf(angle.y), sinf(angle.y), 0.0f);
	target = XMVector3Normalize(target);
	
	rightVector = XMVector3Cross(target, upWorld);
	rightVector = XMVector3Normalize(rightVector);

	upVector = XMVector3Cross(rightVector, target);
	upVector = XMVector3Normalize(upVector);

	XMVECTOR vt = position + target;
	
	view = XMMatrixLookAtRH(position, vt, upVector);

	projection = XMMatrixPerspectiveFovRH(0.4f*PI, 1.33, 1.0f, 1000.0f);
}

XMMATRIX& Camera::MatView()
{
	return view;
}


XMMATRIX& Camera::MatProj()
{
	return projection;
}

v3 Camera::Position()
{
	return v3(
		XMVectorGetX(position),
		XMVectorGetY(position),
		XMVectorGetZ(position)
	);
}
