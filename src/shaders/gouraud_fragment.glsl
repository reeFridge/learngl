#version 330 core
out vec4 FragColor;

in vec3 ResultColor;

void main()
{
	    FragColor = vec4(ResultColor, 1.0);
}
