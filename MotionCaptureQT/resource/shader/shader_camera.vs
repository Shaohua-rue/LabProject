#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 Rotate;
uniform vec3 Position;

vec4 APos=Rotate*vec4(aPos/5.0f,1.0);
out vec3 ourColor;
void main()
{
    gl_Position = projection * view * model*vec4(APos.x+Position.x, APos.y+Position.y,APos.z+Position.z,1.0);
    ourColor = vec3(0.2f, 0.1f, 0.2f);
}
