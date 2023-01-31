#version 450 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

struct Lamp {
    vec3 lightPos;
    vec4 lightColor;
    vec3 viewPos;
};

uniform bool isRefract;
uniform bool isReflect;

uniform float refractVal;

uniform float ratioRefractReflect;

uniform samplerCube skybox;

uniform vec4 objectColor;
uniform Lamp lamp;

void main() {
    vec3 I = normalize(FragPos - lamp.viewPos);
    vec3 resultColor = objectColor.xyz;
    
    if (isReflect || isRefract) {
        vec3 rfl, rfr = vec3(1.0);
        if (isReflect)
            rfl = reflect(I, normalize(Normal));
        if (isRefract)
            rfr = refract(I, normalize(Normal), refractVal);

        resultColor = mix(texture(skybox, rfr).rgb, texture(skybox, rfl).rgb, ratioRefractReflect);
    }


    // ambient
    float ambientStrength = 0.5;
    vec3 ambient;
    ambient = ambientStrength * lamp.lightColor.xyz;
    
    // diffuse
    vec3 lightDir;
    float diff;
    vec3 diffuse;
    
    vec3 norm = normalize(Normal);
    lightDir = normalize(lamp.lightPos - FragPos);
    diff = max(dot(norm, lightDir), 0.0);
    diffuse = diff * lamp.lightColor.xyz;
    
    // specular
    float specularStrength = 0.5; // this is set higher to better show the effect of Gouraud shading 
    
    vec3 viewDir = normalize(lamp.viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lamp.lightColor.xyz;
    
    // result
    vec3 result = vec3(0.0);
    result += (ambient + diffuse + specular);
    result *= mix(objectColor.xyz, resultColor, 0.5);

    //FragColor = lamp.lightColor;

    FragColor = vec4(result, 1.0);
}