#include "Object3d.hlsli"


struct DirectionLight {
	float32_t4 color;     // 
	float32_t3 direction; // 
	float intensity; //輝度
	int32_t isEnable;
};

struct PointLight {
	float32_t4 color; //ライトの色
	float32_t3 position;//ライトの位置
	float intensity; //輝度
	float radius;//ライトの届く距離
	float decay;//減衰
	int32_t isEnable;
};

struct SpotLight {
	float32_t4 color; //ライトの色
	float32_t3 position;//ライトの位置
	float intensity; //輝度
	float32_t3 direction; // 方向
	float distance;//ライトの届く距離
	float decay;//減衰
	float cosAngle;//
	int32_t isEnable;
};

struct WritingStyle {
	DirectionLight directionLight;
	PointLight pointLight;
	SpotLight spotLight;
};



ConstantBuffer<Material>gMaterial : register(b0);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<WritingStyle> gLight: register(b1);
ConstantBuffer<Camera> gCamera : register(b2);


PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);

	
	if (textureColor.a == 0.0) {
		discard;
	}
	
	if (gMaterial.enableLighting != 0) {
		
		//DirectionalLight
		// 拡散反射
		float NdotL = dot(normalize(input.normal), -gLight.directionLight.direction);
		float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
		float32_t3 diffuse = gMaterial.color.rbg * textureColor.rgb * gLight.directionLight.color.rgb * cos * gLight.directionLight.intensity;

		// 鏡面反射
		//float32_t3 reflectLight = reflect(gLight.directionLight.direction, normalize(input.normal));
		float32_t3 halfVector = normalize(-gLight.directionLight.direction + toEye);
		float NdotH = dot(normalize(input.normal), halfVector);
		float specularPow = pow(saturate(NdotH), gMaterial.shininess);
		float32_t3 specular = gLight.directionLight.color.rgb * gLight.directionLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);


		if (gLight.directionLight.isEnable != 0) {
			// 拡散反射＋鏡面反射
			output.color.rgb = diffuse + specular;
		}
		//PointLight
		if (gLight.pointLight.isEnable !=0) {
			float32_t3 pointLightDirection = normalize(input.worldPosition - gLight.pointLight.position);
			float32_t distance = length(gLight.pointLight.position - input.worldPosition);
			float32_t factor = pow(saturate(-distance / gLight.pointLight.radius + 1.0f), gLight.pointLight.decay);
			float32_t3 pointLightColor = gLight.pointLight.color.rgb * gLight.pointLight.intensity * factor;

			// 拡散反射
			float pNdotL = dot(normalize(input.normal), -pointLightDirection);
			float pCos = pow(pNdotL * 0.5f + 0.5f, 2.0f);
			float32_t3 pDiffuse = gMaterial.color.rgb * textureColor.rgb * pointLightColor * pCos;

			// 鏡面反射
			//float32_t3 reflectLight = reflect(pointLightDirection, normalize(input.normal));
			float32_t3 pHalfVector = normalize(-pointLightDirection + toEye);
			float pNdotH = dot(normalize(input.normal), pHalfVector);
			float pSpecularPow = pow(saturate(pNdotH), gMaterial.shininess);
			float32_t3 pSpecular = pointLightColor * pSpecularPow * float32_t3(1.0f, 1.0f, 1.0f);

			// 拡散反射＋鏡面反射
			output.color.rgb = diffuse + specular+pDiffuse + pSpecular;
			
		}

		//SpotLight
		if (gLight.spotLight.isEnable != 0) {
			float32_t3 spotLightDirectionOnSurface = normalize(input.worldPosition - gLight.spotLight.position);
			float32_t3 cosAngle = dot(spotLightDirectionOnSurface, gLight.spotLight.direction);
			float32_t3 falloffFactor = saturate((cosAngle - gLight.spotLight.cosAngle) / (1.0f - gLight.spotLight.cosAngle));
			float32_t distance = length(gLight.spotLight.position - input.worldPosition);
			float32_t factor = pow(saturate(-distance/ gLight.spotLight.distance + 1.0f),gLight.spotLight.decay);
			float32_t3 spotLightColor = gLight.spotLight.color.rgb * gLight.spotLight.intensity*factor*falloffFactor;

			// 拡散反射
			float sNdotL = dot(normalize(input.normal), -gLight.spotLight.direction);
			float sCos = pow(sNdotL * 0.5f + 0.5f, 2.0f);
			float32_t3 sDiffuse = gMaterial.color.rgb * textureColor.rgb * spotLightColor * sCos;

			// 鏡面反射
			//float32_t3 reflectLight = reflect(pointLightDirection, normalize(input.normal));
			float32_t3 sHalfVector = normalize(-gLight.spotLight.direction + toEye);
			float sNdotH = dot(normalize(input.normal), sHalfVector);
			float sSpecularPow = pow(saturate(sNdotH), gMaterial.shininess);
			float32_t3 sSpecular = spotLightColor * sSpecularPow * float32_t3(1.0f, 1.0f, 1.0f);

			// 拡散反射＋鏡面反射
			output.color.rgb = sDiffuse + sSpecular;
		}
		


	} else {
	   output.color.rgb = gMaterial.color.rgb * textureColor.rgb;
	 
	}

	output.color.a = gMaterial.color.a * textureColor.a;

	if (output.color.a == 0.0) {
		discard;
	}

	return output;
}

