#pragma once

#include <VrLib/Application.h>
#include <VrLib/gl/shader.h>
#include <list>

namespace vrlib { class Model; class Texture; 
namespace gl { class FBO; }
}

class DeferredRenderer : public vrlib::Application
{
	vrlib::Model* environment;


	vrlib::Model* model;
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


	enum class PostProcessUniforms
	{
		s_color,
		s_normal,
		s_position,
		lightPosition,
		lightRange,
		lightColor,
	};
	vrlib::gl::Shader<PostProcessUniforms>* postProcessShader;


	class Light
	{
	public:
		glm::vec3 position;
		float range;
		glm::vec4 color;
		Light(const glm::vec3 &position, float range, glm::vec4 color = glm::vec4(1, 1, 1, 1))
		{
			this->position = position;
			this->range = range;
			this->color = color;
		}
	};
	std::vector<Light> lights;

	vrlib::gl::FBO* fbo;

public:
	DeferredRenderer();


	virtual void init() override;
	virtual void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix) override;
	virtual void preFrame(double frameTime, double totalTime) override;
};

