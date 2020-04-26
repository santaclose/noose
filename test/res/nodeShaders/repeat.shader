uniform sampler2D tex;
uniform vec2 originalSize;
uniform vec2 newSize;

void main()
{
	vec4 t0 = texture2D(tex, vec2(mod(gl_FragCoord.x, originalSize.x), mod(gl_FragCoord.y, originalSize.y))/originalSize);
	gl_FragColor = t0;
}