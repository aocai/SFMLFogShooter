uniform sampler2D	texture;
uniform vec2		offset;
uniform vec2		lightPosition;

void main()
{
	vec2 textureCoord = gl_TexCoord[0].xy;
	vec4 color = texture2D(texture, textureCoord) * 0.2270270270;
	color += texture2D(texture, textureCoord - 4.0 * offset) * 0.0162162162;
	color += texture2D(texture, textureCoord - 3.0 * offset) * 0.0540540541;
	color += texture2D(texture, textureCoord - 2.0 * offset) * 0.1216216216;
	color += texture2D(texture, textureCoord - offset) * 0.1945945946;
	color += texture2D(texture, textureCoord + offset) * 0.1945945946;
	color += texture2D(texture, textureCoord + 2.0 * offset) * 0.1216216216;
	color += texture2D(texture, textureCoord + 3.0 * offset) * 0.0540540541;
	color += texture2D(texture, textureCoord + 4.0 * offset) * 0.0162162162;
	gl_FragColor = color;
}