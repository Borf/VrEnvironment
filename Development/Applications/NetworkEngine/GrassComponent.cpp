#include "GrassComponent.h"

#include <VrLib/gl/VAO.h>
#include <VrLib/gl/shader.h>
#include <VrLib/Texture.h>

#include <VrLib/tien/Terrain.h>
#include <VrLib/tien/components/Transform.h>
#include <VrLib/tien/Node.h>


GrassComponent::GrassComponent(vrlib::tien::Terrain &terrain) : terrain(terrain)
{
	renderContext = GrassRenderContext::getInstance();

	grassTexture = vrlib::Texture::loadCached("data/TienTest/biker/textures/texture_grass.jpg");

	std::vector<vrlib::gl::VertexP3> vertices;

	int gridSize = 4;

	for (int x = 0; x < terrain.getWidth() - gridSize; x += gridSize)
	{
		for (int y = 0; y < terrain.getHeight() - gridSize; y += gridSize)
		{
			vrlib::gl::VertexP3 v;
			vrlib::gl::setP3(v, glm::vec3(x, terrain[x][y], y));
			vertices.push_back(v);

			vrlib::gl::setP3(v, glm::vec3(x, terrain[x][y + gridSize], y + gridSize));
			vertices.push_back(v);

			vrlib::gl::setP3(v, glm::vec3(x + gridSize, terrain[x + gridSize][y + gridSize], y + gridSize));
			vertices.push_back(v);

			vrlib::gl::setP3(v, glm::vec3(x + gridSize, terrain[x + gridSize][y], y));
			vertices.push_back(v);
		}
	}


	vbo.bind();
	vbo.setData(vertices.size(), &vertices[0], GL_STATIC_DRAW);
	vao = new vrlib::gl::VAO(&vbo);
}


void GrassComponent::update(float elapsedTime, vrlib::tien::Scene& scene)
{
	time += elapsedTime;
}


void GrassComponent::draw()
{
	glDisable(GL_CULL_FACE);
	vrlib::tien::components::Transform* t = node->getComponent<vrlib::tien::components::Transform>();

	GrassRenderContext* context = dynamic_cast<GrassRenderContext*>(renderContext);
	context->renderShader->use();
	context->renderShader->setUniform(GrassRenderContext::RenderUniform::modelMatrix, t->globalTransform);
	context->renderShader->setUniform(GrassRenderContext::RenderUniform::normalMatrix, glm::transpose(glm::inverse(glm::mat3(t->globalTransform))));
	context->renderShader->setUniform(GrassRenderContext::RenderUniform::modelViewProjectionMatrix, context->viewprojection * t->globalTransform);
	context->renderShader->setUniform(GrassRenderContext::RenderUniform::time, time);
	grassTexture->bind();
	vao->bind();
	glDrawArrays(GL_QUADS, 0, vbo.getLength());
	vao->unBind();

	glEnable(GL_CULL_FACE);

}

void GrassComponent::drawShadowMap()
{
}


void GrassComponent::GrassRenderContext::frameSetup(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix)
{
	renderShader->use();
	renderShader->setUniform(RenderUniform::projectionMatrix, projectionMatrix);
	renderShader->setUniform(RenderUniform::viewMatrix, viewMatrix);
	viewprojection = projectionMatrix * viewMatrix;
}


void GrassComponent::GrassRenderContext::init()
{
	renderShader = new vrlib::gl::Shader<RenderUniform>("data/TienTest/biker/shaders/grass.vert", "data/TienTest/biker/shaders/grass.frag", "data/TienTest/biker/shaders/grass.geom");
	renderShader->bindAttributeLocation("a_position", 0);
	renderShader->bindAttributeLocation("a_normal", 1);
	renderShader->bindAttributeLocation("a_bitangent", 2);
	renderShader->bindAttributeLocation("a_tangent", 3);
	renderShader->bindAttributeLocation("a_texture", 4);
	renderShader->link();
	renderShader->bindFragLocation("fragColor", 0);
	renderShader->bindFragLocation("fragNormal", 1);
	//shader->bindFragLocation("fragColor", 0);
	renderShader->registerUniform(RenderUniform::modelMatrix, "modelMatrix");
	renderShader->registerUniform(RenderUniform::projectionMatrix, "projectionMatrix");
	renderShader->registerUniform(RenderUniform::viewMatrix, "viewMatrix");
	renderShader->registerUniform(RenderUniform::normalMatrix, "normalMatrix");
	renderShader->registerUniform(RenderUniform::s_texture, "s_texture");
	renderShader->registerUniform(RenderUniform::modelViewProjectionMatrix, "modelViewProjectionMatrix");
	renderShader->registerUniform(RenderUniform::time, "time");
	renderShader->use();
	renderShader->setUniform(RenderUniform::s_texture, 0);
}