#pragma once

#include <VrLib/gl/VBO.h>
#include <VrLib/gl/VIO.h>
#include <VrLib/tien/components/Renderable.h>

namespace vrlib
{
	class Texture;
	namespace gl	{		class VAO; }
	namespace tien	{		class Terrain;	}
}


class GrassComponent : public vrlib::tien::components::Renderable
{
	class GrassRenderContext : public Renderable::RenderContext, public vrlib::Singleton<GrassRenderContext>
	{
	public:
		enum class RenderUniform
		{
			modelMatrix,
			projectionMatrix,
			viewMatrix,
			modelViewProjectionMatrix,
			normalMatrix,
			s_texture,
			time,
		};
		glm::mat4 viewprojection;
		vrlib::gl::Shader<RenderUniform>* renderShader;
		virtual void init() override;
		virtual void frameSetup(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix) override;
	};


	vrlib::gl::VBO<vrlib::gl::VertexP3>	vbo;
	vrlib::gl::VIO<unsigned int>		vio;
	vrlib::gl::VAO*						vao;
	vrlib::tien::Terrain &terrain;
public:
	float time;

	vrlib::Texture* grassTexture;

	GrassComponent(vrlib::tien::Terrain &terrain);
	virtual void update(float elapsedTime, vrlib::tien::Scene& scene) override;
	virtual void draw() override;
	virtual void drawShadowMap() override;

};