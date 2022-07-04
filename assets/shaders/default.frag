#version 120

uniform sampler2D u_texture;
uniform vec4 u_color;

varying vec2 v_texCoord;
varying vec4 v_color;

void main() {
    vec4 texColor = texture2D(u_texture, v_texCoord);
    gl_FragColor = texColor * v_color * u_color;
}
