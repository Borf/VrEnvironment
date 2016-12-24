#include "PanoramaViewer.h"

#include <gl/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include <VrLib/gl/Vertex.h>
#include <VrLib/Model.h>
#include <VrLib/Texture.h>
#include <VrLib/Video.h>
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
	shader->registerUniform(Uniforms::offset, "offset");
	shader->registerUniform(Uniforms::flip, "flip");
	shader->use();
	shader->setUniform(Uniforms::s_texture, 0);

	vive.init();
	model = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2>("data/PanoramaViewer/models/sphere.obj");

	std::function<void(const std::string &path)> scanDir;
	scanDir = [this, &scanDir](const std::string &path)
	{
		logger << "Scanning directory " << path << Log::newline;
		std::vector<std::string> files = vrlib::util::scandir(path);
		for (auto &f : files)
		{
			if (f[0] == '.')
				continue;
			if (f[f.size() - 1] == '/')
			{
				scanDir(path + f);
				continue;
			}

			vrlib::Texture* t = vrlib::Texture::loadCached(path + f);
			if (t)
			{
				textures.push_back(t);
				logger << "Loaded " << path + f << Log::newline;
			}
			else
				logger << "error loading " << path + f << Log::newline;
		}
	};

	scanDir("data/PanoramaViewer/textures/");
	sphereSelection.resize(textures.size(), 0);
	texture = textures[0];

	if (vive.controllers[0].transform.isInitialized())
	{
		viveController = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2>("data/vrlib/rendermodels/vr_controller_vive_1_5/vr_controller_vive_1_5.obj");
	}


	//texture = new vrlib::Texture(nullptr);

	logger << "Initialized" << Log::newline;
}

void PanoramaViewer::draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	shader->use();
	shader->setUniform(Uniforms::projectionMatrix, projectionMatrix);
	shader->setUniform(Uniforms::viewMatrix, modelViewMatrix);
	shader->setUniform(Uniforms::modelMatrix, glm::mat4());
	shader->setUniform(Uniforms::textureFactor, 1.0f);
	shader->setUniform(Uniforms::diffuseColor, glm::vec4(1, 1, 1, 1));
	shader->setUniform(Uniforms::useSphereMap, true);
	shader->setUniform(Uniforms::offset, 0.0f);
	shader->setUniform(Uniforms::textureFactor, 1.0f);

	if(texture->image && dynamic_cast<vrlib::Video*>(texture->image))
		shader->setUniform(Uniforms::flip, true);
	else
		shader->setUniform(Uniforms::flip, false);



	texture->bind();

	model->draw([this](const glm::mat4 &modelMatrix)
	{
		glm::mat4 matrix(modelMatrix);
		matrix = glm::scale(matrix, glm::vec3(75, 75, 75));
		shader->setUniform(Uniforms::modelMatrix, matrix);
	});

	if (showMenu)
	{
		for (size_t i = 0; i < textures.size(); i++)
		{
			textures[i]->bind();
			if (textures[i]->image && dynamic_cast<vrlib::Video*>(textures[i]->image))
				shader->setUniform(Uniforms::flip, true);
			else
				shader->setUniform(Uniforms::flip, false);

			shader->setUniform(Uniforms::offset, time * (float)sin(i / 2.135345f));

			model->draw([this, i](const glm::mat4 &modelMatrix)
			{
				float s = .5f + sphereSelection[i] * .5f;
				glm::mat4 matrix(modelMatrix);
				matrix = glm::translate(matrix, getPos(i, true));
				matrix = glm::scale(matrix, glm::vec3(s, s, s));
				shader->setUniform(Uniforms::modelMatrix, matrix);
			});
		}
	}


	glCullFace(GL_BACK);
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
		return true;
	});


}

glm::vec3 PanoramaViewer::getPos(int i, bool selection)
{
	return glm::vec3((1.25f- (selection ? sphereSelection[i] : 0)) * glm::cos( i/(float)15.0f * 2 * 3.1415),
					1 + i / 25.0f, 
					(1.25f - (selection ? sphereSelection[i] : 0)) * glm::sin(i / (float)15.0f * 2 * 3.1415));
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
		glm::vec3 p = getPos(i, false);
		float dist = glm::distance(p, glm::vec3(vive.controllers[0].transform.getData() * glm::vec4(0, 0, 0, 1)));
		if (dist < 0.5f)
			sphereSelection[i] = 1 - dist * 2;

		if (showMenu && glm::distance(getPos(i, true), glm::vec3(vive.controllers[0].transform.getData() * glm::vec4(0, 0, 0, 1))) < 0.25f && 
			(vive.controllers[0].triggerButton.getData() == vrlib::DigitalState::ON || vive.controllers[0].touchButton.getData() == vrlib::DigitalState::ON))
		{
			texture = textures[i];
			showMenu = false;
		}
	}

	if (vive.controllers[0].applicationMenuButton.getData() == vrlib::DigitalState::TOGGLE_ON)
		showMenu = !showMenu;
	texture->update(frameTime / 1000.0f);

	time += frameTime / 10000.0f;

}


