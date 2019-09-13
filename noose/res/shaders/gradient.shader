uniform int limit = 1;

void main()
{
	// 0.0 * 0.8 + 0.2 = 0.2
	// 1.0 * 0.8 + 0.2 = 1.0

	// 0.0 / 0.8 +
	// 1.0 / 0.8 + 0.2 = 1.0
	const float ratio = 0.6;
	float newUV = clamp(gl_TexCoord[0].y / ratio, 0.0, 1.0);
	gl_FragColor = vec4(vec3(newUV), 1.0);
}