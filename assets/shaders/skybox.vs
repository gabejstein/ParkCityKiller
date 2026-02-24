#version 330

in vec3 vertexPosition;
uniform mat4 matProjection;
uniform mat4 matView;
uniform mat4 matModel;

uniform vec3 movement;

out vec3 fragPosition;

void main()
{
	fragPosition = vertexPosition;

	mat4 rotMatrix = mat4(mat3(matView));
	vec4 pos = matProjection * rotMatrix  * matModel * vec4(vertexPosition,1.0);

	gl_Position = pos;
}