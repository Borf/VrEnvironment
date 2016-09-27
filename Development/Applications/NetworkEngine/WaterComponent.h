#pragma once

#include <VrLib/gl/VBO.h>
#include <VrLib/gl/VIO.h>
#include <VrLib/tien/components/Renderable.h>

namespace vrlib
{
	class Texture;
	namespace gl	{		class VAO; class CubeMap;}
	namespace tien	{		class Terrain;	}
	namespace json	{		class Value; }
}


class WaterComponent : public vrlib::tien::components::Renderable
{
	class WaterRenderContext : public Renderable::RenderContext, public vrlib::Singleton<WaterRenderContext>
	{
	public:
		enum class RenderUniform
		{
			modelMatrix,
			projectionMatrix,
			normalMatrix,
			eyePos,
			time,
			envMap,
			numWaves,
			amplitude,
			wavelength,
			speed,
			direction
		};
		glm::mat4 view;
		vrlib::gl::Shader<RenderUniform>* renderShader;
		virtual void init() override;
		virtual void frameSetup(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix) override;
	};

	vrlib::gl::CubeMap* cubemap;

	vrlib::gl::VBO<vrlib::gl::VertexP3>	vbo;
	vrlib::gl::VIO<unsigned int>		vio;
	vrlib::gl::VAO*						vao;
public:
	float time;
	bool setup = false;

	vrlib::Texture* WaterTexture;

	WaterComponent(const glm::vec2 &size, float resolution);
	virtual void update(float elapsedTime, vrlib::tien::Scene& scene) override;
	virtual void draw() override;
	virtual void drawShadowMap() override;

	vrlib::json::Value toJson(vrlib::json::Value &meshes) const;

};