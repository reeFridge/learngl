#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D t0;
uniform sampler2D t1;
uniform float mix_f;

void main()
{
    FragColor = mix(texture(t0, TexCoord), texture(t1, TexCoord), mix_f);
}
