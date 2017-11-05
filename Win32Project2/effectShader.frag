uniform vec2		lightPosition;
uniform float		distanceThreshold;
uniform vec4		spriteColor;

void main()
{
	float dist = distance(gl_FragCoord, lightPosition);
	//vec4 t1 = vec4(spriteColor.rgb, max(0, (distanceThreshold - dist) / distanceThreshold));
	//gl_FragColor = mix(t0, t1, t1.a);
	gl_FragColor = vec4(max(0, (distanceThreshold - dist) / distanceThreshold), 0.,0.,max(0, (distanceThreshold - dist) / distanceThreshold));
}