#include "Scene.h"
#include <vector>
#include <algorithm>
#include <fstream>

#include <DirectXMath.h>
#include "imgui/imgui.h"
#include "PrimitiveFactory.h"

#include "App.h"

using namespace DirectX;

extern ID3D11Device* d3d11Device;
extern ID3D11DeviceContext* d3d11DevCon;


D3D11_INPUT_ELEMENT_DESC ParticleLayout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	{ "INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT,    1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "INSTANCEALPHA", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,    1, 12, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "INSTANCESCALE", 0, DXGI_FORMAT_R32G32_FLOAT,    1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
};

Scene::~Scene()
{
	for (auto& vs : vertexShaders)
	{

		delete vs.second;
	}

	for (auto& ps : pixelShaders)
	{
		delete ps.second;
	}

	psmrv_yel->Release();
	psmrv_prp->Release();
	psmrv_blu->Release();
	psmrv_red->Release();

	ilParticleLayout->Release();
	ilLineLayout->Release();
	cbMesh->Release();
	cbGlobal->Release();
	cbDebugLine->Release();

	pInstanceBuffer->Release();
	pVertBuffer->Release();

	sphereIndexBuffer->Release();
	sphereVertBuffer->Release();

	smrv->Release();

	CubesTexSamplerState->Release();

}




void Scene::Init()
{
	//mesh constant buffer
	D3D11_BUFFER_DESC constantBufferDesc;
	ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBufferDesc.ByteWidth = sizeof(Mesh::constant_buffer);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.MiscFlags = 0;

	d3d11Device->CreateBuffer(&constantBufferDesc, NULL, &cbMesh);

	//line constant buffer
	ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBufferDesc.ByteWidth = sizeof(DebugLine::constant_buffer);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.MiscFlags = 0;

	d3d11Device->CreateBuffer(&constantBufferDesc, NULL, &cbDebugLine);

	//global constant buffer
	ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBufferDesc.ByteWidth = sizeof(global_constant_buffer);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.MiscFlags = 0;

	d3d11Device->CreateBuffer(&constantBufferDesc, NULL, &cbGlobal);

	//instance constant buffer
	ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBufferDesc.ByteWidth = sizeof(particle_instance_constant_buffer);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.MiscFlags = 0;

	d3d11Device->CreateBuffer(&constantBufferDesc, NULL, &cbInstance);

	//sky constant buffer
	ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBufferDesc.ByteWidth = sizeof(sky_constant_buffer);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.MiscFlags = 0;

	d3d11Device->CreateBuffer(&constantBufferDesc, NULL, &cbSky);

	//sampler state for sky cubemeap
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	auto hr = d3d11Device->CreateSamplerState(&sampDesc, &CubesTexSamplerState);

	CreateVS("vs_particle", "vs_particle.hlsl");
	CreateGS("gs_particle", "gs_particle.hlsl");
	CreatePS("ps_particle", "ps_particle.hlsl");

	CreateVS("vs_line", "vs_line.hlsl");
	CreatePS("ps_line", "ps_line.hlsl");

	CreateVS("vs_sky", "vs_sky.hlsl");
	CreatePS("ps_sky", "ps_sky.hlsl");


	// Create the Input Layout
	auto vsBuffer = vertexShaders["vs_particle"]->GetByteCodeBuffer();
	auto r = d3d11Device->CreateInputLayout(ParticleLayout, ARRAYSIZE(ParticleLayout), vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &ilParticleLayout);

	auto vsBufferLine = vertexShaders["vs_line"]->GetByteCodeBuffer();
	r = d3d11Device->CreateInputLayout(LineInputLayout, ARRAYSIZE(LineInputLayout), vsBufferLine->GetBufferPointer(), vsBufferLine->GetBufferSize(), &ilLineLayout);

	// Set the Input Layout
	d3d11DevCon->IASetInputLayout(ilParticleLayout);
	d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	//create sky
	D3DX11_IMAGE_LOAD_INFO loadSMInfo;
	loadSMInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	ID3D11Texture2D* SMTexture = 0;
	auto tres = D3DX11CreateTextureFromFile(d3d11Device, L"resources/night2048.dds",
		&loadSMInfo, 0, (ID3D11Resource**)&SMTexture, 0);

	D3D11_TEXTURE2D_DESC SMTextureDesc;
	SMTexture->GetDesc(&SMTextureDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
	SMViewDesc.Format = SMTextureDesc.Format;
	SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	SMViewDesc.TextureCube.MipLevels = SMTextureDesc.MipLevels;
	SMViewDesc.TextureCube.MostDetailedMip = 0;

	tres = d3d11Device->CreateShaderResourceView(SMTexture, &SMViewDesc, &smrv);


	std::vector<Mesh::vertex> sphereVbo;
	std::vector<DWORD> sphereIbo;
	prim_sphere(sphereIbo, sphereVbo, 12, 2.0f);
	sphereIndices = sphereIbo.size();

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Mesh::vertex) * sphereVbo.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = sphereVbo.data();
	hr = d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &sphereVertBuffer);

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * sphereIndices;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA sphInit;

	sphInit.pSysMem = sphereIbo.data();
	d3d11Device->CreateBuffer(&indexBufferDesc, &sphInit, &sphereIndexBuffer);


	//sky settings
	skyConstantBuffer.gradient1 = v4(102.0f / 256.0f, 141.0f / 256.0f, 190.0f / 256.0f, 1.0f);
	skyConstantBuffer.gradient2 = v4(202.0f / 256.0f, 213.0f / 256.0f, 222.0f / 256.0f, 1.0f);
	skyConstantBuffer.nightAmount = 1.0f;

	sun.direction = v4(0.0f, 0.0f, 1.0f, 0.0f);
	sun.ambient = v4(0.35f, 0.35f, 0.35f, 1.0f);
	sun.diffuse = v4(0.6f, 0.6f, 0.6f, 1.0f);
	sun.specular = v4(0.1f, 0.1f, 0.1f, 1.0f);

		//particles
	D3D11_BUFFER_DESC instBuffDesc;
	ZeroMemory(&instBuffDesc, sizeof(instBuffDesc));

	instBuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	instBuffDesc.ByteWidth = sizeof(particle_instance) * MAX_PARTICLES;
	instBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instBuffDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA instData;
	ZeroMemory(&instData, sizeof(instData));

	instData.pSysMem = particleSystem.GetInstanceBuffer();

	hr = d3d11Device->CreateBuffer(&instBuffDesc, &instData, &pInstanceBuffer);

	D3D11_BUFFER_DESC billboardBufferDesc;
	ZeroMemory(&billboardBufferDesc, sizeof(billboardBufferDesc));

	billboardBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	billboardBufferDesc.ByteWidth = sizeof(particle_vertex);
	billboardBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	billboardBufferDesc.CPUAccessFlags = 0;
	billboardBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA ginitData;

	particle_vertex gv{ v3(0.0f, 0.0f, 0.0f), v2() };

	ginitData.pSysMem = &gv;
	d3d11Device->CreateBuffer(&billboardBufferDesc, &ginitData, &pVertBuffer);

	//Create sprite textures
	hr = D3DX11CreateShaderResourceViewFromFile(d3d11Device, L"resources/Yki_trail.dds", NULL, NULL, &psmrv_yel, NULL);
	hr = D3DX11CreateShaderResourceViewFromFile(d3d11Device, L"resources/Rki_trail.dds", NULL, NULL, &psmrv_red, NULL);
	hr = D3DX11CreateShaderResourceViewFromFile(d3d11Device, L"resources/Bki_trail.dds", NULL, NULL, &psmrv_blu, NULL);
	hr = D3DX11CreateShaderResourceViewFromFile(d3d11Device, L"resources/Pki_trail.dds", NULL, NULL, &psmrv_prp, NULL);

	debugLine.SetupBuffers();

	


	//define firing events
	std::srand(time(0));


	fireEvents[2] = [&]() {
		particleSystem.CreateEmitter<RocketEmitter>(v3(0, -100, 0), std::rand() % 4);
	};

	fireEvents[12] = [&]() {
		particleSystem.CreateEmitter<RocketEmitter>(v3(0, 100, 0), std::rand() % 4);
	};

	fireEvents[24] = [&]() {
		particleSystem.CreateEmitter<RocketEmitter>(v3(0, -50, 0), std::rand() % 4);
	};

	fireEvents[30] = [&]() {
		particleSystem.CreateEmitter<RocketEmitter>(v3(0, -120, 0), std::rand() % 4);
	};

	
	fireEvents[34] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, -120, 0), std::rand() % 4); };
	fireEvents[38] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, -100, 0), std::rand() % 4); };
	fireEvents[42] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, -80, 0), std::rand() % 4); };
	fireEvents[46] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, -60, 0), std::rand() % 4); };
	fireEvents[50] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, -40, 0), std::rand() % 4); };
	fireEvents[54] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, -20, 0), std::rand() % 4); };
	fireEvents[58] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, 0, 0), std::rand() % 4); };
	fireEvents[60] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, 20, 0), std::rand() % 4); };
	fireEvents[62] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, 40, 0), std::rand() % 4); };
	fireEvents[66] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, 60, 0), std::rand() % 4); };
	fireEvents[68] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, 80, 0), std::rand() % 4); };
	fireEvents[72] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, 100, 0), std::rand() % 4); };
	fireEvents[76] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, 120, 0), std::rand() % 4); };
	fireEvents[80] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, 140, 0), std::rand() % 4); };

	fireEvents[81] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, 120, 0), std::rand() % 4); };
	fireEvents[82] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, 100, 0), std::rand() % 4); };
	fireEvents[83] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, 80, 0), std::rand() % 4); };
	fireEvents[84] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, 60, 0), std::rand() % 4); };
	fireEvents[85] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, 40, 0), std::rand() % 4); };
	fireEvents[86] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, 20, 0), std::rand() % 4); };
	fireEvents[87] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, 0, 0), std::rand() % 4); };
	fireEvents[88] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, -20, 0), std::rand() % 4); };
	fireEvents[89] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, -40, 0), std::rand() % 4); };
	fireEvents[90] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, -60, 0), std::rand() % 4); };
	fireEvents[91] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, -80, 0), std::rand() % 4); };
	fireEvents[92] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, -100, 0), std::rand() % 4); };
	fireEvents[93] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, -120, 0), std::rand() % 4); };
	fireEvents[94] = [&]() { particleSystem.CreateEmitter<RocketEmitter>(v3(0, -140, 0), std::rand() % 4); };

	for (int i = 0; i < 30; i++)
	{
		fireEvents[95 + i * 2] = [&]() { 
			particleSystem.CreateEmitter<RocketFallEmitter>(v3(0, 20, 0), std::rand() % 4);
			particleSystem.CreateEmitter<RocketEmitter>(v3(0, -120 + std::rand() % 240, 0), std::rand() % 4);
		};
	}

	for (int i = 0; i < 80; i++)
	{
		fireEvents[150 + i * 2] = [&]() {
			particleSystem.CreateEmitter<RocketFallEmitter>(v3(0, -120 + std::rand() % 240, -10 + std::rand() % 20), std::rand() % 4);
			particleSystem.CreateEmitter<RocketEmitter>(v3(0, -120 + std::rand() % 240, -10 + std::rand() % 20), std::rand() % 4);
		};
	}

	for (int i = 0; i < 100; i++)
	{
		fireEvents[320 + i * 2] = [&]() {
			particleSystem.CreateEmitter<RocketFallEmitter>(v3(0, 0, 0), std::rand() % 4);
			particleSystem.CreateEmitter<RocketEmitter>(v3(0, -60 + std::rand() % 80, 0), std::rand() % 4);
			particleSystem.CreateEmitter<RocketEmitter>(v3(0, -120 + std::rand() % 240, -20 + std::rand() % 20), std::rand() % 4);
			particleSystem.CreateEmitter<RocketEmitter>(v3(0, -160 + std::rand() % 320, 0), std::rand() % 4);
		};
	}

	

	imgui_sceneEnabled = true;
	
}

void Scene::Update(float dt, input_map& input)
{
	//input
	float mul = float(input.shift + 1) * 50.0f;
	freeCamera.Translate(v3(
		(input.w - input.s) * mul * dt,
		0.0f,
		(input.d - input.a) * mul * dt
	));

	if (input.rmb)
	{
		freeCamera.Rotate(v3(
			-input.axis[0] * 5.0f * dt,
			-input.axis[1] * 5.0f * dt,
			0.0f
		));
	}

	freeCamera.Update();

	//update sky
	auto camPos = freeCamera.Position();
	sphereWorld = XMMatrixIdentity();
	auto sphereTrans = XMMatrixTranslation(camPos.x, camPos.y, camPos.z);
	sphereWorld = sphereTrans;

	//update global lighting
	sun.direction = v4(
		cosf(sunAngle.x) * cosf(sunAngle.y),
		sinf(sunAngle.x) * cosf(sunAngle.y),
		sinf(sunAngle.y),
		1.0f
	);

	if (input.lmb && !debugSelected)
	{
		debugSelected = true;
		particleSystem.CreateEmitter<RocketFallEmitter>(v3(-50.0f, -100, 0), std::rand() % 4);
	}
	if (!input.lmb && debugSelected)
		debugSelected = false;

	if (fireEvents.find(input.sCycle) != fireEvents.end() && lastTick == input.sCycle && !tickLock)
	{
		tickLock = true;
		fireEvents[input.sCycle]();
	}

	if (lastTick != input.sCycle && tickLock)
	{
		tickLock = false;
	}
	lastTick = input.sCycle;

	particleSystem.Update(dt, camPos);
}

void Scene::Render()
{
	XMMATRIX& camView = freeCamera.MatView();
	XMMATRIX& camProjection = freeCamera.MatProj();
	UINT stride = sizeof(Mesh::vertex);
	UINT offset = 0;

	globalConstantBuffer.cameraPosition = freeCamera.Position();
	d3d11DevCon->UpdateSubresource(cbGlobal, 0, NULL, &globalConstantBuffer, 0, 0);

	//begin imgui panels
	ImGui::Begin("Scene", &imgui_sceneEnabled);
	ImGui::Text("Camera: %.3f, %.3f, %.3f", globalConstantBuffer.cameraPosition.x, globalConstantBuffer.cameraPosition.y, globalConstantBuffer.cameraPosition.z);
	ImGui::Text("Sun");
	ImGui::SliderFloat("Pan", (float*)&sunAngle.x, -3.141f, 3.141f, "%.3f", 1.0f);
	ImGui::SliderFloat("Tilt", (float*)&sunAngle.y, -3.141f, 3.141f, "%.3f", 1.0f);
	ImGui::ColorEdit3("Ambient", (float*)&sun.ambient);
	ImGui::ColorEdit3("Diffuse", (float*)&sun.diffuse);
	ImGui::ColorEdit3("Specular", (float*)&sun.specular);

	ImGui::Text("Sky");
	ImGui::ColorEdit3("Colour1", (float*)&skyConstantBuffer.gradient1);
	ImGui::ColorEdit3("Colour2", (float*)&skyConstantBuffer.gradient2);
	ImGui::SliderFloat("Night", (float*)&skyConstantBuffer.nightAmount, 0.0f, 1.0f);
	ImGui::End();
	//end imgui panels

	
	d3d11DevCon->UpdateSubresource(cbGlobal, 0, NULL, &globalConstantBuffer, 0, 0);

	//draw sky
	d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	d3d11DevCon->IASetIndexBuffer(sphereIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	d3d11DevCon->IASetVertexBuffers(0, 1, &sphereVertBuffer, &stride, &offset);

	XMMATRIX sWvp = sphereWorld * camView * camProjection;
	meshConstantBuffer.matWorldViewProj = XMMatrixTranspose(sWvp);
	meshConstantBuffer.matWorld = XMMatrixTranspose(sphereWorld);
	d3d11DevCon->UpdateSubresource(cbMesh, 0, NULL, &meshConstantBuffer, 0, 0);
	d3d11DevCon->VSSetConstantBuffers(0, 1, &cbMesh);
	d3d11DevCon->UpdateSubresource(cbSky, 0, NULL, &skyConstantBuffer, 0, 0);
	d3d11DevCon->PSSetConstantBuffers(0, 1, &cbSky);
	d3d11DevCon->PSSetShaderResources(0, 1, &smrv);
	d3d11DevCon->PSSetSamplers(0, 1, &CubesTexSamplerState);

	d3d11DevCon->VSSetShader(vertexShaders["vs_sky"]->GetProgram(), 0, 0);
	d3d11DevCon->PSSetShader(pixelShaders["ps_sky"]->GetProgram(), 0, 0);
	d3d11DevCon->OMSetDepthStencilState(App::instance().GetLessEqualState(), 0);
	d3d11DevCon->RSSetState(App::instance().GetCullNone());
	d3d11DevCon->DrawIndexed(sphereIndices, 0, 0);

	d3d11DevCon->OMSetDepthStencilState(NULL, 0);
	d3d11DevCon->RSSetState(App::instance().GetCull());

	v3 bbLineColour(1.0f, 1.0f, 1.0f);

	d3d11DevCon->PSSetConstantBuffers(0, 1, &cbGlobal);


	D3D11_MAPPED_SUBRESOURCE resource;
	d3d11DevCon->Map(pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, particleSystem.GetInstanceBuffer(), sizeof(particle_instance) * MAX_PARTICLES);
	d3d11DevCon->Unmap(pInstanceBuffer, 0);


	// Set topology to points
	d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);


	d3d11DevCon->VSSetShader(vertexShaders["vs_particle"]->GetProgram(), 0, 0);
	d3d11DevCon->GSSetShader(geometryShaders["gs_particle"]->GetProgram(), 0, 0);
	d3d11DevCon->PSSetShader(pixelShaders["ps_particle"]->GetProgram(), 0, 0);


	UINT strides[2] = { sizeof(particle_vertex), sizeof(particle_instance) };
	UINT offsets[2] = { 0, 0 };

	ID3D11Buffer* vertBillInstBuffers[2] = { pVertBuffer, pInstanceBuffer };

	//Set the models vertex and isntance buffer using the arrays created above
	d3d11DevCon->IASetVertexBuffers(0, 2, vertBillInstBuffers, strides, offsets);

	//Set the WVP matrix and send it to the constant buffer in effect file
	XMMATRIX worldMat = XMMatrixIdentity();
	XMMATRIX wvp = camView * camProjection;

	particleConstantBuffer.matWorldViewProj = XMMatrixTranspose(wvp);
	particleConstantBuffer.matWorld = XMMatrixTranspose(worldMat);


	d3d11DevCon->UpdateSubresource(cbMesh, 0, NULL, &particleConstantBuffer, 0, 0);


	ID3D11Buffer* vsBillConstBuffers[2] = { cbMesh, cbInstance };
	d3d11DevCon->VSSetConstantBuffers(0, 2, &cbMesh);

	ID3D11Buffer* gsBillConstBuffers[2] = { cbGlobal, cbMesh };
	d3d11DevCon->GSSetConstantBuffers(0, 2, gsBillConstBuffers);
	d3d11DevCon->PSSetShaderResources(0, 1, &psmrv_yel);
	d3d11DevCon->PSSetShaderResources(1, 1, &psmrv_blu);
	d3d11DevCon->PSSetShaderResources(2, 1, &psmrv_blu);
	d3d11DevCon->PSSetShaderResources(3, 1, &psmrv_red);

	d3d11DevCon->RSSetState(App::instance().GetCullNone());
	d3d11DevCon->DrawInstanced(1, particleSystem.GetNumParticles(), 0, 0);

	// set gs and ps back to defaults
	d3d11DevCon->GSSetShader(NULL, 0, 0);

	d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void Scene::CreateVS(std::string name, std::string filename)
{
	if (vertexShaders.find(name) == vertexShaders.end())
	{
		VertexShader* vs = new VertexShader();
		vs->Compile(filename.c_str(), "vs_4_0", "main");
		vertexShaders[name] = vs;
	}
}

void Scene::CreateGS(std::string name, std::string filename)
{
	if (geometryShaders.find(name) == geometryShaders.end())
	{
		GeometryShader* gs = new GeometryShader();
		gs->Compile(filename.c_str(), "gs_4_0", "main");
		geometryShaders[name] = gs;
	}
}

void Scene::CreatePS(std::string name, std::string filename)
{
	if (pixelShaders.find(name) == pixelShaders.end())
	{
		PixelShader* ps = new PixelShader();
		ps->Compile(filename.c_str(), "ps_4_0", "main");
		pixelShaders[name] = ps;
	}
}
