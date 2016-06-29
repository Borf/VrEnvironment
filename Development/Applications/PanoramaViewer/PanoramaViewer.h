#pragma once

#include <VrLib/Application.h>
#include <VrLib/gl/shader.h>

namespace vrlib { class Model; class Texture;  }


class PanoramaViewer : public vrlib::Application
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

	vrlib::Texture* texture;

public:
	PanoramaViewer();
	~PanoramaViewer();
	virtual void init() override;
	virtual void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix) override;
	virtual void preFrame(double frameTime, double totalTime) override;
};

