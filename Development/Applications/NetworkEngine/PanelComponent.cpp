	#include "PanelComponent.h"

#include <VrLib/gl/FBO.h>
#include <VrLib/gl/Vertex.h>
#include <VrLib/json.h>
#include <VrLib/Font.h>
#include <VrLib/gl/Vertex.h>
#include <glm/gtc/matrix_transform.hpp>


vrlib::gl::Shader<PanelComponent::FontUniform>* PanelComponent::fontShader = nullptr;
std::map<std::pair<std::string, float>, vrlib::TrueTypeFont*> PanelComponent::fonts;

PanelComponent::PanelComponent(const glm::vec2 &size, const glm::ivec2 &res)
{
	if (!fontShader)
	{
		fontShader = new vrlib::gl::Shader<FontUniform>("data/NetworkEngine/shaders/font.vert", "data/NetworkEngine/shaders/font.frag");
		fontShader->bindAttributeLocation("a_position", 0);
		fontShader->bindAttributeLocation("a_color", 1);
		fontShader->link();
		fontShader->registerUniform(FontUniform::projectionMatrix, "projectionMatrix");
		fontShader->registerUniform(FontUniform::modelMatrix, "modelMatrix");
		fontShader->registerUniform(FontUniform::s_texture, "s_texture");
		fontShader->registerUniform(FontUniform::color, "color");
		fontShader->use();
		fontShader->setUniform(FontUniform::s_texture, 0);
		fontShader->setUniform(FontUniform::color, glm::vec4(1, 1, 1, 1));
	}


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


bool PanelComponent::drawText(const glm::vec2 &position, const std::string &font, const std::string &text, const glm::vec4 &color, float size)
{
	if (fonts.find(std::pair<std::string, float>(font, size)) == fonts.end())
	{
		vrlib::TrueTypeFont* f = new vrlib::TrueTypeFont(font, size);
		fonts[std::pair<std::string, float>(font, size)] = f;
	}

	vrlib::TrueTypeFont* f = fonts[std::pair<std::string, float>(font, size)];
	if (!f || !f->fileData)
		return false;

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, backFbo->getWidth(), backFbo->getHeight());
	backFbo->bind();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	fontShader->use();
	fontShader->setUniform(FontUniform::modelMatrix, glm::translate(glm::mat4(), glm::vec3(position,0)));
	fontShader->setUniform(FontUniform::projectionMatrix, glm::ortho(0.0f, (float)backFbo->getWidth(), (float)backFbo->getHeight(), 0.0f));
	fontShader->setUniform(FontUniform::color, color);
	f->drawText<vrlib::gl::VertexP2T2>(text);



	backFbo->unbind();
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	return true;
}

bool PanelComponent::drawImage(const std::string & image, const glm::vec2 & position, const glm::vec2 & size)
{
	vrlib::Texture* t = vrlib::Texture::loadCached(image);
	if (!t)
		return false;

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, backFbo->getWidth(), backFbo->getHeight());
	backFbo->bind();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	fontShader->use();
	fontShader->setUniform(FontUniform::modelMatrix, glm::translate(glm::mat4(), glm::vec3(position, 0)));
	fontShader->setUniform(FontUniform::projectionMatrix, glm::ortho(0.0f, (float)backFbo->getWidth(), (float)backFbo->getHeight(), 0.0f));
	fontShader->setUniform(FontUniform::color, glm::vec4(1,1,1,1));
	
	std::vector<vrlib::gl::VertexP2T2> verts;

	verts.push_back(vrlib::gl::VertexP2T2(glm::vec2(position.x,					position.y),			glm::vec2(0, 0)));
	verts.push_back(vrlib::gl::VertexP2T2(glm::vec2(position.x + size.x,		position.y),			glm::vec2(1, 0)));
	verts.push_back(vrlib::gl::VertexP2T2(glm::vec2(position.x + size.x,		position.y + size.y),	glm::vec2(1, 1)));
	verts.push_back(vrlib::gl::VertexP2T2(glm::vec2(position.x,					position.y + size.y),	glm::vec2(0, 1)));


	t->bind();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	vrlib::gl::setAttributes<vrlib::gl::VertexP2T2>(&verts[0]);

	glDrawArrays(GL_QUADS, 0, verts.size());
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);


	backFbo->unbind();
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	return true;
}


void PanelComponent::swap()
{
	std::swap(fbo, backFbo);
	((FboToTexture*)material.texture)->fbo = fbo;
}

vrlib::json::Value PanelComponent::toJson(vrlib::json::Value &meshes) const
{
	vrlib::json::Value ret;
	ret["type"] = "panel";
	return ret;
}