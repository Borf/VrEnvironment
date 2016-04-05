#pragma once

#include <VrLib/Application.h>
#include <VrLib/gl/shader.h>
#include <list>
#include <functional>

namespace vrlib { 
	namespace gl { class FBO; }
	class Model; class Texture; }

class ShadowMap : public vrlib::Application
{
	vrlib::Model* model;
	vrlib::Texture* texture;
	enum class Uniforms
	{
		modelMatrix,
		projectionMatrix,
		viewMatrix,
		shadowMatrix,
		s_texture,
		s_shadowmap,
		diffuseColor,
		textureFactor
	};
	vrlib::gl::Shader<Uniforms>* shader;
	vrlib::gl::FBO* shadowMapFbo;

	enum class ShadowUniforms
	{
		modelMatrix,
		projectionMatrix,
		viewMatrix
	};

	vrlib::gl::Shader<ShadowUniforms>* shadowMapShader;


public:
	ShadowMap();

	virtual void drawScene(int pass, std::function<void(const glm::mat4 &modelMatrix)> modelViewCallback);

	virtual void init() override;
	virtual void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix) override;
	virtual void preFrame(double frameTime, double totalTime) override;
};

