#include "CaveAlign.h"

#include <gl/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include <VrLib/gl/Vertex.h>
#include <vrlib/Texture.h>
#include <VrLib/Log.h>
using vrlib::Log;
using vrlib::logger;


CaveAlign::CaveAlign()
{
	clearColor = glm::vec4(0.0f, 0.5f, 0.9f, 1.0f);
}

void CaveAlign::init()
{
	wand.init("WandPosition");
	shader = new vrlib::gl::Shader<Uniforms>("data/CaveAlign/shaders/default.vert", "data/CaveAlign/shaders/default.frag");
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
	
	texture = vrlib::Texture::loadCached("data/CaveAlign/textures/grid.png");

	logger << "Initialized" << Log::newline;
}

void CaveAlign::draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
{
	glDisable(GL_CULL_FACE);
	shader->use();
	shader->setUniform(Uniforms::projectionMatrix, projectionMatrix);
	shader->setUniform(Uniforms::viewMatrix, modelViewMatrix);
	shader->setUniform(Uniforms::modelMatrix, glm::mat4());
	shader->setUniform(Uniforms::diffuseColor, glm::vec4(1, 1, 1, 1));
	shader->setUniform(Uniforms::textureFactor, 1.0f);
	texture->bind();
	//dit opbouwen moet natuurlijk niet ieder frame, maar voor 1 quad kan 't wel hier steeds
	std::vector<vrlib::gl::VertexP3N3T2> verts;
	vrlib::gl::VertexP3N3T2 vert;
	vrlib::gl::setN3(vert, glm::vec3(0, 1, 0));
	vrlib::gl::setP3(vert, glm::vec3(-1.5, 0, -1.5)); vrlib::gl::setT2(vert, glm::vec2(0, 0)); verts.push_back(vert);
	vrlib::gl::setP3(vert, glm::vec3(-1.5, 0, 1.5)); vrlib::gl::setT2(vert, glm::vec2(0, 2)); verts.push_back(vert);
	vrlib::gl::setP3(vert, glm::vec3(1.5, 0, 1.5)); vrlib::gl::setT2(vert, glm::vec2(2, 2)); verts.push_back(vert);
	vrlib::gl::setP3(vert, glm::vec3(1.5, 0, -1.5)); vrlib::gl::setT2(vert, glm::vec2(2, 0)); verts.push_back(vert);

	vrlib::gl::setN3(vert, glm::vec3(0, 0, 1));
	vrlib::gl::setP3(vert, glm::vec3(-1.5, 0, -1.5)); vrlib::gl::setT2(vert, glm::vec2(0, 0)); verts.push_back(vert);
	vrlib::gl::setP3(vert, glm::vec3(-1.5, 3, -1.5)); vrlib::gl::setT2(vert, glm::vec2(0, 2)); verts.push_back(vert);
	vrlib::gl::setP3(vert, glm::vec3(1.5, 3, -1.5)); vrlib::gl::setT2(vert, glm::vec2(2, 2)); verts.push_back(vert);
	vrlib::gl::setP3(vert, glm::vec3(1.5, 0, -1.5)); vrlib::gl::setT2(vert, glm::vec2(2, 0)); verts.push_back(vert);

	vrlib::gl::setN3(vert, glm::vec3(1, 0, 0));
	vrlib::gl::setP3(vert, glm::vec3(-1.5, 0, -1.5)); vrlib::gl::setT2(vert, glm::vec2(0, 0)); verts.push_back(vert);
	vrlib::gl::setP3(vert, glm::vec3(-1.5, 3, -1.5)); vrlib::gl::setT2(vert, glm::vec2(0, 2)); verts.push_back(vert);
	vrlib::gl::setP3(vert, glm::vec3(-1.5, 3, 1.5)); vrlib::gl::setT2(vert, glm::vec2(2, 2)); verts.push_back(vert);
	vrlib::gl::setP3(vert, glm::vec3(-1.5, 0, 1.5)); vrlib::gl::setT2(vert, glm::vec2(2, 0)); verts.push_back(vert);

	vrlib::gl::setN3(vert, glm::vec3(-1, 0, 0));
	vrlib::gl::setP3(vert, glm::vec3(1.5, 0, -1.5)); vrlib::gl::setT2(vert, glm::vec2(0, 0)); verts.push_back(vert);
	vrlib::gl::setP3(vert, glm::vec3(1.5, 3, -1.5)); vrlib::gl::setT2(vert, glm::vec2(0, 2)); verts.push_back(vert);
	vrlib::gl::setP3(vert, glm::vec3(1.5, 3, 1.5)); vrlib::gl::setT2(vert, glm::vec2(2, 2)); verts.push_back(vert);
	vrlib::gl::setP3(vert, glm::vec3(1.5, 0, 1.5)); vrlib::gl::setT2(vert, glm::vec2(2, 0)); verts.push_back(vert);


	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	vrlib::gl::setAttributes<vrlib::gl::VertexP3N3T2>(&verts[0]);
	glDrawArrays(GL_QUADS, 0, verts.size());
	
	
	glm::mat4 wandMatrix = wand.getData();


	glm::vec3 wandPosition(wandMatrix * glm::vec4(0, 0, 0, 1));
	glm::vec3 wandForward(wandMatrix * glm::vec4(0, 0, -1, 1));
	shader->setUniform(Uniforms::diffuseColor, glm::vec4(1, 0, 0, 1));
	shader->setUniform(Uniforms::textureFactor, 0.0f);

	verts.clear();
	vrlib::gl::setP3(vert, wandPosition); verts.push_back(vert);
	vrlib::gl::setP3(vert, wandForward); verts.push_back(vert);

	vrlib::gl::setAttributes<vrlib::gl::VertexP3N3T2>(&verts[0]);
	glLineWidth(10);
	glDrawArrays(GL_LINES, 0, verts.size());

	
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);


}

void CaveAlign::preFrame(double frameTime, double totalTime)
{

}


