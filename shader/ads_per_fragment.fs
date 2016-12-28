//fragment shader

uniform mat4 mView;
uniform vec4 color;

varying vec4 vertexPosition;
varying vec4 normalPosition;

const vec3 lightSource = vec3(100.0, 100.0, 100.0);

vec3 calcADS(vec3 c, vec3 vertex, vec3 normal, vec3 light)
{
	vec3 A = 0.1*c.rgb;
	vec3 D = c.rgb;
	vec3 S = vec3(0.0);

	vec3 s = normalize(light - vertex);
	vec3 n = normalize(normal);
	vec3 v = normalize(-vertex);
	vec3 r = reflect(-s,n);

	float diffuseIntensity = max(dot(s,n), 0.0);
	if( diffuseIntensity > 0.0 )
		S = vec3(1.0,1.0,0.6) * pow( max( dot(r,v), 0.0 ), 100.0 );

	return A + diffuseIntensity*D + S;
}

void main()
{
	vec4 lightPosition = mView * vec4(lightSource, 1.0);

	vec3 ADS = calcADS(color.rgb, vertexPosition.xyz, normalPosition.xyz, lightPosition.xyz);

	//gl_FragColor = vec4(color);
	gl_FragColor = vec4(ADS, 1.0);
}
