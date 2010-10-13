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
	float phase = 2.0 * 3.1415926535 * ((512.0 * q.x + i) * fsc + hue);
	vec3 p1 = texture2D(texture, vec2(q.x + i / 512.0, q.y)).xyz;
	p1.y *= sin(phase) * sqrt(2.0) * saturation;
	p1.z *= cos(phase) * sqrt(2.0) * saturation;
	
	float phase2 = 2.0 * 3.1415926535 * ((512.0 * q.x - i) * fsc + hue);
	vec3 p2 = texture2D(texture, vec2(q.x - i / 512.0, q.y)).xyz;
	p2.y *= sin(phase2) * sqrt(2.0) * saturation;
	p2.z *= cos(phase2) * sqrt(2.0) * saturation;
	
	vec3 c = vec3(cy, cc, cc);
	return (p1 + p2) * c;
}

// x, y, z is used as Y'UV
void main(void)
{
	mat3 decoderMatrix = mat3(1.0, 1.0, 1.0, 0.0, -0.394642, 2.032062, 1.139883,-0.580622, 0.0);

//	vec2 q = gl_TexCoord[0].xy;
	vec2 q = gl_FragCoord.xy / 512.0;
	q.y = 1.0 - q.y;
	vec3 col = filter(q, 8.0, 0.001834, 0.005608678704129);
	col += filter(q, 7.0, 0.001595, 0.013136133246966);
	col += filter(q, 6.0, -0.006908, 0.025634921259280);
	col += filter(q, 5.0, -0.023328, 0.042403293307818);
	col += filter(q, 4.0, -0.025079, 0.062044634750949);
	col += filter(q, 3.0, 0.023269, 0.082124466712280);
	col += filter(q, 2.0, 0.130295, 0.099609418890150);
	col += filter(q, 1.0, 0.248233, 0.111545537317893);
	col += filter(q, 0.0, 0.300177 / 2.0, 0.115785831621067 / 2.0);
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
