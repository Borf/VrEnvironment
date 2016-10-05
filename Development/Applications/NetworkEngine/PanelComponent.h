#pragma once
#include <VrLib/Texture.h>
#include <VrLib/tien/components/MeshRenderer.h>
#include <VrLib/gl/FBO.h>

namespace vrlib
{
	class Texture;
	class TrueTypeFont;
	namespace json { class Value; }
	namespace gl
	{
		class FBO;
	}
}

class PanelComponent : public vrlib::tien::Component, public vrlib::tien::components::MeshRenderer::Mesh
{

	class FboToTexture : public vrlib::Texture
	{
	public:
		vrlib::gl::FBO* fbo;
		FboToTexture(vrlib::gl::FBO* fbo) : fbo(fbo), vrlib::Texture(nullptr)
		{
		}

		inline void bind() override
		{
			fbo->use();
		}

	};

public:
	glm::vec4 clearColor;
	vrlib::gl::FBO* fbo;
	vrlib::gl::FBO* backFbo;

	enum class FontUniform
	{
		projectionMatrix,
		modelMatrix,
		s_texture,
		color,
	};
	static vrlib::gl::Shader<FontUniform>* fontShader;
	static std::map<std::pair<std::string, float>, vrlib::TrueTypeFont*> fonts;




	PanelComponent(const glm::vec2 &size, const glm::ivec2 &res);
	~PanelComponent();

	void clear();
	void swap();
	bool drawText(const glm::vec2 &position, const std::string &font, const std::string &text, const glm::vec4 &color, float size);
	bool drawImage(const std::string &image,const glm::vec2 &position, const glm::vec2 &size);

	virtual vrlib::json::Value toJson(vrlib::json::Value &meshes) const;

};