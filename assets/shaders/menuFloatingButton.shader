uniform float radius;

#define fadeRange 0.03
#define lineLength 0.4
#define lineThickness 0.04

void main()
{
    vec2 uv = gl_TexCoord[0].xy;

    // circle
    gl_FragColor.rgb = gl_Color.rgb;
    gl_FragColor.a = 1.0 - smoothstep(radius - fadeRange / 2.0, radius + fadeRange / 2.0, distance(uv, vec2(0.5, 0.5)));

    // plus sign
    if (uv.x > 0.5 - lineLength * 0.5 && uv.x < 0.5 + lineLength * 0.5 && (
            uv.y > 0.50 - lineThickness * 0.5 && uv.y < 0.50 + lineThickness * 0.5 ||
            uv.y > 0.62 - lineThickness * 0.5 && uv.y < 0.62 + lineThickness * 0.5 ||
            uv.y > 0.38 - lineThickness * 0.5 && uv.y < 0.38 + lineThickness * 0.5 ))
        gl_FragColor.rgb += vec3(0.6);
}