SamplerState PSamplerState;
Texture2D PTextureY : register(t0);;
Texture2D PTextureP : register(t1);;
Texture2D PTextureB : register(t2);;
Texture2D PTextureR : register(t3);;

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 posWorld : POSITION;
	float2 texCoord : TEXCOORD;
	float4 alpha : INSTANCEALPHA;
	float2 scale : INSTANCESCALE;
};

float4 main(VS_OUT input) : SV_TARGET
{
	float4 diffuse;
	if(input.alpha.r < 1.0) 
		diffuse = PTextureY.Sample(PSamplerState, input.texCoord);
	else if (input.alpha.r < 2.0)
		diffuse = PTextureP.Sample(PSamplerState, input.texCoord);
	else if (input.alpha.r < 3.0)
		diffuse = PTextureB.Sample(PSamplerState, input.texCoord);
	else if (input.alpha.r < 4.0)
		diffuse = PTextureR.Sample(PSamplerState, input.texCoord);

	return float4(diffuse.r, diffuse.g, diffuse.b, diffuse.a * input.alpha.a);
}