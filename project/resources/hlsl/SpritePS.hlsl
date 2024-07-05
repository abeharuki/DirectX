#include "Sprite.hlsli"

ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	// float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);

	if (textureColor.a <= 0.5) {
		discard;
	}
	if (textureColor.a == 0.0) {
		discard;
	}

	if (gMaterial.enableLighting != 0) {
		float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
		float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);

		output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb *
		                   cos * gDirectionalLight.intensity;
		output.color.a = gMaterial.color.a * textureColor.a;

		if (output.color.a == 0.0) {
			discard;
		}
	} else {
		output.color = gMaterial.color * textureColor;
		if (output.color.a == 0.0) {
			discard;
		}
	}
	/*
    float32_t mask = gMaskTexture.Sample(gSampler, input.texcoord);
        //maskの値が0.5（閾値）以下の場合はdiscardして抜く
    if (mask < gLight.dissolve.threshold)
    {
        discard;
    }
        //Edgeっぽさほ算出
    float32_t edge = 1.0f - smoothstep(gLight.dissolve.threshold, gLight.dissolve.threshold + 0.03f, mask);
  
	 //Edgeっぽいほど指定した色を加算
    output.color.rgb += edge * gLight.dissolve.edgeColor;
	*/

	return output;
}
