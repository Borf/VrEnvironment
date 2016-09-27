#include "WaterComponent.h"

#include <VrLib/gl/VAO.h>
#include <VrLib/gl/shader.h>
#include <VrLib/Texture.h>
#include <VrLib/json.h>
#include <VrLib/gl/Cubemap.h>

#include <VrLib/tien/Terrain.h>
#include <VrLib/tien/components/Transform.h>
#include <VrLib/tien/Node.h>
#include <VrLib/util.h>

WaterComponent::WaterComponent(const glm::vec2 &size, float resolution)
{
	std::vector<glm::vec3> verts;
	verts.push_back(glm::vec3(-0, 0, -0));
	verts.push_back(glm::vec3(1, 0, -0));
	verts.push_back(glm::vec3(1, 0, 1));
	verts.push_back(glm::vec3(-0, 0, 1));


	renderContext = WaterRenderContext::getInstance();
	std::vector<vrlib::gl::VertexP3> vertices;


	vrlib::gl::VertexP3 v;

	for (float x = -size.x/2; x < size.y/2; x += resolution)
		for (float y = -size.y/2; y < size.y/2; y += resolution)
			for (auto vv : verts)
			{
				vrlib::gl::setP3(v, vv * resolution + glm::vec3(x, 0, y));
				vertices.push_back(v);
			}



	vbo.bind();
	vbo.setData(vertices.size(), &vertices[0], GL_STATIC_DRAW);
	vao = new vrlib::gl::VAO(&vbo);

	cubemap = new vrlib::gl::CubeMap();
	cubemap->setTexture(0, "data/NetworkEngine/textures/SkyBoxes/interstellar/interstellar_rt.png");
	cubemap->setTexture(1, "data/NetworkEngine/textures/SkyBoxes/interstellar/interstellar_lf.png");
	cubemap->setTexture(2, "data/NetworkEngine/textures/SkyBoxes/interstellar/interstellar_up.png");
	cubemap->setTexture(3, "data/NetworkEngine/textures/SkyBoxes/interstellar/interstellar_dn.png");
	cubemap->setTexture(4, "data/NetworkEngine/textures/SkyBoxes/interstellar/interstellar_bk.png");
	cubemap->setTexture(5, "data/NetworkEngine/textures/SkyBoxes/interstellar/interstellar_ft.png");

	time = 0;
	deferred = false;
}


void WaterComponent::update(float elapsedTime, vrlib::tien::Scene& scene)
{
	time += elapsedTime;
}


void WaterComponent::draw()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	vrlib::tien::components::Transform* t = node->getComponent<vrlib::tien::components::Transform>();

	WaterRenderContext* context = dynamic_cast<WaterRenderContext*>(renderContext);
	context->renderShader->use();
	context->renderShader->setUniform(WaterRenderContext::RenderUniform::modelMatrix, context->view * t->globalTransform);
	context->renderShader->setUniform(WaterRenderContext::RenderUniform::normalMatrix, glm::transpose(glm::inverse(glm::mat3(context->view * t->globalTransform))));
	context->renderShader->setUniform(WaterRenderContext::RenderUniform::time, time);
	context->renderShader->setUniform(WaterRenderContext::RenderUniform::eyePos, glm::vec3(glm::inverse(context->view * t->globalTransform) * glm::vec4(0,0,0,1)));
	

	if (!setup)
	{
		setup = true;
		context->renderShader->setUniform(WaterRenderContext::RenderUniform::numWaves, 6);
		context->renderShader->setUniform(WaterRenderContext::RenderUniform::envMap, 0);
		for (int i = 0; i < 4; ++i) {
			float amplitude = 0.15f / (i + 1);
			context->renderShader->setUniform(WaterRenderContext::RenderUniform::amplitude, i, amplitude);

			float wavelength = 8 * glm::pi<float>() / (i + 1);
			context->renderShader->setUniform(WaterRenderContext::RenderUniform::wavelength, i, wavelength);

			float speed = 1.0f + 2 * i;
			context->renderShader->setUniform(WaterRenderContext::RenderUniform::speed, i, speed);

			float angle = vrlib::util::random(-glm::pi<float>() / 3, glm::pi<float>() / 3);
			context->renderShader->setUniform(WaterRenderContext::RenderUniform::direction, i, glm::vec2(cos(angle), sin(angle)));
		}
		context->renderShader->setUniform(WaterRenderContext::RenderUniform::amplitude, 4, 0.001f);
		context->renderShader->setUniform(WaterRenderContext::RenderUniform::wavelength, 4, 0.9f);
		context->renderShader->setUniform(WaterRenderContext::RenderUniform::speed, 4, 0.1f);
		float angle = vrlib::util::random(-glm::pi<float>() / 3, glm::pi<float>() / 3);
		context->renderShader->setUniform(WaterRenderContext::RenderUniform::direction, 4, glm::vec2(cos(angle), sin(angle)));

		context->renderShader->setUniform(WaterRenderContext::RenderUniform::amplitude, 5, 0.001f);
		context->renderShader->setUniform(WaterRenderContext::RenderUniform::wavelength, 5, 0.45f);
		context->renderShader->setUniform(WaterRenderContext::RenderUniform::speed, 5, 0.07f);
		angle = angle + 2;
		context->renderShader->setUniform(WaterRenderContext::RenderUniform::direction, 5, glm::vec2(cos(angle), sin(angle)));


	}



	glActiveTexture(GL_TEXTURE0);
	cubemap->bind();
	vao->bind();
	glDrawArrays(GL_QUADS, 0, vbo.getLength());
	vao->unBind();

	glEnable(GL_CULL_FACE);

}

void WaterComponent::drawShadowMap()
{
}


void WaterComponent::WaterRenderContext::frameSetup(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix)
{
	renderShader->use();
	renderShader->setUniform(RenderUniform::projectionMatrix, projectionMatrix);
	view = viewMatrix;
}


void WaterComponent::WaterRenderContext::init()
{
	renderShader = new vrlib::gl::Shader<RenderUniform>("data/NetworkEngine/shaders/Water.vert", "data/NetworkEngine/shaders/Water.frag");
	renderShader->bindAttributeLocation("a_position", 0);
	renderShader->link();
	renderShader->bindFragLocation("fragColor", 0);
	//shader->bindFragLocation("fragColor", 0);
	renderShader->registerUniform(RenderUniform::modelMatrix, "modelMatrix");
	renderShader->registerUniform(RenderUniform::projectionMatrix, "projectionMatrix");
	renderShader->registerUniform(RenderUniform::normalMatrix, "normalMatrix");
	renderShader->registerUniform(RenderUniform::eyePos, "eyePos");
	renderShader->registerUniform(RenderUniform::time, "time");

	renderShader->registerUniform(RenderUniform::envMap, "envMap");
	renderShader->registerUniform(RenderUniform::numWaves, "numWaves");
	renderShader->registerUniformArray(RenderUniform::amplitude, "amplitude", 8);
	renderShader->registerUniformArray(RenderUniform::wavelength, "wavelength", 8);
	renderShader->registerUniformArray(RenderUniform::speed, "speed", 8);
	renderShader->registerUniformArray(RenderUniform::direction, "direction", 8);

	renderShader->use();
}


vrlib::json::Value WaterComponent::toJson(vrlib::json::Value &meshes) const
{
	vrlib::json::Value ret;
	ret["type"] = "water";

	return ret;
}