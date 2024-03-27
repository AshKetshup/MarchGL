#version 330 core

out vec4 FragColor;
uniform float iTime;

void main() {
    gl_FragColor = vec4(1.0, 1.0, 1.0, smoothstep(3.0, 0.0, iTime-2.0));
}