#include "TienTest.h"

#include <gl/glew.h>
#include <fstream>

#include <VrLib/tien/Scene.h>
#include <VrLib/tien/components/Transform.h>
#include <VrLib/tien/components/TransformAttach.h>
#include <VrLib/tien/components/ModelRenderer.h>
#include <VrLib/tien/components/BoxCollider.h>
#include <VrLib/tien/components/RigidBody.h>
#include <VrLib/tien/components/Camera.h>
#include <VrLib/tien/components/Light.h>
#include <VrLib/Model.h>
#include <VrLib/Util.h>
#include <VrLib/json.h>

#include <VrLib/Log.h>
using vrlib::Log;
using vrlib::logger;


TienTest::TienTest()
{
	clearColor = glm::vec4(0.0f, 0.5f, 0.9f, 1.0f);
}

void TienTest::init()
{
	mWand.init("WandPosition");
	mWandLeft.init("WandPositionLeft");


	leftButton.init("LeftButton");
	mHead.init("MainUserHead");
	tien.init();
	logger << "Initialized" << Log::newline;


	vrlib::tien::Scene scene;
	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Main Camera", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 5)));
		n->addComponent(new vrlib::tien::components::Camera());
		n->addComponent(new vrlib::tien::components::TransformAttach(mHead));
	}

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Sunlight", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(1, 1, 1)));
		vrlib::tien::components::Light* light = new vrlib::tien::components::Light();
		light->color = glm::vec4(1, 1, 0.8627f, 1);
		light->intensity = 20.0f;
		light->type = vrlib::tien::components::Light::Type::directional;
		n->addComponent(light);
		sunLight = n;
	}


	{
		vrlib::tien::Node* n = new vrlib::tien::Node("FloorCollider", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, -.5f, 0)));
		n->addComponent(new vrlib::tien::components::RigidBody(0));
		n->addComponent(new vrlib::tien::components::BoxCollider(glm::vec3(50,1,50)));
	}

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("LeftHand", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0,0,0)));
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/vrlib/rendermodels/vr_controller_vive_1_5/vr_controller_vive_1_5.obj"));
		n->addComponent(new vrlib::tien::components::RigidBody(0.1f));
		n->addComponent(new vrlib::tien::components::BoxCollider(n));
		n->addComponent(new vrlib::tien::components::TransformAttach(mWandLeft));
	}
	{
		vrlib::tien::Node* n = new vrlib::tien::Node("RightHand", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0)));
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/vrlib/rendermodels/vr_controller_vive_1_5/vr_controller_vive_1_5.obj"));
		n->addComponent(new vrlib::tien::components::RigidBody(0.1f));
		n->addComponent(new vrlib::tien::components::BoxCollider(n));
		n->addComponent(new vrlib::tien::components::TransformAttach(mWand));
	}


	for (int y = 0; y < 7; y++)
	{
		for (int x = 0; x < 7-y; x++)
		{
			vrlib::tien::Node* nn = new vrlib::tien::Node("box", &scene);
			nn->addComponent(new vrlib::tien::components::Transform(glm::vec3(.3f * (x + .6 * y)-1, .25f * y+0.15f, -1), glm::quat(), glm::vec3(0.25f, 0.25f, 0.25f)));
			nn->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/WoodenBox02.obj"));
//			nn->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/normaltest/normaltest2.obj"));
			nn->addComponent(new vrlib::tien::components::RigidBody(5));
			nn->addComponent(new vrlib::tien::components::BoxCollider(nn));
			


		/*	vrlib::tien::Node* n = new vrlib::tien::Node("LightUnderBox", nn);
			n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 1)));
			
			vrlib::tien::components::Light* light = new vrlib::tien::components::Light();
			light->color = glm::vec4(vrlib::util::randomHsv(), 1);
			light->intensity = 20.0f;
			light->range = 0.5;
			light->type = vrlib::tien::components::Light::Type::point;
			n->addComponent(light);*/
		}
	}

#if 0
	for (float x = -3; x <= 3; x+=3)
	{
		for (float z = -4; z < 10; z += 3)
		{
			vrlib::tien::Node* n = new vrlib::tien::Node("Lamp1", &renderer);
			n->addComponent(new vrlib::tien::components::Transform(glm::vec3(x, 3, z)));
			vrlib::tien::components::Light* light = new vrlib::tien::components::Light();
			light->color = glm::vec4(1, 1, 0.9f, 1);
			light->intensity = 20.0f;
			light->range = 5;
			light->type = vrlib::tien::components::Light::Type::point;
			n->addComponent(light);
			{
				vrlib::tien::Node* nn = new vrlib::tien::Node("box", n);
				nn->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0), glm::quat(), glm::vec3(0.25f, 0.25f, 0.25f)));
				nn->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/bulb/bulb.obj"));
			}

		}
	}

	vrlib::json::Value v = vrlib::json::readJson(std::ifstream("data/virtueelpd/scenes/Real PD1 v1-6.json"));
	for (const auto &o : v["objects"])
	{
		if (o["model"].asString().find(".fbx") != std::string::npos)
			o["model"] = o["model"].asString().substr(0, o["model"].asString().size() - 4);
		if (o["model"].asString().find("realPD1") != std::string::npos)
			Sleep(0);
		std::string fileName = "data/virtueelpd/models/" + o["category"].asString() + "/" + o["model"].asString() + "/" + o["model"].asString() + ".fbx";
		std::ifstream file(fileName);
		if (file.is_open())
		{
			file.close();
			vrlib::tien::Node* n = new vrlib::tien::Node("Environment", &renderer);
			n->addComponent(new vrlib::tien::components::Transform(
				glm::vec3(o["x"], o["y"], -o["z"].asFloat() + 10),
				glm::quat(o["rotationquat"]["w"], o["rotationquat"]["x"], o["rotationquat"]["y"], o["rotationquat"]["z"]),
				glm::vec3(o["scale"], o["scale"], o["scale"]) * 40.0f));
			n->addComponent(new vrlib::tien::components::ModelRenderer(fileName));
		}
		else
			logger << "Could not find file " << fileName << Log::newline;


	}
#endif


	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Environment", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0), glm::quat(glm::vec3(0, glm::radians(90.0f), 0)), glm::vec3(0.008f, 0.008f, 0.008f)));
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/crytek-sponza/sponza.obj"));

//		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, -32, 0), glm::quat(glm::vec3(0, glm::radians(90.0f), 0)), glm::vec3(.5f, .5f, .5f)));
//		n->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/Paris/Paris2010_0.obj"));

//		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0), glm::quat(glm::vec3(0, glm::radians(90.0f), 0)), glm::vec3(.5f, .5f, .5f)));
//		n->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/Damaged Downtown/Downtown_Damage_2.obj"));

		//n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 13, 0), glm::quat(glm::vec3(0, glm::radians(90.0f), 0)), glm::vec3(0.8f, 0.8f, 0.8f)));
		//n->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/cathedral/sibenik2.obj"));
	}


	tien.currentScene = scene;
	tien.start();
	//tien.pause();

}

void TienTest::draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
{
	tien.render(projectionMatrix, modelViewMatrix);
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

	sunLight->getComponent<vrlib::tien::components::Transform>()->position = glm::normalize(glm::vec3(
		cos(totalTime/1000.0f), 1, sin(totalTime / 1000.0f)));

	tien.update(frameTime / 1000.0f);
}


