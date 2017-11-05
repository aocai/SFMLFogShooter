uniform sampler2D	texture;
uniform vec2		lightPosition;

void main()
{
	gl_FragColor = texture2D(texture, gl_TexCoord[0].xy);
	float value = distance(gl_FragCoord, lightPosition) / 750.f;
	gl_FragColor.r = max(0, gl_FragColor.r - value);
	gl_FragColor.g = max(0, gl_FragColor.g - value);
	gl_FragColor.b = max(0, gl_FragColor.b - value);
}