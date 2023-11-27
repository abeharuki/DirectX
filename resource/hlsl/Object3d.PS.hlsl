#include "Object3d.hlsli"


ConstantBuffer<Material>gMaterial : register(b0);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<DirectionalLight> gDirectionalLight: register(b1);


PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	//float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
	
	if (textureColor.a <= 0.5) {
		discard;
	}
	if (textureColor.a == 0.0) {
		discard;
	}
	
	if (gMaterial.enableLighting != 0) {
		float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
		float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
		
		output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb* cos * gDirectionalLight.intensity;
		output.color.a = gMaterial.color.a * textureColor.a;
		
		if (output.color.a == 0.0) {
			discard;
		}
	}
	else {
		output.color = gMaterial.color * textureColor;
		if (output.color.a == 0.0) {
			discard;
		}
	}
	
	return output;

}

