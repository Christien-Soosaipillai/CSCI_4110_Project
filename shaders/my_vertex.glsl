uniform mat4 u_MVPMatrix;
uniform mat4 u_MVMatrix;
uniform vec4 u_DiffuseColour;

attribute vec4 position;
attribute vec3 normal;
attribute vec2 textureCoords;

varying vec3 v_Position;
varying vec3 v_Normal;
varying vec2 v_TextureCoords;

void main() {
	v_TextureCoords = textureCoords;
    v_Position = vec3(u_MVMatrix * position);
    v_Normal = vec3(u_MVMatrix * vec4(normal, 0.0));

    gl_Position = u_MVPMatrix * position;
}
