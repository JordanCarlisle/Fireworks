cbuffer cb_line
{
	float4x4 matWorldViewProj;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 colour : COLOUR;
};

VS_OUT main(float4 inPos : POSITION, float4 inColour : COLOUR)
{
	VS_OUT vs_out;
	vs_out.pos = mul(inPos, matWorldViewProj);
	vs_out.colour = inColour;
	return vs_out;
}