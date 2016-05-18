#include "DeferredRenderer.h"

#include <gl/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include <VrLib/gl/Vertex.h>
#include <VrLib/gl/FBO.h>
#include <vrlib/Model.h>
#include <vrlib/Texture.h>
#include <VrLib/Log.h>
using vrlib::Log;
using vrlib::logger;


DeferredRenderer::DeferredRenderer()
{
	clearColor = glm::vec4(0.0f, 0.5f, 0.9f, 1.0f);
}

void DeferredRenderer::init()
{
	shader = new vrlib::gl::Shader<Uniforms>("data/DeferredRenderer/shaders/default.vert", "data/DeferredRenderer/shaders/default.frag");
	shader->bindAttributeLocation("a_position", 0);
	shader->bindAttributeLocation("a_normal", 1);
	shader->bindAttributeLocation("a_texcoord", 2);
	shader->link();
	shader->bindFragLocation("fragColor", 0);
	shader->bindFragLocation("fragNormal", 1);
	shader->bindFragLocation("fragPosition", 2);
	//shader->bindFragLocation("fragColor", 0);
	shader->registerUniform(Uniforms::modelMatrix, "modelMatrix");
	shader->registerUniform(Uniforms::projectionMatrix, "projectionMatrix");
	shader->registerUniform(Uniforms::viewMatrix, "viewMatrix");
	shader->registerUniform(Uniforms::s_texture, "s_texture");
	shader->registerUniform(Uniforms::diffuseColor, "diffuseColor");
	shader->registerUniform(Uniforms::textureFactor, "textureFactor");
	shader->use();
	shader->setUniform(Uniforms::s_texture, 0);


	postProcessShader = new vrlib::gl::Shader<PostProcessUniforms>("data/DeferredRenderer/shaders/postprocess.vert", "data/DeferredRenderer/shaders/postprocess.frag");
	postProcessShader->bindAttributeLocation("a_position", 0);
	postProcessShader->link();
	postProcessShader->bindFragLocation("fragColor", 0);
	postProcessShader->registerUniform(PostProcessUniforms::s_color, "s_color");
	postProcessShader->registerUniform(PostProcessUniforms::s_normal, "s_normal");
	postProcessShader->registerUniform(PostProcessUniforms::s_position, "s_position");
	postProcessShader->registerUniform(PostProcessUniforms::lightPosition, "lightPosition");
	postProcessShader->registerUniform(PostProcessUniforms::lightRange, "lightRange");
	postProcessShader->registerUniform(PostProcessUniforms::lightColor, "lightColor");
	postProcessShader->use();
	postProcessShader->setUniform(PostProcessUniforms::s_color, 0);
	postProcessShader->setUniform(PostProcessUniforms::s_normal, 1);
	postProcessShader->setUniform(PostProcessUniforms::s_position, 2);

	model = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2>("data/DeferredRenderer/models/WoodenBox02.obj");
	texture = vrlib::Texture::loadCached("data/DeferredRenderer/textures/grid.png");

	fbo = new vrlib::gl::FBO(2048, 2048, true, vrlib::gl::FBO::Color, vrlib::gl::FBO::Normal, vrlib::gl::FBO::Position);
	logger << "Initialized" << Log::newline;


	lights.push_back(Light(glm::vec3(0, 50, 0), 100, glm::vec4(1,1,1,1)));
	for (int i = 0; i < 3; i++)
	{
		lights.push_back(Light(glm::vec3(5, 2, 5), 3.5 + sin(i), glm::vec4(1, 0, 0, 1)));
		lights.push_back(Light(glm::vec3(5, 2, 5), 3.5 + sin(i), glm::vec4(0, 1, 0, 1)));
		lights.push_back(Light(glm::vec3(5, 2, 5), 3.5 + sin(i), glm::vec4(0, 0, 1, 1)));
		lights.push_back(Light(glm::vec3(5, 2, 5), 3.5 + sin(i), glm::vec4(0, 1, 1, 1)));
		lights.push_back(Light(glm::vec3(5, 2, 5), 3.5 + sin(i), glm::vec4(1, 1, 0, 1)));
		lights.push_back(Light(glm::vec3(5, 2, 5), 3.5 + sin(i), glm::vec4(1, 0, 1, 1)));
	}
}





void DeferredRenderer::draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
{
	glDisable(GL_BLEND);
	shader->use();
	shader->setUniform(Uniforms::projectionMatrix, projectionMatrix);
	shader->setUniform(Uniforms::viewMatrix, modelViewMatrix);
	shader->setUniform(Uniforms::modelMatrix, glm::mat4());
	shader->setUniform(Uniforms::textureFactor, 1.0f);
	shader->setUniform(Uniforms::diffuseColor, glm::vec4(1, 1, 1, 1));

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glDisable(GL_SCISSOR_TEST);

	fbo->bind();
	glViewport(0, 0, fbo->getWidth(), fbo->getHeight());
	glClearColor(0, 0, 0, 1);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);



	for (int x = -10; x < 10; x++)
	{
		for (int y = -10; y < 10; y++)
		{
			model->draw([this, x, y](const glm::mat4 &modelMatrix)
			{
				glm::mat4 matrix;
				matrix = glm::translate(matrix, glm::vec3(x, 0.2f, y));
				matrix = glm::scale(matrix, glm::vec3(0.3f, 0.3f, 0.3f));
				matrix = glm::rotate(matrix, (float)(sin(x/10.0f) + cos(y/4.0f)), glm::vec3(0, 1, 0));

				shader->setUniform(Uniforms::modelMatrix, matrix * modelMatrix);
			},
				[this](const vrlib::Material &material)
			{
				if (material.texture)
				{
					shader->setUniform(Uniforms::textureFactor, 1.0f);
					material.texture->bind();
				}
				else
				{
					shader->setUniform(Uniforms::textureFactor, 0.0f);
					shader->setUniform(Uniforms::diffuseColor, material.color.diffuse);
				}
			});
		}
	}

	{
		shader->setUniform(Uniforms::modelMatrix, glm::mat4());
		std::vector<vrlib::gl::VertexP3N3T2> verts;
		vrlib::gl::VertexP3N3T2 vert;
		vrlib::gl::setN3(vert, glm::vec3(0, 1, 0));
		vrlib::gl::setP3(vert, glm::vec3(-12, 0, -12)); vrlib::gl::setT2(vert, glm::vec2(0, 0)); verts.push_back(vert);
		vrlib::gl::setP3(vert, glm::vec3(-12, 0, 12)); vrlib::gl::setT2(vert, glm::vec2(0, 6)); verts.push_back(vert);
		vrlib::gl::setP3(vert, glm::vec3(12, 0, 12)); vrlib::gl::setT2(vert, glm::vec2(6, 6)); verts.push_back(vert);
		vrlib::gl::setP3(vert, glm::vec3(12, 0, -12)); vrlib::gl::setT2(vert, glm::vec2(6, 0)); verts.push_back(vert);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		texture->bind();
		vrlib::gl::setAttributes<vrlib::gl::VertexP3N3T2>(&verts[0]);
		glDrawArrays(GL_QUADS, 0, 4);
	}


	fbo->unbind();
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	postProcessShader->use();

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glDisable(GL_DEPTH_TEST);
	

	
	{
		std::vector<vrlib::gl::VertexP2> verts;
		vrlib::gl::VertexP2 vert;
		vrlib::gl::setP2(vert, glm::vec2(-1, -1));	verts.push_back(vert);
		vrlib::gl::setP2(vert, glm::vec2(-1, 1));	verts.push_back(vert);
		vrlib::gl::setP2(vert, glm::vec2(1, 1));	verts.push_back(vert);
		vrlib::gl::setP2(vert, glm::vec2(1, -1));	verts.push_back(vert);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		fbo->use();
		vrlib::gl::setAttributes<vrlib::gl::VertexP2>(&verts[0]);

		for (const Light &light : lights)
		{
			postProcessShader->setUniform(PostProcessUniforms::lightPosition, light.position);
			postProcessShader->setUniform(PostProcessUniforms::lightRange, light.range);
			postProcessShader->setUniform(PostProcessUniforms::lightColor, light.color);
			glDrawArrays(GL_QUADS, 0, 4);
		}
	}

	glDisable(GL_BLEND);
}

void DeferredRenderer::preFrame(double frameTime, double totalTime)
{

	for (size_t i = 1; i < lights.size(); i++)
	{
		lights[i].position.x = 5 * cos(totalTime / 3000 + i) + 5 * cos(totalTime/2000) * cos(totalTime / 1000 + 2 * i);
		lights[i].position.z = 5 * sin(totalTime / 3000 + i) + 5 * cos(totalTime / 2000) * sin(totalTime / 1000 + 2*i);
	}

}


