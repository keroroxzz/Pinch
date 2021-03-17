#version 330 core
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 brushIn;
uniform vec4 mouseIn;
uniform vec2 textureSize;


uniform sampler2D sampler0;
uniform sampler2D sampler1;

void main()
{
  int x = int(TexCoord.x*50.0);
  int y = int(TexCoord.y*50.0);
  vec3 checkboard = vec3((x+y)%2)*0.25+0.5;
  
  float r = length(TexCoord.xy*textureSize - mouseIn.xy);
  vec4 brush = vec4(1.0, 0.0, 0.0, 0.5*smoothstep(brushIn.y, brushIn.z, r));

  vec2 displacement = texture(sampler0, TexCoord).xy;
  vec4 sampling = texture(sampler1, displacement).xyzw;
  
  vec4 r0 = vec4(checkboard, 1.0);
  r0 = r0 *(1.0-sampling.w) + sampling*sampling.w;
  r0 = r0 *(1.0-brush.w) + brush*brush.w;
  
  float ratio = 0.75 - abs(brushIn.y - r);
  ratio = (ratio<0.0?0.0:ratio)*0.5;
  
  FragColor =  vec4(vec3(ratio) + r0.xyz*(1.0 - 2.0*ratio), 1.0);
}



















