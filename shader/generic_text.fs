//fragment shader

uniform vec4 color;
uniform sampler2D samp;
varying vec2 texCoordOut;

void main()
{
	float t;
	vec4 tex = texture2D(samp, texCoordOut);

	if (tex.r>0.7)
		t = 1.0;
	else
		t = 0.0;

	gl_FragColor = vec4(tex.rgb, t);
}
