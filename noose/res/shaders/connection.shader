uniform float ratio;

void main()
{
    if (gl_TexCoord[0].y < ratio && gl_TexCoord[0].y > 1.0 - ratio)
        gl_FragColor = vec4(gl_Color.xyz, gl_Color.a/*1.0*/);
    else
    {
        if (gl_TexCoord[0].y > 0.5)
            gl_FragColor = vec4(gl_Color.xyz, gl_Color.a * (1.0 - gl_TexCoord[0].y) / (1.0 - ratio));
        else
          gl_FragColor = vec4(gl_Color.xyz, gl_Color.a * gl_TexCoord[0].y / (1.0 - ratio));
    }
}