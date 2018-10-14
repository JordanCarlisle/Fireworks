struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 colour : COLOUR;
};

float4 main(VS_OUT inVS) : SV_TARGET
{
	return inVS.colour;
}
