#include "TienTest.h"

#include <gl/glew.h>

#include <VrLib/tien/Node.h>
#include <VrLib/tien/Renderer.h>
#include <VrLib/tien/components/Transform.h>
#include <VrLib/tien/components/ModelRenderer.h>
#include <VrLib/tien/components/Camera.h>
#include <VrLib/tien/components/Light.h>
#include <VrLib/Model.h>

#include <VrLib/Log.h>
using vrlib::Log;
using vrlib::logger;


TienTest::TienTest()
{
	clearColor = glm::vec4(0.0f, 0.5f, 0.9f, 1.0f);
}

void TienTest::init()
{
	leftButton.init("LeftButton");
	renderer.init();
	logger << "Initialized" << Log::newline;


	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Main Camera", &renderer);
		n->components.push_back(new vrlib::tien::components::Transform(glm::vec3(0, 0, 5)));
		n->components.push_back(new vrlib::tien::components::Camera());
	}

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Sunlight", &renderer);
		n->components.push_back(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0)));
		vrlib::tien::components::Light* light = new vrlib::tien::components::Light();
		light->color = glm::vec4(1, 1, 0.8627f, 1);
		light->intensity = 20.0f;
		light->type = vrlib::tien::components::Light::Type::directional;
		n->components.push_back(light);
	}


	{
		vrlib::tien::Node* n = new vrlib::tien::Node("MovingLight", &renderer);
		n->components.push_back(new vrlib::tien::components::Transform(glm::vec3(0, 1, 0)));
		vrlib::tien::components::Light* light = new vrlib::tien::components::Light();
		light->color = glm::vec4(1, 0, 0, 1);
		light->intensity = 20.0f;
		light->range = 2;
		light->type = vrlib::tien::components::Light::Type::point;
		n->components.push_back(light);
		movingLight = n;
	}

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Environment", &renderer);
		n->components.push_back(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0), glm::quat(glm::vec3(0, glm::radians(90.0f), 0)), glm::vec3(0.008f, 0.008f, 0.008f)));
		n->components.push_back(new vrlib::tien::components::ModelRenderer("data/TienTest/models/crytek-sponza/sponza.obj"));

		//n->components.push_back(new vrlib::tien::components::Transform(glm::vec3(0, 13, 0), glm::quat(glm::vec3(0, glm::radians(90.0f), 0)), glm::vec3(0.8f, 0.8f, 0.8f)));
		//n->components.push_back(new vrlib::tien::components::ModelRenderer("data/TienTest/models/cathedral/sibenik2.obj"));
		
	}


}

void TienTest::draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
{
	renderer.render(projectionMatrix, modelViewMatrix);
}

void TienTest::preFrame(double frameTime, double totalTime)
{
	auto t = movingLight->getComponent<vrlib::tien::components::Transform>();
	t->position.x = 4 * cos(totalTime / 1000.0f);
	t->position.z = 4 * sin(totalTime / 1000.0f);

	renderer.update((float)frameTime / 1000.0f);
}


