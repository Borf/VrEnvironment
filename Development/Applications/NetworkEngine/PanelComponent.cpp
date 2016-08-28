#include "PanelComponent.h"


#include <VrLib/gl/FBO.h>
#include <VrLib/json.h>

PanelComponent::PanelComponent(const glm::vec2 &size, const glm::ivec2 &res)
{
	clearColor = glm::vec4(1, 1, 1, 1);
	fbo = new vrlib::gl::FBO(res.x, res.y);
	backFbo = new vrlib::gl::FBO(res.x, res.y);

	material.texture = new FboToTexture(fbo);
	material.normalmap = nullptr;

	clear();
	swap();

	
	vrlib::gl::VertexP3N2B2T2T2 v;
	vrlib::gl::setN3(v, glm::vec3(0, 0, 1));
	vrlib::gl::setTan3(v, glm::vec3(1, 0, 0));
	vrlib::gl::setBiTan3(v, glm::vec3(0, 1, 0));


	vrlib::gl::setP3(v, glm::vec3(-size.x / 2.0f, -size.y / 2.0f, 0));			vrlib::gl::setT2(v, glm::vec2(0, 0));			vertices.push_back(v);
	vrlib::gl::setP3(v, glm::vec3( size.x / 2.0f, -size.y / 2.0f, 0));			vrlib::gl::setT2(v, glm::vec2(1, 0));			vertices.push_back(v);
	vrlib::gl::setP3(v, glm::vec3( size.x / 2.0f,  size.y / 2.0f, 0));			vrlib::gl::setT2(v, glm::vec2(1, 1));			vertices.push_back(v);
	vrlib::gl::setP3(v, glm::vec3(-size.x / 2.0f,  size.y / 2.0f, 0));			vrlib::gl::setT2(v, glm::vec2(0, 1));			vertices.push_back(v);

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(3);
}

PanelComponent::~PanelComponent()
{
	delete fbo;
	delete backFbo;
	delete material.texture;
}


void PanelComponent::clear()
{
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, backFbo->getWidth(), backFbo->getHeight());
	backFbo->bind();
	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT);
	backFbo->unbind();
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}



void PanelComponent::swap()
{
	std::swap(fbo, backFbo);
	((FboToTexture*)material.texture)->fbo = fbo;
}

vrlib::json::Value PanelComponent::toJson() const
{
	vrlib::json::Value ret;
	ret["type"] = "panel";
	return ret;
}