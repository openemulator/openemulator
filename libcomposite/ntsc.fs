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

vec3 filter(vec2 q, float i, float cy, float cc)
{
	vec2 qn = q / 512.0;
	qn.y = 1.0 - qn.y;
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
	vec3 col = vec3(0.0, 0.0, 0.0);
	col += filter(q, -8.0, 0.001834, 0.005171);
	col += filter(q, -7.0, 0.001595, 0.012393);
	col += filter(q, -6.0, -0.006908, 0.024668);
	col += filter(q, -5.0, -0.023328, 0.041486);
	col += filter(q, -4.0, -0.025079, 0.061527);
	col += filter(q, -3.0, 0.023269, 0.082293);
	col += filter(q, -2.0, 0.130295, 0.100559);
	col += filter(q, -1.0, 0.248233, 0.113111);
	col += filter(q, 0.0, 0.300177, 0.117585);
	col += filter(q, -1.0, 0.248233, 0.113111);
	col += filter(q, -2.0, 0.130295, 0.100559);
	col += filter(q, -3.0, 0.023269, 0.082293);
	col += filter(q, -4.0, -0.025079, 0.061527);
	col += filter(q, -5.0, -0.023328, 0.041486);
	col += filter(q, -6.0, -0.006908, 0.024668);
	col += filter(q, -7.0, 0.001595, 0.012393);
	col += filter(q, -8.0, 0.001834, 0.005171);
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
