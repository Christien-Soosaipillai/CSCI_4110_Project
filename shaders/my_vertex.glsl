uniform mat4 u_MVPMatrix;
uniform mat4 u_MVMatrix;

attribute vec4 position;
attribute vec3 normal;
attribute vec2 textureCoords;

varying vec3 v_Position;
varying vec3 v_Normal;
varying vec2 v_TextureCoords;

void main() {
    gl_Position = u_MVPMatrix * position;
	v_TextureCoords = textureCoords;
    v_Normal = normal;
    v_Position = gl_Position.xyz;

    //v_Normal = vec3(u_MVMatrix * vec4(normal, 0.0));
}
