#include "ShadowMap.h"

#include <gl/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include <VrLib/gl/Vertex.h>
#include <VrLib/gl/FBO.h>
#include <vrlib/Model.h>
#include <vrlib/Texture.h>
#include <VrLib/Log.h>
using vrlib::Log;
using vrlib::logger;


ShadowMap::ShadowMap()
{
	clearColor = glm::vec4(0.0f, 0.5f, 0.9f, 1.0f);
}

void ShadowMap::init()
{
	shadowMapShader = new vrlib::gl::Shader<ShadowUniforms>("data/ShadowMap/shaders/shadowmap.vert", "data/ShadowMap/shaders/shadowmap.frag");
	shadowMapShader->bindAttributeLocation("a_position", 0);
	shadowMapShader->link();
	shadowMapShader->registerUniform(ShadowUniforms::modelMatrix, "modelMatrix");
	shadowMapShader->registerUniform(ShadowUniforms::projectionMatrix, "projectionMatrix");
	shadowMapShader->registerUniform(ShadowUniforms::viewMatrix, "viewMatrix");
	shadowMapShader->use();
	
	shader = new vrlib::gl::Shader<Uniforms>("data/ShadowMap/shaders/default.vert", "data/ShadowMap/shaders/default.frag");
	shader->bindAttributeLocation("a_position", 0);
	shader->bindAttributeLocation("a_normal", 1);
	shader->bindAttributeLocation("a_texcoord", 2);
	shader->link();
	shader->bindFragLocation("fragColor", 0);
	shader->registerUniform(Uniforms::modelMatrix,			"modelMatrix");
	shader->registerUniform(Uniforms::projectionMatrix,		"projectionMatrix");
	shader->registerUniform(Uniforms::viewMatrix,			"viewMatrix");
	shader->registerUniform(Uniforms::shadowMatrix,			"shadowMatrix");
	shader->registerUniform(Uniforms::s_texture,			"s_texture");
	shader->registerUniform(Uniforms::diffuseColor,			"diffuseColor");
	shader->registerUniform(Uniforms::textureFactor,		"textureFactor");
	shader->registerUniform(Uniforms::s_shadowmap,			"s_shadowmap");
	shader->use();
	shader->setUniform(Uniforms::s_texture, 0);
	shader->setUniform(Uniforms::s_shadowmap, 1);

	model = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2>("data/ShadowMap/models/mier.3ds");
	texture = vrlib::Texture::loadCached("data/ShadowMap/textures/grid.png");

	shadowMapFbo = new vrlib::gl::FBO(1024 * 8 , 1024 * 8, false, 0, true);

	logger << "Initialized" << Log::newline;
}

void ShadowMap::draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
{
	float fac = 10.0f;
	static float lightDirection = 0;
	lightDirection += 0.001f;

	glm::vec3 lightAngle(cos(lightDirection) * 3, 2, sin(lightDirection) * 3);
	glm::mat4 shadowProjectionMatrix = glm::ortho<float>(-fac, fac, -fac, fac, -5, 50);
	glm::mat4 shadowCameraMatrix = glm::lookAt(lightAngle + glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	shadowMapShader->use();
	shadowMapShader->setUniform(ShadowUniforms::projectionMatrix, shadowProjectionMatrix);
	shadowMapShader->setUniform(ShadowUniforms::viewMatrix, shadowCameraMatrix);
	shadowMapShader->setUniform(ShadowUniforms::modelMatrix, glm::mat4());


	shadowMapFbo->bind();
	glViewport(0, 0, shadowMapFbo->getWidth(), shadowMapFbo->getHeight());
	glClearColor(1, 0, 0, 1);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	drawScene(0, [this](const glm::mat4 &modelMatrix)
	{
		shadowMapShader->setUniform(ShadowUniforms::modelMatrix, modelMatrix);
	});
	shadowMapFbo->unbind();
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	glClearColor(0, 1, 0, 1);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	shader->use();
	shader->setUniform(Uniforms::projectionMatrix, projectionMatrix);
	shader->setUniform(Uniforms::viewMatrix, modelViewMatrix);
	shader->setUniform(Uniforms::modelMatrix, glm::mat4());
	shader->setUniform(Uniforms::shadowMatrix, shadowProjectionMatrix * shadowCameraMatrix);
	shader->setUniform(Uniforms::textureFactor, 1.0f);
	shader->setUniform(Uniforms::diffuseColor, glm::vec4(1, 1, 1, 1));

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, shadowMapFbo->texid[0]);
	glActiveTexture(GL_TEXTURE0);
	
	drawScene(1, [this](const glm::mat4 &modelMatrix)
	{
		shader->setUniform(Uniforms::modelMatrix, modelMatrix);
	});

/*	glUseProgram(0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -10, 10);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	shadowMapFbo->use();
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(-1, -1, 0);
		glTexCoord2f(0, 0); glVertex3f(1,  -1, 0);
		glTexCoord2f(0, 0); glVertex3f(1,   1, 0);
		glTexCoord2f(0, 0); glVertex3f(-1,  1, 0);
	glEnd();
	glEnable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, 0);*/


}

void ShadowMap::preFrame(double frameTime, double totalTime)
{

}

void ShadowMap::drawScene(int pass, std::function<void(const glm::mat4 &modelMatrix)> modelViewCallback)
{
	model->draw([this, &modelViewCallback](const glm::mat4 &modelMatrix)
	{
		glm::mat4 matrix(modelMatrix);
		matrix = glm::translate(matrix, glm::vec3(0, 0, -1.5f));
		matrix = glm::scale(matrix, glm::vec3(0.5f, 0.5f, 0.5f));
		modelViewCallback(matrix);
	},
	[this, &pass](const vrlib::Material &material)
	{
		if (pass == 0)
			return;
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


	modelViewCallback(glm::mat4());

	std::vector<vrlib::gl::VertexP3N3T2> verts;
	vrlib::gl::VertexP3N3T2 vert;
	vrlib::gl::setN3(vert, glm::vec3(0, 1, 0));
	vrlib::gl::setP3(vert, glm::vec3(-4, -1.5f, -4)); vrlib::gl::setT2(vert, glm::vec2(0, 0)); verts.push_back(vert);
	vrlib::gl::setP3(vert, glm::vec3(-4, -1.5f, 4)); vrlib::gl::setT2(vert, glm::vec2(0, 4)); verts.push_back(vert);
	vrlib::gl::setP3(vert, glm::vec3(4, -1.5f, 4)); vrlib::gl::setT2(vert, glm::vec2(4, 4)); verts.push_back(vert);
	vrlib::gl::setP3(vert, glm::vec3(4, -1.5f, -4)); vrlib::gl::setT2(vert, glm::vec2(4, 0)); verts.push_back(vert);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	texture->bind();
	vrlib::gl::setAttributes<vrlib::gl::VertexP3N3T2>(&verts[0]);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

