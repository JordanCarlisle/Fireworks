cbuffer cbMesh
{
	float4x4 matWorldViewProj;
	float4x4 matWorld;
	float4x4 matModel;
};

struct SKYMAP_VS_OUTPUT    //output structure for skymap vertex shader
{
	float4 Pos : SV_POSITION;
	float3 texCoord : TEXCOORD;
};



SKYMAP_VS_OUTPUT main(float3 inPos : POSITION)
{
	SKYMAP_VS_OUTPUT output = (SKYMAP_VS_OUTPUT)0;

	//Set Pos to xyww instead of xyzw, so that z will always be 1 (furthest from camera)
	output.Pos = mul(float4(inPos, 1.0f), matWorldViewProj).xyww;

	output.texCoord = inPos.xzy;

	return output;
}
