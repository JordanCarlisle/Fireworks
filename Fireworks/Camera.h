#pragma once
#include <DirectXMath.h>
#include "v3.h"
#include "AlignmentClass.h"

using namespace DirectX;

class Camera : public AlignmentClass16
{
	XMMATRIX WVP;
	XMMATRIX World;

	XMMATRIX view;
	XMMATRIX projection;

	XMVECTOR position;
	v3 angle;
	XMVECTOR target;
	XMVECTOR upVector;
	XMVECTOR upWorld;
	XMVECTOR rightVector;

public:
	Camera();
	~Camera() = default;

	void Translate(v3& vec);
	void Rotate(v3& vec);
	void Update();
	XMMATRIX& MatView();
	XMMATRIX& MatProj();
	v3 Position();
};
