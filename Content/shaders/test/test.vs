$input a_position, a_weight, a_texcoord0, a_normal
$output v_color0

/*
 * Copyright 2011-2019 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#include "../common/common.sh"

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position.xyz, 1.0) );
	v_color0 = vec4(1.0, 1.0, 1.0, 1.0);
}
