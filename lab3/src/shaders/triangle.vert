#version 330 core

layout (location = 0) in vec3 iPosition;
uniform mat4 trans;

void main() {
  gl_Position = trans * vec4(iPosition.xyz, 1.0);
}