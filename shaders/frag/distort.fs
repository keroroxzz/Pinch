#version 330 core
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D sampler0;
uniform sampler2D sampler1;

void main()
{
  vec2 displacement = texture(sampler0, TexCoord).xy;
  vec4 sampling = texture(sampler1, displacement).xyzw;
  
  FragColor = sampling.zyxw;
}



















