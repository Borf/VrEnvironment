#include "TienTest.h"

#include <gl/glew.h>

#include <VrLib/tien/Node.h>
#include <VrLib/tien/Renderer.h>
#include <VrLib/tien/components/Transform.h>
#include <VrLib/tien/components/TransformAttach.h>
#include <VrLib/tien/components/ModelRenderer.h>
#include <VrLib/tien/components/Camera.h>
#include <VrLib/tien/components/Light.h>
#include <VrLib/Model.h>
#include <VrLib/Util.h>

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
	mHead.init("MainUserHead");
	renderer.init();
	logger << "Initialized" << Log::newline;


	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Main Camera", &renderer);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 5)));
		n->addComponent(new vrlib::tien::components::Camera());
		n->addComponent(new vrlib::tien::components::TransformAttach(mHead));
	}

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Sunlight", &renderer);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(1, 1, 1)));
		vrlib::tien::components::Light* light = new vrlib::tien::components::Light();
		light->color = glm::vec4(1, 1, 0.8627f, 1);
		light->intensity = 20.0f;
		light->type = vrlib::tien::components::Light::Type::directional;
		n->addComponent(light);
	}

	for (int i = 0; i < 20; i++)
	{
		vrlib::tien::Node* n = new vrlib::tien::Node("MovingLight", &renderer);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0.5, 0)));
		if(i == 0)
			n->addComponent(new vrlib::tien::components::TransformAttach(mHead));

		vrlib::tien::components::Light* light = new vrlib::tien::components::Light();
		light->color = glm::vec4(vrlib::util::randomHsv(), 1);
		light->intensity = 20.0f;
		light->range = 1;
		light->type = vrlib::tien::components::Light::Type::point;
		n->addComponent(light);
		movingLights.push_back(n);

		{
			vrlib::tien::Node* nn = new vrlib::tien::Node("box", n);
			nn->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, -0.25, 0), glm::quat(), glm::vec3(0.25f, 0.25f, 0.25f)));
			nn->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/WoodenBox02.obj"));
		}
	}

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Environment", &renderer);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0), glm::quat(glm::vec3(0, glm::radians(90.0f), 0)), glm::vec3(0.008f, 0.008f, 0.008f)));
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/crytek-sponza/sponza.obj"));

//		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, -32, 0), glm::quat(glm::vec3(0, glm::radians(90.0f), 0)), glm::vec3(.5f, .5f, .5f)));
//		n->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/Paris/Paris2010_0.obj"));

//		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0), glm::quat(glm::vec3(0, glm::radians(90.0f), 0)), glm::vec3(.5f, .5f, .5f)));
//		n->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/Damaged Downtown/Downtown_Damage_2.obj"));

		//n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 13, 0), glm::quat(glm::vec3(0, glm::radians(90.0f), 0)), glm::vec3(0.8f, 0.8f, 0.8f)));
		//n->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/cathedral/sibenik2.obj"));
	}


}

void TienTest::draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
{
	renderer.render(projectionMatrix, modelViewMatrix);
}

void TienTest::preFrame(double frameTime, double totalTime)
{
	int i = 0;
	for (auto movingLight : movingLights)
	{
		auto t = movingLight->getComponent<vrlib::tien::components::Transform>();
		t->position.x = 2 * cos(totalTime / 1000.0f + 0.31416 *i);
		t->position.y = 1.5f + 1.0f * cos(totalTime / 1000.0f + 3.1416 *i);
		t->position.z = 2 * sin(totalTime / 1000.0f + 0.31416 *i);
		t->rotation = glm::quat(glm::vec3(0, -(totalTime / 1000.0f + 0.31416 *i), 0));
		i++;
	}

	renderer.update((float)frameTime / 1000.0f);
}


