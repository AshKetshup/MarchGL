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


float hash( vec2 p ) {
	float h = dot(p,vec2(127.1,311.7));	
    return fract(sin(h)*43758.5453123);
}

float noisee( vec2 p ) {
    vec2 i = floor( p );
    vec2 f = fract( p );	
	vec2 u = f*f*(3.0-2.0*f);
    return -1.0+2.0*mix( mix( hash( i + vec2(0.0,0.0) ), 
                     hash( i + vec2(1.0,0.0) ), u.x),
                mix( hash( i + vec2(0.0,1.0) ), 
                     hash( i + vec2(1.0,1.0) ), u.x), u.y);
}

void main() {
    vec3 viewDir = normalize(lamp.viewPos - FragPos);
    vec3 normal = normalize(Normal);


    vec3 I = normalize(FragPos - lamp.viewPos);
    vec3 resultColor = objectColor.xyz;
    
    if (isReflect || isRefract) {
        vec3 rfl, rfr = vec3(1.0);
        if (isReflect)
            rfl = reflect(I, normal);
        if (isRefract)
            rfr = refract(I, normal, refractVal);

        //float ratio = mix(noisee(normalize(vec2(rfr.x, rfl.y))), noisee(normalize(vec2(rfr.y, rfl.z))), 0.5);
        //resultColor = mix(texture(skybox, rfr).rgb, texture(skybox, rfl).rgb, ratio);
        //resultColor = texture(skybox, mix(rfr, rfl, ratioRefractReflect)).rgb;
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