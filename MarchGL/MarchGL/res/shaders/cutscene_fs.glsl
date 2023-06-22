#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform float iTime;

uniform sampler2D ourTexture;

void main() {
    gl_FragColor = vec4(texture(ourTexture, TexCoord).xyz, smoothstep(4.0, 0.0, iTime-2.0));
}