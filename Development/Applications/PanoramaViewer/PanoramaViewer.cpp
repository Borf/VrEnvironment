#include "PanoramaViewer.h"

#include <gl/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include <VrLib/gl/Vertex.h>
#include <VrLib/Model.h>
#include <VrLib/Texture.h>
#include <VrLib/util.h>
#include <VrLib/Log.h>
using vrlib::Log;
using vrlib::logger;


PanoramaViewer::PanoramaViewer()
{
	clearColor = glm::vec4(0.0f, 0.5f, 0.9f, 1.0f);
}

void PanoramaViewer::init()
{
	shader = new vrlib::gl::Shader<Uniforms>("data/PanoramaViewer/shaders/default.vert", "data/PanoramaViewer/shaders/default.frag");
	shader->bindAttributeLocation("a_position", 0);
	shader->bindAttributeLocation("a_normal", 1);
	shader->bindAttributeLocation("a_texcoord", 2);
	shader->link();
	shader->bindFragLocation("fragColor", 0);
	shader->registerUniform(Uniforms::modelMatrix, "modelMatrix");
	shader->registerUniform(Uniforms::projectionMatrix, "projectionMatrix");
	shader->registerUniform(Uniforms::viewMatrix, "viewMatrix");
	shader->registerUniform(Uniforms::s_texture, "s_texture");
	shader->registerUniform(Uniforms::diffuseColor, "diffuseColor");
	shader->registerUniform(Uniforms::textureFactor, "textureFactor");
	shader->registerUniform(Uniforms::useSphereMap, "useSphereMap");
	shader->use();
	shader->setUniform(Uniforms::s_texture, 0);

	vive.init();
	model = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2>("data/PanoramaViewer/models/sphere.obj");

	std::function<void(const std::string &path)> scanDir;
	scanDir = [this, &scanDir](const std::string &path)
	{
		std::vector<std::string> files = vrlib::util::scandir(path);
		for (auto &f : files)
		{
			if (f[0] == '.')
				continue;
			if (f[f.size() - 1] == '/')
			{
				scanDir(path + f);
				return;
			}
			if (f.substr(f.size() - 4) == ".mp4" || f.substr(f.size() - 4) == ".MP4")
				continue;

			vrlib::Texture* t = vrlib::Texture::loadCached(path + f);
			if (t)
				textures.push_back(t);
		}
	};

	scanDir("data/PanoramaViewer/textures/");
	sphereSelection.resize(textures.size(), 0);
	texture = textures[0];

	if (vive.controllers[0].transform.isInitialized())
	{
		viveController = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2>("data/vrlib/rendermodels/vr_controller_vive_1_5/vr_controller_vive_1_5.obj");
	}

	logger << "Initialized" << Log::newline;
}

void PanoramaViewer::draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
{
	shader->use();
	shader->setUniform(Uniforms::projectionMatrix, projectionMatrix);
	shader->setUniform(Uniforms::viewMatrix, modelViewMatrix);
	shader->setUniform(Uniforms::modelMatrix, glm::mat4());
	shader->setUniform(Uniforms::textureFactor, 1.0f);
	shader->setUniform(Uniforms::diffuseColor, glm::vec4(1, 1, 1, 1));
	shader->setUniform(Uniforms::useSphereMap, true);

	shader->setUniform(Uniforms::textureFactor, 1.0f);
	texture->bind();

	model->draw([this](const glm::mat4 &modelMatrix)
	{
		glm::mat4 matrix(modelMatrix);
		matrix = glm::scale(matrix, glm::vec3(75, 75, 75));
		shader->setUniform(Uniforms::modelMatrix, matrix);
	},
		[this](const vrlib::Material &material)
	{
	});


	for (size_t i = 0; i < textures.size(); i++)
	{
		textures[i]->bind();
		model->draw([this,i](const glm::mat4 &modelMatrix)
		{
			float s = .5f + sphereSelection[i] * .75f;
			glm::mat4 matrix(modelMatrix);
			matrix = glm::translate(matrix, getPos(i));
			matrix = glm::scale(matrix, glm::vec3(s, s, s));
			shader->setUniform(Uniforms::modelMatrix, matrix);
		},
			[this](const vrlib::Material &material)
		{
		});
	}


	shader->setUniform(Uniforms::useSphereMap, false);
	viveController->draw([this](const glm::mat4 &modelMatrix)
	{
		glm::mat4 matrix(modelMatrix);
		shader->setUniform(Uniforms::modelMatrix, vive.controllers[0].transform.getData() * matrix);
	},
		[this](const vrlib::Material &material)
	{
		if (material.texture)
		{
			shader->setUniform(Uniforms::textureFactor, 1.0f);
			material.texture->bind();
		}
		else
		{
			shader->setUniform(Uniforms::textureFactor, 0.0f);
			shader->setUniform(Uniforms::diffuseColor, material.color.diffuse);
		}
	});


}

glm::vec3 PanoramaViewer::getPos(int i)
{
	return glm::vec3((float)i/2.0f - textures.size() / 4.0f, 1 + sphereSelection[i], -1);
}

void PanoramaViewer::preFrame(double frameTime, double totalTime)
{
	for (size_t i = 0; i < sphereSelection.size(); i++)
	{
		sphereSelection[i] -= (float)(frameTime / 1000.0f);
		if (sphereSelection[i] < 0)
			sphereSelection[i] = 0;
	}

	for (size_t i = 0; i < sphereSelection.size(); i++)
	{
		glm::vec3 p = getPos(i);
		p.y = 1;
		float dist = glm::distance(p, glm::vec3(vive.controllers[0].transform.getData() * glm::vec4(0, 0, 0, 1)));
		if (dist < 0.5f)
			sphereSelection[i] = 1 - dist * 2;

		if (dist < 0.333f && vive.controllers[0].triggerButton.getData() == vrlib::DigitalState::ON)
			texture = textures[i];
	}
}


