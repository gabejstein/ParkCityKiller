#version 330

uniform samplerCube environmentMap;

in vec3 fragPosition;
out vec4 finalColor;

void main()
{
    finalColor = texture(environmentMap,fragPosition).rgba;
}