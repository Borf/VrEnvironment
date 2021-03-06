#include "AnimatedModel.h"

#include <gl/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include <VrLib/gl/Vertex.h>
#include <vrlib/Model.h>
#include <VrLib/models/AssimpModel.h>
#include <vrlib/Texture.h>
#include <VrLib/Log.h>
using vrlib::Log;
using vrlib::logger;


AnimatedModel::AnimatedModel()
{
	clearColor = glm::vec4(0.0f, 0.5f, 0.9f, 1.0f);
}

void AnimatedModel::init()
{ 
	shader = new vrlib::gl::Shader<Uniforms>("data/AnimatedModel/shaders/default.vert", "data/AnimatedModel/shaders/default.frag");
	shader->bindAttributeLocation("a_position", 0);
	shader->bindAttributeLocation("a_normal", 1);
	shader->bindAttributeLocation("a_texcoord", 2);
	shader->bindAttributeLocation("a_boneIds", 3);
	shader->bindAttributeLocation("a_boneWeights", 4);

	shader->link();
	shader->bindFragLocation("fragColor", 0);
	shader->registerUniform(Uniforms::modelMatrix, "modelMatrix");
	shader->registerUniform(Uniforms::projectionMatrix, "projectionMatrix");
	shader->registerUniform(Uniforms::viewMatrix, "viewMatrix");
	shader->registerUniform(Uniforms::s_texture, "s_texture");
	shader->registerUniform(Uniforms::diffuseColor, "diffuseColor");
	shader->registerUniform(Uniforms::textureFactor, "textureFactor");
	shader->registerUniform(Uniforms::boneMatrices, "boneMatrices");
	shader->use();
	shader->setUniform(Uniforms::s_texture, 0);

	//model = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2B4B4>("data/AnimatedModel/models/testgastje.fbx");
	model = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2B4B4>("data/TienTest/biker/models/bike/bikeAnimTest.fbx");
	modelInstance = model->getInstance();
	//((vrlib::State*)modelInstance)->playAnimation("AnimStack::Armature|walk");
	((vrlib::State*)modelInstance)->playAnimation("Armature|ArmatureAction.001");

	logger << "Initialized" << Log::newline;
}

void AnimatedModel::draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
{
	shader->use();
	shader->setUniform(Uniforms::projectionMatrix, projectionMatrix);
	shader->setUniform(Uniforms::viewMatrix, modelViewMatrix);
	shader->setUniform(Uniforms::modelMatrix, glm::mat4());
	shader->setUniform(Uniforms::textureFactor, 1.0f);
	shader->setUniform(Uniforms::diffuseColor, glm::vec4(1, 1, 1, 1));
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	shader->setUniform(Uniforms::boneMatrices, ((vrlib::State*)modelInstance)->boneMatrices);
	modelInstance->draw([this](const glm::mat4 &modelMatrix)
	{
		shader->setUniform(Uniforms::modelMatrix, glm::mat4());
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
		return true;
	});


}

void AnimatedModel::preFrame(double frameTime, double totalTime)
{
	modelInstance->update(frameTime / 1000.0f);
}


