//fragment shader

uniform vec4 color;
uniform sampler2D samp;
varying vec2 texCoordOut;

void main()
{
	float t;
	vec3 outColor;
	vec4 tex = texture2D(samp, texCoordOut);

	if (tex.r<0.5)
	{
		t = 1.0;
		outColor = vec3(1.0,0.0,0.0);
	}
	else
	{
		t = 0.0;
		outColor = color.rgb;
	}

	gl_FragColor = vec4(outColor, t);
}
