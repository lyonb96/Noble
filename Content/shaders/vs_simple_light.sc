$input a_position, a_texcoord0, a_normal, a_tangent
$output v_worldpos, v_texcoord, v_worldnorm

#include "common/common.sh"

void main()
{
	v_worldpos = mul(u_model[0], vec4(a_position.xyz, 1.0));
	gl_Position = mul(u_modelViewProj, vec4(a_position.xyz, 1.0));

	v_texcoord = a_texcoord0;

	vec3 binormal = cross(a_tangent, a_normal);
	v_worldnorm = mat3(a_tangent, binormal, a_normal);
}