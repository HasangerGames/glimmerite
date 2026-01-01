$input v_texcoord0, v_color0

#include <bgfx_shader.sh>

SAMPLER2D(s_tex, 0);

void main() {
    float layer = floor(v_texcoord0.y);
    vec2 coord = vec2(v_texcoord0.x, fract(v_texcoord0.y));
    vec4 tex = texture2D(s_tex, coord);
    gl_FragColor = tex * v_color0;
}
