uniform float vertical;

void main()
{
	if (mod(vertical, 2.0) == 1.0)
		gl_FragColor.rgb = vec3(1.0 - gl_TexCoord[0].t);
	else
		gl_FragColor.rgb = vec3(gl_TexCoord[0].s);

	gl_FragColor.a = 1.0;
}