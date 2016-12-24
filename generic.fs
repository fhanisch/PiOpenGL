//fragment shader

uniform vec4 color;
uniform sampler2D samp;
varying vec2 texCoordOut;

void main()
{
	vec4 outColor;
	int i=1;
/*
	if (i==0)
		outColor = texture2D(samp, texCoordOut);
	else
		outColor = color;
*/
	gl_FragColor = vec4(color);
}
