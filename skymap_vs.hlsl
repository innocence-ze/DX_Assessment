cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct InputType
{
	float4 position : POSITION; 
	float2 inTexCoord : TEXCOORD;
	float3 normal : NORMAL;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float3 texCoord : TEXCOORD;
};

OutputType main(InputType input)
{
	OutputType output = (OutputType)0;

	input.position = input.position / input.position.w;

	//Set Pos to xyww instead of xyzw, so that z will always be 1 (furthest from camera)
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.texCoord = input.position.xyz;
	return output;
}