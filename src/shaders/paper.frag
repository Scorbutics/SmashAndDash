#version 450
// The world as a watercolour on paper (PaperLook, Renderer::setWorldShader : the world only, the interface over it is
// not) : the grain of the paper fixed to the screen (the sheet under the paint), the pigment that pools where the
// colours change (it also softens the edges of the pixel art), a warm wash, a soft vignette. Subtle : the painted tiles
// already carry their own wash (tools/render3d).
layout(location = 0) in vec4 v_color;
layout(location = 1) in vec2 v_uv;
layout(location = 0) out vec4 o_color;
layout(set = 2, binding = 0) uniform sampler2D u_texture;
layout(set = 3, binding = 0) uniform Uniforms {
	// x : grain, y : pooling at the edges, z : warm wash, w : vignette (0 .. 1 each)
	vec4 u_paper;
	// x, y : size of the screen (pixels), z : the seed of the paper
	vec4 u_screen;
};

float hash(vec2 p) {
	return fract(sin(dot(p, vec2(127.1, 311.7)) + u_screen.z) * 43758.5453);
}

float noise(vec2 p) {
	vec2 i = floor(p);
	vec2 f = fract(p);
	vec2 u = f * f * (3.0 - 2.0 * f);
	return mix(mix(hash(i), hash(i + vec2(1.0, 0.0)), u.x), mix(hash(i + vec2(0.0, 1.0)), hash(i + vec2(1.0, 1.0)), u.x), u.y);
}

void main() {
	vec2 texel = 1.0 / vec2(textureSize(u_texture, 0));
	vec4 color = texture(u_texture, v_uv);
	vec3 rgb = color.rgb;

	// The pigment pools where the colours change : darker along the edges (4 neighbours, a pixel and a half away)
	vec3 around = texture(u_texture, v_uv + vec2(1.5, 0.0) * texel).rgb + texture(u_texture, v_uv - vec2(1.5, 0.0) * texel).rgb
		+ texture(u_texture, v_uv + vec2(0.0, 1.5) * texel).rgb + texture(u_texture, v_uv - vec2(0.0, 1.5) * texel).rgb;
	float edge = clamp(length(rgb - around * 0.25) * 1.8, 0.0, 1.0);
	rgb *= 1.0 - 0.14 * edge * u_paper.y;

	// The paper : its fine grain, its fibres, an uneven wash, fixed to the screen
	vec2 p = gl_FragCoord.xy;
	float grain = noise(p / 1.6) * 0.55 + noise(p / 6.0) * 0.45;
	float wash = noise(p / 150.0);
	rgb *= 1.0 + (grain - 0.5) * 0.07 * u_paper.x + (wash - 0.5) * 0.05 * u_paper.x;

	// A warm wash : the colours drift a little towards the paper, the shadows lifted
	vec3 paper = vec3(1.0, 0.975, 0.91);
	rgb = mix(rgb, paper, 0.04 * u_paper.z);
	rgb = mix(rgb, rgb * paper, 0.25 * u_paper.z);

	// A soft vignette
	vec2 fromCenter = gl_FragCoord.xy / u_screen.xy - 0.5;
	rgb *= mix(1.0, smoothstep(0.95, 0.3, length(fromCenter)), 0.22 * u_paper.w);

	o_color = vec4(rgb * v_color.rgb, color.a);
}
