uniform sampler2D tex;
uniform vec2 topLeft;
uniform vec2 ratio;

void main()
{
	vec2 frameUV = vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t);

	vec2 targetUV = frameUV - vec2(topLeft.x * ratio.x, topLeft.y * ratio.y);
	targetUV = vec2(targetUV.x / ratio.x, targetUV.y / ratio.y);

	//gl_FragColor = vec4(ratio.x, ratio.y, 0.0, 1.0);
	gl_FragColor = texture2D(tex, targetUV);
}