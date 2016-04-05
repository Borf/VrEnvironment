#pragma once

#include <VrLib/Application.h>
#include <VrLib/gl/shader.h>
#include <list>

namespace vrlib { class Texture; }

class RenderWithShader : public vrlib::Application
{
	vrlib::Texture* texture;
	enum class Uniforms
	{
		modelMatrix,
		projectionMatrix,
		viewMatrix,
		s_texture,
		diffuseColor,
		textureFactor
	};

	vrlib::gl::Shader<Uniforms>* shader;

public:
	RenderWithShader();


	virtual void init() override;
	virtual void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix) override;
	virtual void preFrame(double frameTime, double totalTime) override;
};

