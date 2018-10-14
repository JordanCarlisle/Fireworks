#define MAX_PARTICLES 65536

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

VS_OUT main(float4 pos : POSITION, float3 texCoord : TEXCOORD, float3 instancePos : INSTANCEPOS, float4 col : INSTANCEALPHA, float2 scale : INSTANCESCALE, uint instanceID : SV_InstanceID)
{
	VS_OUT vs_out;

	pos = float4(instancePos, 0.0);

	vs_out.pos = mul(pos, matWorldViewProj);
	vs_out.posWorld = mul(pos, matWorld);
	vs_out.texCoord = texCoord;
	vs_out.alpha = col;
	vs_out.scale = scale;

	return vs_out;
}