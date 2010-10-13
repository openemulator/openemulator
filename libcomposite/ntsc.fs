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

// x, y, z is used as Y'UV
void main(void)
{
	mat3 decoderMatrix = mat3(1.0, 1.0, 1.0, 0.0, -0.394642, 2.032062, 1.139883,-0.580622, 0.0);

	vec2 q = gl_FragCoord.xy;
	vec2 qn = q / 512.0;
	qn.y = 1.0 - qn.y;
	vec3 col = vec3(0.0, 0.0, 0.0);
	for (float i = -11.0; i <= 11.0; i += 1.0)
	{
		vec3 p = texture2D(texture, vec2(qn.x + i / 512.0, qn.y)).xyz;
		p.y *= sin(2.0 * 3.1415926535 * ((q.x + i) * fsc + hue)) * 2.0 * saturation;
		p.z *= cos(2.0 * 3.1415926535 * ((q.x + i) * fsc + hue)) * 2.0 * saturation;
		col += p;
	}
	col /= 23.0;
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
