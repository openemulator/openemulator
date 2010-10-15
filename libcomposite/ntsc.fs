/*
 * GLSL NTSC Shader 1.01
 *
 * (C) 2010 by Marc S. Ressl/Gortu
 * Takes as input a raw NTSC frame
 *
 * Parameters:
 * size_*          - texture size
 * comp_fsc        - subcarrier to sampling ratio
 *                   (Apple II uses 0.25)
 * comp_black      - composite black level
 *                   (usually 0.2)
 * comp_white      - composite white level
 *                   (usually 0.8)
 * comp_hue        - hue phase adjustment
 *                   (0 to 1)
 * comp_saturation - chroma gain
 *                   (0 to 1)
 * rgb_*           - rgb gains
 *                   (0 to 1, 1 default)
 * vid_barrel      - Monitor barrel effect
 *                   (0 to 1)
 * vid_brightness  - Monitor brightness
 *                   (-1 to 1, 0 default)
 * vid_contrast    - Monitor contrast
 *                   (0 to 1, 1 default)
 */

uniform sampler2DRect texture;
uniform float size_x;
uniform float size_y;
uniform float comp_fsc;
uniform float comp_black;
uniform float comp_white;
uniform float comp_saturation;
uniform float comp_hue;
uniform float rgb_red;
uniform float rgb_green;
uniform float rgb_blue;
uniform float vid_contrast;
uniform float vid_brightness;
uniform float vid_barrel;

vec3 demodulate(vec2 q, float i)
{
	float phase1 = 2.0 * 3.1415926535 * comp_fsc * (q.x + i);
	vec3 p = texture2DRect(texture, vec2(q.x + i, q.y)).xyz;
	p -= comp_black;
	p.y *= sin(phase1);
	p.z *= cos(phase1);
	return p;
}

vec3 filter(vec2 q, float i, float cy, float cc)
{
	return (demodulate(q, i) + demodulate(q, -i)) * vec3(cy, cc, cc);
}

// x, y, z is used as Y'UV
void main(void)
{
	float hue = 2.0 * 3.1415926535 * comp_hue;
	
	// Decoder matrix
	mat3 decoderMatrix = mat3(
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0);
	// Contrast gain
	decoderMatrix *= vid_contrast;
	// RGB gain
	decoderMatrix *= mat3(
		rgb_red, 0.0, 0.0,
		0.0, rgb_green, 0.0,
		0.0, 0.0, rgb_blue);
	// Y'UV to RGB decoder matrix
	decoderMatrix *= mat3(
		1.0, 1.0, 1.0,
		0.0, -0.394642, 2.032062,
		1.139883,-0.580622, 0.0);
	// Hue gain
	decoderMatrix *= mat3(
		1.0, 0.0, 0.0,
		0.0, cos(hue), -sin(hue),
		0.0, sin(hue), cos(hue));
	// Saturation gain
	decoderMatrix *= mat3(
		1.0, 0.0, 0.0,
		0.0, comp_saturation, 0.0,
		0.0, 0.0, comp_saturation);
	// Demodulator gain
	decoderMatrix *= mat3(
		1.0, 0.0, 0.0,
		0.0, sqrt(2.0), 0.0,
		0.0, 0.0, sqrt(2.0));
	// Dynamic range gain
	decoderMatrix /= (comp_white - comp_black);
	
	// Barrel effect
	vec2 q = gl_TexCoord[0].xy;
	q = (q / vec2(size_x, size_y)) - vec2(0.5, 0.5);
	q += vid_barrel * q * (q.x * q.x + q.y * q.y);
	q = (q + vec2(0.5, 0.5)) * vec2(size_x, size_y);
	
	// FIR filter
	vec3 col = filter(q, 8.0, 0.001834, 0.005608678704129);
	col += filter(q, 7.0, 0.001595, 0.013136133246966);
	col += filter(q, 6.0, -0.006908, 0.025634921259280);
	col += filter(q, 5.0, -0.023328, 0.042403293307818);
	col += filter(q, 4.0, -0.025079, 0.062044634750949);
	col += filter(q, 3.0, 0.023269, 0.082124466712280);
	col += filter(q, 2.0, 0.130295, 0.099609418890150);
	col += filter(q, 1.0, 0.248233, 0.111545537317893);
	col += filter(q, 0.0, 0.300177 / 2.0, 0.115785831621067 / 2.0);
	col = decoderMatrix * col + vid_brightness;
	
	gl_FragColor = vec4(col, 1.0);
}
