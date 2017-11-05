uniform vec2		lightPosition;
uniform float		lightRadii;
uniform int			time;

void main()
{
    float dist = distance(gl_FragCoord, lightPosition);
    
    vec4 t0 = vec4(1.,1.-0.5*min(1.,dist/lightRadii),0.5-pow(dist/lightRadii,3.),1.);
	vec4 t1 = vec4(0.,0.,0.,min(1.0, dist/(lightRadii + (0.1*(abs(cos(time/5.)) + 1.))*lightRadii*abs(sin(time/5.)))));
	gl_FragColor = mix(t0, t1, t1.a);
}