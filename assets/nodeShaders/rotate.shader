uniform sampler2D tex;
uniform vec2 originalSize;
uniform vec2 newSize;
uniform float angle;

//#define DEG2RAD 0.0174533

void main()
{
	//float angle = DEG2RAD * degrees;
	float ratioX = newSize.x / originalSize.x;
	float ratioY = newSize.y / originalSize.y;
	float borderX = ratioX - 1.0;
	float borderY = ratioY - 1.0;
	float aspectRatio = originalSize.x / originalSize.y;

	vec2 uv = vec2(gl_TexCoord[0].s * ratioX, (1.0 - gl_TexCoord[0].t) * ratioY);

	// translate
	uv.x -= borderX * 0.5 + 0.5;
	uv.y -= borderY * 0.5 + 0.5;

	uv.y /= aspectRatio; 

	// rotate
	float newX = uv.x * cos(angle) - uv.y * sin(angle);
	float newY = uv.x * sin(angle) + uv.y * cos(angle);
	uv.x = newX;
	uv.y = newY;

	uv.y *= aspectRatio; 

	// translate and center
	uv.x += /*borderX * 0.5*/ + 0.5;
	uv.y += /*borderY * 0.5*/ + 0.5;

	gl_FragColor = texture2D(tex, uv);
}