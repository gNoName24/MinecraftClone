#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;
in vec4 FragPosLightSpace;

uniform vec3 globalLightDir;
uniform vec3 globalLightColor;
uniform vec3 ambientLightColor;
uniform vec3 viewPos;
uniform sampler2D shadowMap;

struct PointLight {
    vec4 position;
    vec3 color;
};

#define MAX_POINT_LIGHTS 4
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int numPointLights;

float ShadowCalculation(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if(projCoords.z > 1.0)
        return 0.0;

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    return shadow;
}

void main() {
    vec3 norm = normalize(Normal);

    // === Направленный свет ===
    vec3 lightDirNorm = normalize(-globalLightDir);
    float diff = max(dot(norm, lightDirNorm), 0.0);
    vec3 directional = diff * globalLightColor;
    float shadow = ShadowCalculation(FragPosLightSpace);

    // === Точечные источники света ===
    vec3 pointLightResult = vec3(0.0);
    for(int i = 0; i < numPointLights; ++i) {
        PointLight light = pointLights[i];
        vec3 toLight = light.position.xyz - FragPos;
        float dist = length(toLight);
        if(dist > light.position.w)
            continue;

        vec3 lightDir = normalize(toLight);
        float attenuation = 1.0 - clamp(dist / light.position.w, 0.0, 1.0);
        float diff = max(dot(norm, lightDir), 0.0);
        pointLightResult += diff * light.color * attenuation;
    }

    vec3 ambient = ambientLightColor;
    vec3 finalColor = (ambient + (1.0 - shadow) * directional + pointLightResult) * Color;

    FragColor = vec4(finalColor, 1.0);
}
