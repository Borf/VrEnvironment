#pragma once

#include <VrLib/Application.h>
#include <VrLib/gl/shader.h>
#include <VrLib/HtcVive.h>

namespace vrlib { class Model; class Texture;  }


class PanoramaViewer : public vrlib::Application
{
	vrlib::Vive vive;

	vrlib::Model* model;
	enum class Uniforms
	{
		modelMatrix,
		projectionMatrix,
		viewMatrix,
		s_texture,
		diffuseColor,
		textureFactor,
		useSphereMap,
		offset
	};
	float time = 0;
	bool showMenu = true;
	vrlib::gl::Shader<Uniforms>* shader;

	vrlib::Texture* texture;

	std::vector<vrlib::Texture*> textures;
	std::vector<float> sphereSelection;

	vrlib::Model* viveController;

	glm::vec3 getPos(int i, bool selection);

public:
	PanoramaViewer();
	~PanoramaViewer();
	virtual void init() override;
	virtual void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix) override;
	virtual void preFrame(double frameTime, double totalTime) override;
};

