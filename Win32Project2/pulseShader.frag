uniform sampler2D	texture;
uniform vec2		lightPosition;
uniform float		distanceThreshold;
uniform int			time;

void main()
{
	float dist = distance(gl_FragCoord, lightPosition);

	/*
	float value = abs(sin(time));

	int i = int((dist - 40) / 5);
	gl_FragColor = vec4(0.04*abs(sin(time)) + 0.6 - i*0.02, 0.01*abs(sin(time)) + 0.15 - i*0.005, 0.0, 1.0);
	*/

	vec4 t0 = texture2D(texture, gl_TexCoord[0].xy);
	t0.a = 0.8;
	vec4 t1 = vec4(0.0,0.0,0.0,min(1.0, dist/distanceThreshold));
	gl_FragColor = mix(t0, t1, t1.a);
}