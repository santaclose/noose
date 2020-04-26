uniform float radius;

#define fadeRange 0.05

void main()
{
    vec2 uv = gl_TexCoord[0].xy;

    // circle
    gl_FragColor.rgb = gl_Color.rgb;
    gl_FragColor.a = 1.0 - smoothstep(radius - fadeRange / 2.0, radius + fadeRange / 2.0, distance(uv, vec2(0.5, 0.5)));
}