#pragma once
#include <VrLib/Texture.h>
#include <VrLib/tien/components/MeshRenderer.h>
#include <VrLib/gl/FBO.h>

namespace vrlib
{
	class Texture;
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

	PanelComponent(const glm::vec2 &size, const glm::ivec2 &res);
	~PanelComponent();

	void clear();
	void swap();

	virtual vrlib::json::Value toJson() const;

};