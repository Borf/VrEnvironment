#include "PanoramaViewer.h"

#include <gl/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include <VrLib/gl/Vertex.h>
#include <vrlib/Model.h>
#include <vrlib/Texture.h>
#include <VrLib/Log.h>
using vrlib::Log;
using vrlib::logger;


PanoramaViewer::PanoramaViewer()
{
	clearColor = glm::vec4(0.0f, 0.5f, 0.9f, 1.0f);
}

void PanoramaViewer::init()
{
	shader = new vrlib::gl::Shader<Uniforms>("data/PanoramaViewer/shaders/default.vert", "data/PanoramaViewer/shaders/default.frag");
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

	model = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2>("data/PanoramaViewer/models/sphere.obj");

	texture = vrlib::Texture::loadCached("data/PanoramaViewer/textures/R0010020.JPG");

	logger << "Initialized" << Log::newline;
}

void PanoramaViewer::draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
{
	shader->use();
	shader->setUniform(Uniforms::projectionMatrix, projectionMatrix);
	shader->setUniform(Uniforms::viewMatrix, modelViewMatrix);
	shader->setUniform(Uniforms::modelMatrix, glm::mat4());
	shader->setUniform(Uniforms::textureFactor, 1.0f);
	shader->setUniform(Uniforms::diffuseColor, glm::vec4(1, 1, 1, 1));

	shader->setUniform(Uniforms::textureFactor, 1.0f);
	texture->bind();

	model->draw([this](const glm::mat4 &modelMatrix)
	{
		glm::mat4 matrix(modelMatrix);
		matrix = glm::scale(matrix, glm::vec3(50, 50, 50));
		shader->setUniform(Uniforms::modelMatrix, matrix);
	},
		[this](const vrlib::Material &material)
	{
	});


}

void PanoramaViewer::preFrame(double frameTime, double totalTime)
{

}


