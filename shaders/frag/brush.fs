#version 330 core
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 brush;
uniform vec4 mouseIn;
uniform vec2 textureSize;
uniform float step;

uniform sampler2D sampler0;


void main()
{
  vec2 m = mouseIn.xy;
  vec2 d = mouseIn.zw / step;
  
	float s = 0.0;

	for(float i=0;i<step;i++)
	{
	  float r = length(gl_FragCoord.xy - m);
	  s += smoothstep(brush.y, brush.z, r)*brush.x;
	  
	  m += d;
	}
	FragColor = texture(sampler0, TexCoord+d*s/textureSize);

}



















