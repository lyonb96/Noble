$input v_worldpos, v_texcoord, v_worldnorm

#include "common/common.sh"

SAMPLER2D(DiffuseTex, 0);
SAMPLER2D(NormalTex, 1);

void main()
{
	vec4 color = texture2D(DiffuseTex, v_texcoord);
	color.a = 1.0F;
	gl_FragColor = color;
}