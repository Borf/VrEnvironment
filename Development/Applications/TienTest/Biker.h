#pragma once

#include "Demo.h"
#include <vector>
#include <glm/glm.hpp>
#include <VrLib/gl/VAO.h>
#include <VrLib/gl/VBO.h>
#include <VrLib/gl/VIO.h>

#include <VrLib/tien/components/Renderable.h>

namespace vrlib {
	class Texture;
	namespace tien { class Terrain;  class Node; }
}

class Route
{
	std::vector<std::tuple<glm::vec2, glm::vec2, float> > nodes;
public:
	void addNode(const glm::vec2 &position, const glm::vec2 &direction);
	void finish();
	
	glm::vec2 getPosition(float index);
	float length = 0;


};

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

	vrlib::json::Value toJson(vrlib::json::Value &meshes) const override;

};



class Biker : public Demo
{
public:
	Biker();
	~Biker();
	
	vrlib::tien::Node* bike;

	Route route;
	vrlib::tien::Terrain* terrain;

	float time = 0;
	float position = 0;

	// Inherited via Demo
	virtual void init(vrlib::tien::Scene & scene, TienTest * app) override;
	virtual void update(float frameTime, vrlib::tien::Scene & scene, TienTest * app) override;
};

