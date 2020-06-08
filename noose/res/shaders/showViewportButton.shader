uniform float radius;

#define fadeRange 0.05
#define plusSignLength 0.4
#define plusSignThickness 0.03

void main()
{
    vec2 uv = gl_TexCoord[0].xy;

    // circle
    gl_FragColor.rgb = gl_Color.rgb;
    gl_FragColor.a = 1.0 - smoothstep(radius - fadeRange / 2.0, radius + fadeRange / 2.0, distance(uv, vec2(0.5, 0.5)));

    // eye
    float whiteness;
    whiteness  = 1.0 - smoothstep(radius * 0.7 - fadeRange / 2.0, radius * 0.7 + fadeRange / 2.0, distance(uv, vec2(0.5, 0.7)));
    whiteness *= 1.0 - smoothstep(radius * 0.7 - fadeRange / 2.0, radius * 0.7 + fadeRange / 2.0, distance(uv, vec2(0.5, 0.3)));
    whiteness *=       smoothstep(radius * 0.25 - fadeRange / 2.0, radius * 0.25 + fadeRange / 2.0, distance(uv, vec2(0.5, 0.5)));
    gl_FragColor.rgb += vec3(whiteness);
}