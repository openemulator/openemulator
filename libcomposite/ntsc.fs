#ifdef GL_ES
precision highp float;
#endif

uniform sampler2D texture;
uniform float contrast;
uniform float brightness;
uniform float saturation;
uniform float hue;
uniform float fsc;
uniform float flicker;
uniform float time;

vec3 filter(vec2 q, vec2 qn, float i, float cy, float cc)
{
		vec3 p = texture2D(texture, vec2(qn.x + i / 512.0, qn.y)).xyz;
		p.x *= cy;
		p.y *= cc * sin(2.0 * 3.1415926535 * ((q.x + i) * fsc + hue)) * sqrt(2.0) * saturation;
		p.z *= cc * cos(2.0 * 3.1415926535 * ((q.x + i) * fsc + hue)) * sqrt(2.0) * saturation;
		return p;
}

// x, y, z is used as Y'UV
void main(void)
{
	mat3 decoderMatrix = mat3(1.0, 1.0, 1.0, 0.0, -0.394642, 2.032062, 1.139883,-0.580622, 0.0);

	vec2 q = gl_FragCoord.xy;
	vec2 qn = q / 512.0;
	qn.y = 1.0 - qn.y;
	vec3 col = vec3(0.0, 0.0, 0.0);
	col += filter(q, qn, -8.0, 0.000000238419, 0.0);
	col += filter(q, qn, -7.0, 0.000000043104, 0.0);
	col += filter(q, qn, -6.0, 0.000000024426, 0.0);
	col += filter(q, qn, -5.0, 0.000000021192, 0.0);
	col += filter(q, qn, -4.0, 0.000000055881, 0.0);
	col += filter(q, qn, -3.0, 0.000000024810, 0.0);
	col += filter(q, qn, -2.0, 0.100000077592, 0.0);
	col += filter(q, qn, -1.0, 0.200000037292, 0.0);
	col += filter(q, qn, 0.0, 0.500000000000, 0.0);
	col += filter(q, qn, -1.0, 0.200000037292, 0.0);
	col += filter(q, qn, -2.0, 0.100000077592, 0.0);
	col += filter(q, qn, -3.0, 0.000000024810, 0.0);
	col += filter(q, qn, -4.0, 0.000000055881, 0.0);
	col += filter(q, qn, -5.0, 0.000000021192, 0.0);
	col += filter(q, qn, -6.0, 0.000000024426, 0.0);
	col += filter(q, qn, -7.0, 0.000000043104, 0.0);
	col += filter(q, qn, -8.0, 0.000000238419, 0.0);
	col = decoderMatrix * col;
	col = col * contrast + brightness;

//	 Non-linearity
//    col = col*0.0+1.0*col*col;

// Cushion
//    col *= 0.5 + 0.5*16.0*uv.x*uv.y*(1.0-uv.x)*(1.0-uv.y);

// 	 Tint
///    col *= vec3(0.8, 1.0, 0.7);

//	Flicker
//    col *= (1.0 - flicker) + flicker * sin(110.0 * time);

	gl_FragColor = vec4(col, 1.0);
}
