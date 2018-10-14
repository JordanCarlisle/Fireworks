#pragma once
#include "DebugLine.h"
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include <unordered_map>
#include <string>
#include <vector>
#include "v2.h"
#include "v3.h"
#include "v4.h"
#include "input_map.h"
#include <functional>
#include "AlignmentClass.h"
#include <cstdlib>
#include <ctime>
#include "FireworkEmitters.h"

#define PI 3.141592f


//used for the vertex shader
struct particle_vertex
{
	v3 pos;
	v2 texCoord;
};

//used for instancing
struct particle_instance_constant_buffer
{
	v3 positions[MAX_PARTICLES];
	float alpha[MAX_PARTICLES];
};

struct particle_constant_buffer
{
	XMMATRIX matWorldViewProj;
	XMMATRIX matWorld;
	XMMATRIX matModel;
};


struct global_light
{
	v4 direction;
	v4 ambient;
	v4 diffuse;
	v4 specular;
};

struct global_constant_buffer
{
	v3 cameraPosition;
	float _pad0;
};

struct sky_constant_buffer
{
	v4 gradient1;
	v4 gradient2;
	float nightAmount;
	float _pad0[3];
};


class Scene : public AlignmentClass16
{
private:


	std::unordered_map<std::string, VertexShader*> vertexShaders;
	std::unordered_map<std::string, GeometryShader*> geometryShaders;
	std::unordered_map<std::string, PixelShader*> pixelShaders;

	
	global_light sun;

	Camera freeCamera;

	std::unordered_map<int, std::function<void()>> fireEvents;

	Mesh::constant_buffer meshConstantBuffer;
	particle_constant_buffer particleConstantBuffer;
	particle_instance_constant_buffer particleInstanceConstantBuffer;
	DebugLine::constant_buffer lineConstantBuffer;
	global_constant_buffer globalConstantBuffer;
	sky_constant_buffer skyConstantBuffer;

	ID3D11Buffer* cbMesh;
	ID3D11Buffer* cbDebugLine;
	ID3D11Buffer* cbGlobal;
	ID3D11Buffer* cbSky;
	ID3D11Buffer* cbInstance;
	ID3D11InputLayout* ilParticleLayout;
	ID3D11InputLayout* ilLineLayout;

	ID3D11Buffer* pVertBuffer;
	ID3D11Buffer* pInstanceBuffer;

	DebugLine debugLine;

	v3 sunAngle;
	bool imgui_sceneEnabled;


	ID3D11Buffer* sphereIndexBuffer;
	ID3D11Buffer* sphereVertBuffer;

	ID3D11ShaderResourceView* smrv;
	ID3D11SamplerState* CubesTexSamplerState;

	ID3D11ShaderResourceView* psmrv_yel;
	ID3D11ShaderResourceView* psmrv_prp;
	ID3D11ShaderResourceView* psmrv_red;
	ID3D11ShaderResourceView* psmrv_blu;

	XMMATRIX sphereWorld;

	int sphereIndices;

	bool tickLock;
	int lastTick;
	bool debugSelected;
	bool debugHotKeyMove;
	bool debugLockX;
	bool debugLockY;
	bool debugLockZ;
	int debugLockVal[3];
	int debugLastPos[3];

	ParticleSystem particleSystem;

public:
	Scene() :
		tickLock(false),
		lastTick(0),
		debugSelected(false),
		debugHotKeyMove(false),
		debugLockVal{ 0,0,0 },
		debugLockX(0),
		debugLockY(0),
		debugLockZ(0)
	{};
	~Scene();

	void Init();
	void Update(float dt, input_map& input);
	void Render();

	void CreateVS(std::string name, std::string filename);
	void CreateGS(std::string name, std::string filename);
	void CreatePS(std::string name, std::string filename);
};