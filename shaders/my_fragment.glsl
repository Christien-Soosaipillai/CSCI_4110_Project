uniform vec3 u_LightPos;
uniform vec4 u_DiffuseColour; //rgb + w (vec4)
uniform vec3 u_EyePosition; //camera position
uniform float u_Shininess; //specular shininess term

uniform sampler2D u_TextureSampler;

varying vec3 v_Position;
varying vec3 v_Normal;
varying vec2 v_TextureCoords;

void main() {
	vec3 baseColour = texture(u_TextureSampler, v_TextureCoords).rgb;
	//gl_FragColor = vec4(baseColour, 1.0);


	vec3 N = normalize(v_Normal);
	vec3 V = normalize(u_EyePosition - v_Position);
	
	//phong/blinn-phong - ASD (Ambient, Specular, Diffuse)
	float ambient = 0.2f;
	float diffuse = 0.0f;
	float specular = 0.0f;
	
	vec3 L = vec3(0.0f);

	//point light
	L = normalize(u_LightPos - v_Position);

	//directional light
	//L = normalize(-u_LightPos);
	
	//blinn-phong
	vec3 H = normalize(L+V);
	float specularAngle = max(0.0f, dot(V, H));
	specular += pow(specularAngle, u_Shininess);
	
	diffuse += max(0.0f, dot(N, L));

	//all color components
    gl_FragColor = (u_DiffuseColour*(ambient+diffuse+specular)) * vec4(baseColour, 1.0);

    //only diff color
    //gl_FragColor = (u_DiffuseColour*(ambient+diffuse+specular));

    //only texture color
    //gl_FragColor = ((ambient+diffuse+specular))*vec4(baseColour, 1.0);

    //normals
	//gl_FragColor = vec4(N, 1.0);
}
