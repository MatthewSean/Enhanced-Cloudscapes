#version 450 core

in vec2 fullscreen_texture_position;

uniform sampler2D rendering_texture;
layout(location = 0) out vec4 fragment_color;

#define CONSTANT_1 2.51
#define CONSTANT_2 0.03
#define CONSTANT_3 2.43
#define CONSTANT_4 0.59
#define CONSTANT_5 0.14

uniform int bicubic_sampling;
uniform int sample_pattern;

vec3 tone_mapping(vec3 input_color)
{
	return (input_color * ((CONSTANT_1 * input_color) + CONSTANT_2)) / ((input_color * ((CONSTANT_3 * input_color) + CONSTANT_4)) + CONSTANT_5);
}

// Catmull-Rom spline actually passes through control points
vec4 catmull(float x) // cubic_catmullrom(float x)
{
    const float s = 0.5; // potentially adjustable parameter
    float x2 = x * x;
    float x3 = x2 * x;
    vec4 w;
    w.x =    -s*x3 +     2*s*x2 - s*x + 0;
    w.y = (2-s)*x3 +   (s-3)*x2       + 1;
    w.z = (s-2)*x3 + (3-2*s)*x2 + s*x + 0;
    w.w =     s*x3 -       s*x2       + 0;
    return w;
}

vec4 cubic(float v)
{
    vec4 n = vec4(1.0, 2.0, 3.0, 4.0) - v;
    vec4 s = n * n * n;
    float x = s.x;
    float y = s.y - 4.0 * s.x;
    float z = s.z - 4.0 * s.y + 6.0 * s.x;
    float w = 6.0 - x - y - z;
    return vec4(x, y, z, w);
}

vec4 textureBicubic(sampler2D sampler, vec2 texCoords)
{

    vec4 xcubic;
    vec4 ycubic;
	vec2 texSize = textureSize(sampler, 0);
	vec2 invTexSize = 1.0 / texSize;
	texCoords = texCoords * texSize - 0.5;

    vec2 fxy = fract(texCoords);
    texCoords -= fxy;

    if(sample_pattern != 1)
    {
    xcubic = catmull(fxy.x);
    ycubic = catmull(fxy.y);
    }
    else
    {
    xcubic = cubic(fxy.x);
    ycubic = cubic(fxy.y);
    }

    vec4 c = texCoords.xxyy + vec2 (-0.5, +1.5).xyxy;

    vec4 s = vec4(xcubic.xz + xcubic.yw, ycubic.xz + ycubic.yw);
    vec4 offset = c + vec4 (xcubic.yw, ycubic.yw) / s;

    offset *= invTexSize.xxyy;

    vec4 sample0 = texture(sampler, offset.xz);
    vec4 sample1 = texture(sampler, offset.yz);
    vec4 sample2 = texture(sampler, offset.xw);
    vec4 sample3 = texture(sampler, offset.yw);

    float sx = s.x / (s.x + s.y);
    float sy = s.z / (s.z + s.w);

    return mix(mix(sample3, sample2, sx), mix(sample1, sample0, sx), sy);
}

void main()
{
	
	vec4 rendered_color = texture(rendering_texture, fullscreen_texture_position);
	if (bicubic_sampling != 0)
	{
	rendered_color = textureBicubic(rendering_texture, fullscreen_texture_position);
	}
	rendered_color.xyz = tone_mapping(rendered_color.xyz);
	fragment_color = rendered_color;
}