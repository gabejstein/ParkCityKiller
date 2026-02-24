//This is testing to see if the model will always appear on top of everything else.
//Currently this just draws an object flatly on top of everything else. It might be necessary to
//create a custom uniform for a different camera position in order to give the object some perspective.

#version 330

in vec3 vertexPosition;
out vec3 fragPosition;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

void main()
{
	fragPosition = vertexPosition;

	mat4 matRot = mat4(mat3(matView)); //strip away translation

	gl_Position = matRot * matModel * vec4(vertexPosition,1.0f);

}