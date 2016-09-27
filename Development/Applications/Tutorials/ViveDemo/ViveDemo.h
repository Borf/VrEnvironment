#pragma once

#include <VrLib/Application.h>
#include <VrLib/Device.h>
#include <VrLib/gl/shader.h>
#include <list>

namespace vrlib { class Model; }

class ViveDemo : public vrlib::Application
{
	vrlib::Model* model;
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

	vrlib::PositionalDevice  mWand;
	vrlib::PositionalDevice  mWandLeft;
public:
	ViveDemo();


	virtual void init() override;
	virtual void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix) override;
	virtual void preFrame(double frameTime, double totalTime) override;
};

