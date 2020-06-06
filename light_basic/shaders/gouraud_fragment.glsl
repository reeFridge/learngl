#version 330 core
out vec4 FragColor;

in vec3 ResultColor;

uniform vec3 objColor;

void main()
{
	    FragColor = vec4(ResultColor * objColor, 1.0);
}
