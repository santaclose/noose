uniform sampler2D image;
uniform sampler2D kernel;
uniform vec2 imageSize;
uniform vec2 kernelSize;

void main()
{
	vec4 outColor = vec4(0.0, 0.0, 0.0, 0.0);

	for (int i = 0; i < int(kernelSize.x); i++)
	{
		for (int j = 0; j < int(kernelSize.y); j++)
		{
			vec2 kernelSamplePoint = vec2(float(i) / kernelSize.x, float(j) / kernelSize.y);
			vec2 imageSamplePoint = vec2(
				clamp((float(gl_FragCoord.x) - float(kernelSize.x) / 2.0 + float(i)) / imageSize.x, 0.0, 1.0),
				clamp((float(gl_FragCoord.y) - float(kernelSize.y) / 2.0 + float(j)) / imageSize.y, 0.0, 1.0));
			outColor += 1.0 - abs(texture2D(kernel, kernelSamplePoint) - texture2D(image, imageSamplePoint));
		}
	}
	outColor /= kernelSize.x * kernelSize.y;

	gl_FragColor = outColor;
}