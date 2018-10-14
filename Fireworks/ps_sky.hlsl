SamplerState ObjSamplerState;
TextureCube SkyMap;

cbuffer cbSky
{
	float4 gradient1;
	float4 gradient2;
	float nightAmount;
};

struct SKYMAP_VS_OUTPUT    //output structure for skymap vertex shader
{
	float4 Pos : SV_POSITION;
	float3 texCoord : TEXCOORD;
};

float4 main(SKYMAP_VS_OUTPUT input) : SV_Target
{
//	float4 colour1 = float4(0, 0, 0, 1);
//	float4 colour2 = float4(1, 1, 1, 1);
	float lerpFactor = saturate(input.texCoord.y / 1.6);
	float4 dayColour = lerp(gradient2, gradient1, lerpFactor);
	float4 nightColour = SkyMap.Sample(ObjSamplerState, input.texCoord);
	float4 finalColour =  dayColour * (1.0 - nightAmount) + nightColour * nightAmount;
	return finalColour;
//	return SkyMap.Sample(ObjSamplerState, input.texCoord);
}