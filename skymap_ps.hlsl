TextureCube SkyMap;
SamplerState ObjSamplerState;

struct InputType
{
	float4 position : SV_POSITION;
	float3 texCoord : TEXCOORD;
};

float4 main(InputType input) : SV_TARGET
{
	return SkyMap.Sample(ObjSamplerState, input.texCoord);
}