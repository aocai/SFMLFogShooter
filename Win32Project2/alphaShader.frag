uniform sampler2D	texture;
uniform sampler2D	alphaTexture;

void main()
{
	gl_FragColor = texture(texture, gl_TexCoord[0].xy);
	gl_FragColor.a = texture(alphaTexture, gl_TexCoord[0].xy).r;
}