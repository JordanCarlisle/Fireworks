#define MAX_PARTICLES 100

cbuffer cbGlobal
{
	float3 cameraPosition;
	float _pad0;
};

cbuffer cbMesh
{
	float4x4 matWorldViewProj;
	float4x4 matWorld;
	float4x4 matModel;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 posWorld : POSITION;
	float2 texCoord : TEXCOORD;
	float4 alpha : INSTANCEALPHA;
	float2 scale : INSTANCESCALE;
};

[maxvertexcount(4)]
void main(point VS_OUT input[1], inout TriangleStream<VS_OUT> OutputStream)
{
	if (input[0].scale.x < 0.0)
		input[0].scale.x = 0.0;
	if (input[0].scale.y < 0.0)
		input[0].scale.y = 0.0;

	float3 planeNormal = input[0].posWorld - cameraPosition;

	planeNormal = normalize(planeNormal);

	float3 upVector = float3(0.0f, 0.0f, 1.0f);

	float3 rightVector = normalize(cross(planeNormal, upVector)); 
	rightVector *= input[0].scale.x;

	upVector = normalize(cross(planeNormal, rightVector));
	upVector *= input[0].scale.y;

	// Create the billboards quad
	float3 vert[4];

	vert[0] = input[0].posWorld - rightVector - upVector;
	vert[1] = input[0].posWorld + rightVector - upVector;
	vert[2] = input[0].posWorld - rightVector + upVector; 
	vert[3] = input[0].posWorld + rightVector + upVector;

	float2 texCoord[4];
	texCoord[0] = float2(0, 1);
	texCoord[1] = float2(1, 1);
	texCoord[2] = float2(0, 0);
	texCoord[3] = float2(1, 0);

	//append verts to list
	VS_OUT outputVert;
	for (int i = 0; i < 4; i++)
	{
		outputVert.pos = mul(float4(vert[i], 1.0f), matWorldViewProj);
		outputVert.posWorld = float4(vert[i], 0.0f);
		outputVert.texCoord = texCoord[i];
		outputVert.alpha = input[0].alpha;
		outputVert.scale = input[0].scale;

		OutputStream.Append(outputVert);
	}
}