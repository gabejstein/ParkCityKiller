#version 330

uniform vec4 baseColor;
uniform sampler2D texture0;
uniform vec3 cameraDir;
uniform vec3 camPos; //used for fog
uniform vec3 sunDir;

in vec4 fragColor;
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragPos;

out vec4 FragYou;

vec4 fogColor; //make uniform later
float fogDensity;

void main()
{
    fogColor = vec4(0.4,0.4,0.4,1.0);
    fogDensity = 0.02;

    vec4 texelColor = texture(texture0,fragTexCoord);

    if(texelColor.a <= 0.0)
    {
        discard;
    }
    //FragYou = fragColor;
    //FragYou = texelColor;
    //FragYou = texelColor * fragColor;
    //FragYou = baseColor;

    float ambientAmount = max(dot(normalize(sunDir),normalize(fragNormal)),0.8);
    float lightAmount = max(dot(normalize(cameraDir),normalize(fragNormal)),0.4); //should try mixing with light source to light ground better
    float finalLight = ambientAmount * lightAmount;

    FragYou = texelColor * fragColor * vec4(finalLight,finalLight,finalLight,1.0);

}