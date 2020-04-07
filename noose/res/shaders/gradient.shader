void main()
{
	const float ratio = 0.1;
	float c = gl_TexCoord[0].y * (ratio * 2.0 + 1.0) - ratio;
	gl_FragColor = vec4(vec3(c), 1.0);
}