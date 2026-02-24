#version 330

//inputs
//layout(location=0) in vec3 aPos;
//layout(location=1) in vec3 aColor;
in vec3 vertexPosition;
in vec4 vertexColor;
in vec2 vertexTexCoord;
in vec3 vertexNormal;

uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 matNormal;

uniform vec4 posOffset;
uniform vec2 waterMovement;

//outputs
out vec4 fragColor;
out vec2 fragTexCoord;
out vec3 fragNormal;
out vec3 fragPos;

void main()
{
    //gl_Position = mvp * vec4(vertexPosition.x+posOffset.x,vertexPosition.y+posOffset.y,vertexPosition.z+posOffset.z,1.0);//vec4(aPos,1.0);
    gl_Position = mvp * vec4(vertexPosition,1.0);
    fragColor = vertexColor;
    
    //fragTexCoord = vertexTexCoord + waterMovement;
    fragTexCoord = vertexTexCoord;

    fragNormal = normalize(vec3(matNormal * vec4(vertexNormal,1.0)));

    fragPos = vec3(matModel*vec4(vertexPosition, 1.0));
}