#version 450
// Battle screen (post-processing, GUIBattle) : the colors split on a strong hit, the screen goes gray with a vignette
// on a K.O.
layout(location = 0) in vec4 v_color;
layout(location = 1) in vec2 v_uv;
layout(location = 0) out vec4 o_color;
layout(set = 2, binding = 0) uniform sampler2D u_texture;
layout(set = 3, binding = 0) uniform Uniforms {
	// x : gray (0 to 1), y : vignette (0 to 1), z : split of the colors (pixels), w : unused
	vec4 u_effects;
	// x, y : size of the screen (pixels)
	vec4 u_screen;
};

void main() {
	vec2 fromCenter = v_uv - 0.5;
	// The red and the blue slide apart, sideways
	vec2 split = vec2(u_effects.z / u_screen.x, 0.0);
	vec4 color = texture(u_texture, v_uv);
	color.r = texture(u_texture, v_uv + split).r;
	color.b = texture(u_texture, v_uv - split).b;
	vec3 rgb = color.rgb * v_color.rgb;

	float gray = dot(rgb, vec3(0.299, 0.587, 0.114));
	rgb = mix(rgb, vec3(gray), u_effects.x);
	float vignette = smoothstep(0.85, 0.2, length(fromCenter));
	rgb *= mix(1.0, vignette, u_effects.y);
	o_color = vec4(rgb, 1.0);
}
