#include "VirtueelPD.h"

#include <fstream>
#include <algorithm>

#include <VrLib/tien/Scene.h>
#include <VrLib/tien/components/Camera.h>
#include <VrLib/tien/components/Transform.h>
#include <VrLib/tien/components/TransformAttach.h>
#include <VrLib/tien/components/ModelRenderer.h>
#include <VrLib/tien/components/Light.h>
#include <VrLib/json.h>
#include <VrLib/Log.h>
using vrlib::Log;
using vrlib::logger;



VirtueelPD::VirtueelPD()
{
}


VirtueelPD::~VirtueelPD()
{
}

void VirtueelPD::init(vrlib::tien::Scene & scene, TienTest * app)
{
	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Main Camera", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 5)));
		n->addComponent(new vrlib::tien::components::Camera());
		//		n->addComponent(new vrlib::tien::components::TransformAttach(mHead));
	}

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Sunlight", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(1, 1, 0.25f)));
		vrlib::tien::components::Light* light = new vrlib::tien::components::Light();
		light->color = glm::vec4(1, 1, 0.8627f, 1);
		light->intensity = 20.0f;
		light->type = vrlib::tien::components::Light::Type::directional;
		light->shadow = vrlib::tien::components::Light::Shadow::shadowmap;

		n->addComponent(light);
	}


	for (float x = -3; x <= 3; x += 3)
	{
		for (float z = -4; z < 10; z += 3)
		{
			vrlib::tien::Node* n = new vrlib::tien::Node("Lamp1", &scene);
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

			
		std::string fileName = "data/virtueelpd/models/" + o["category"].asString() + "/" + o["model"].asString() + "/" + o["model"].asString() + ".fbx";
		std::ifstream file(fileName);
		if (file.is_open())
		{
			file.close();
			vrlib::tien::Node* n = new vrlib::tien::Node("Environment", &scene);
			n->addComponent(new vrlib::tien::components::Transform(
				glm::vec3(o["x"], o["y"], -o["z"].asFloat() + 10),
				glm::quat(o["rotationquat"]["w"], o["rotationquat"]["x"], o["rotationquat"]["y"], o["rotationquat"]["z"]),
				glm::vec3(o["scale"], o["scale"], o["scale"]) * 40.0f));
			n->addComponent(new vrlib::tien::components::ModelRenderer(fileName));

			if (o["model"].asString().find("realPD1") != std::string::npos)
				n->getComponent<vrlib::tien::components::ModelRenderer>()->castShadow = false;
		}
		else
			logger << "Could not find file " << fileName << Log::newline;
	}

}

void VirtueelPD::update(float frameTime, vrlib::tien::Scene & scene, TienTest * app)
{
}
