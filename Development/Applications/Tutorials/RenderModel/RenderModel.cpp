#include "RenderModel.h"

#include <gl/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include <VrLib/gl/Vertex.h>
#include <vrlib/Model.h>
#include <vrlib/Texture.h>
#include <VrLib/Log.h>
using vrlib::Log;
using vrlib::logger;


RenderModel::RenderModel()
{
	clearColor = glm::vec4(0.0f, 0.5f, 0.9f, 1.0f);
}

void RenderModel::init()
{
	shader = new vrlib::gl::Shader<Uniforms>("data/RenderModel/shaders/default.vert", "data/RenderModel/shaders/default.frag");
	shader->bindAttributeLocation("a_position", 0);
	shader->bindAttributeLocation("a_normal", 1);
	shader->bindAttributeLocation("a_texcoord", 2);
	shader->link();
	shader->bindFragLocation("fragColor", 0);
	shader->registerUniform(Uniforms::modelMatrix, "modelMatrix");
	shader->registerUniform(Uniforms::projectionMatrix, "projectionMatrix");
	shader->registerUniform(Uniforms::viewMatrix, "viewMatrix");
	shader->registerUniform(Uniforms::s_texture, "s_texture");
	shader->registerUniform(Uniforms::diffuseColor, "diffuseColor");
	shader->registerUniform(Uniforms::textureFactor, "textureFactor");
	shader->use();
	shader->setUniform(Uniforms::s_texture, 0);

	model = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2>("data/RenderModel/models/mier.3ds");

	logger << "Initialized" << Log::newline;
}

void RenderModel::draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
{
	shader->use();
	shader->setUniform(Uniforms::projectionMatrix, projectionMatrix);
	shader->setUniform(Uniforms::viewMatrix, modelViewMatrix);
	shader->setUniform(Uniforms::modelMatrix, glm::mat4());
	shader->setUniform(Uniforms::textureFactor, 1.0f);
	shader->setUniform(Uniforms::diffuseColor, glm::vec4(1, 1, 1, 1));

	model->draw([this](const glm::mat4 &modelMatrix)
	{
		glm::mat4 matrix(modelMatrix);
		matrix = glm::translate(matrix, glm::vec3(0, 0, -1.5f));
		matrix = glm::scale(matrix, glm::vec3(0.5f, 0.5f, 0.5f));
		shader->setUniform(Uniforms::modelMatrix, matrix);
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

void RenderModel::preFrame(double frameTime, double totalTime)
{

}


