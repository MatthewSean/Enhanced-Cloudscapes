#include <post_processing_program.hpp>

#include <opengl_helpers.hpp>

#include <simulator_objects.hpp>
#include <plugin_objects.hpp>

#include <XPLMGraphics.h>

namespace post_processing_program
{
	GLuint reference;

	GLint near_clip_z;
	GLint bicubic_sampling;

	void initialize()
	{
		GLuint vertex_shader = load_shader("Resources/plugins/Enhanced Cloudscapes/shaders/post_processing/vertex_shader.glsl", GL_VERTEX_SHADER);
		GLuint fragment_shader = load_shader("Resources/plugins/Enhanced Cloudscapes/shaders/post_processing/fragment_shader.glsl", GL_FRAGMENT_SHADER);

		reference = create_program(vertex_shader, fragment_shader);
		glUseProgram(reference);

		GLint rendering_texture = glGetUniformLocation(reference, "rendering_texture");
		glUniform1i(rendering_texture, 0);

		near_clip_z = glGetUniformLocation(reference, "near_clip_z");
		bicubic_sampling = glGetUniformLocation(reference, "bicubic_sampling");

		glUseProgram(EMPTY_OBJECT);
	}

	void call()
	{
		XPLMSetGraphicsState(0, 1, 0, 0, 1, 0, 0);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindVertexArray(plugin_objects::vertex_array);
		glUseProgram(reference);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, plugin_objects::rendering_texture);

		glUniform1f(near_clip_z, simulator_objects::near_clip_z);
		glUniform1i(bicubic_sampling, simulator_objects::bicubic_sampling);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		XPLMBindTexture2d(EMPTY_OBJECT, 0);

		glUseProgram(EMPTY_OBJECT);
		glBindVertexArray(EMPTY_OBJECT);

		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);

	}
}