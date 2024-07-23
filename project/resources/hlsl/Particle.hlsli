
struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t2 texcoord : TEXCOORD0;
	float32_t3 normal : NORMAL0;
	
};

static const uint32_t kMaxParticles = 1024;

struct Particle
{
    float32_t3 translate;
    float32_t3 scale;
    float32_t lifeTime;
    float32_t3 velocity;
    float32_t currentTime;
    float32_t4 color;
};

struct ParticleForGPU {
	float32_t4x4 WVP;
	float32_t4x4 World;
	float32_t4 color;
};

struct PerFrame
{
    float32_t time;
    float32_t deltaTime;
};

struct ViewProjectionMatrix {
	float32_t4x4 view;
	float32_t4x4 projection;
	float32_t3 camera;
};

struct PreView{
    float32_t4x4 view;
    float32_t4x4 projection;
    float32_t4x4 billboardMatrix;
};



struct VertexShaderOutput {
	float32_t4 position : SV_POSITION;
	float32_t2 texcoord : TEXCOORD0;
	float32_t3 normal : NORMAL0;
	float32_t4 color : COLOR0;
};

struct Color {
	float32_t3 rgb;
	float a;
};

struct Material {
	float32_t4 color;
	int32_t enableLighting;
	float32_t4x4 uvTransform;
};

struct DirectionalLight {
	float32_t4 color;     // 
	float32_t3 direction; // 
	float intensity;      // 
	int32_t isEnable;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};