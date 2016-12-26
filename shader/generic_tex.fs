//fragment shader

uniform vec4 color;
uniform sampler2D samp;
varying vec2 texCoordOut;

void main()
{
	vec4 tex = texture2D(samp, texCoordOut);
	gl_FragColor = vec4(tex.rgb, 1.0);
}
