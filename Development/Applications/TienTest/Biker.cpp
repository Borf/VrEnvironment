#include "Biker.h"
#include "TienTest.h"
#include "Serial.h"

#include <VrLib/math/HermiteCurve.h>
#include <VrLib/math/Ray.h>
#include <VrLib/tien/Scene.h>
#include <VrLib/tien/Node.h>
#include <VrLib/tien/components/Transform.h>
#include <VrLib/tien/components/Light.h>
#include <VrLib/tien/components/Camera.h>
#include <VrLib/tien/components/TerrainRenderer.h>
#include <VrLib/tien/components/RigidBody.h>
#include <VrLib/tien/components/TerrainCollider.h>
#include <VrLib/tien/components/ModelRenderer.h>
#include <VrLib/tien/components/AnimatedModelRenderer.h>
#include <VrLib/tien/components/DynamicSkyBox.h>
#include <VrLib/tien/components/TransformAttach.h>

CSerial serial;
std::string buffer;
float speed;

Biker::Biker()
{
}


Biker::~Biker()
{
}

void Biker::init(vrlib::tien::Scene & scene, TienTest * app)
{
	serial.Open(5, 9600);
	Sleep(100);
	serial.SendData("ST\r\n", 4);


	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Sunlight", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(1, 1, 1)));
		vrlib::tien::components::Light* light = new vrlib::tien::components::Light();
		light->color = glm::vec4(1, 1, 0.8627f, 1);
		light->intensity = 20.0f;
		light->type = vrlib::tien::components::Light::Type::directional;
		//light->shadow = vrlib::tien::components::Light::Shadow::shadowmap;
		n->addComponent(light);
	}

	{

		float scale = 1;// .075f;
		auto terrain = new vrlib::tien::Terrain("data/TienTest/biker/Textures/heightmap.png", 50.0f);

		vrlib::tien::Node* n = new vrlib::tien::Node("Terrain", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(-terrain->getWidth() / 2.0f * scale, -15, -terrain->getHeight() / 2.0f* scale), glm::quat(), glm::vec3(scale, scale, scale)));
		auto terrainRenderer = new vrlib::tien::components::TerrainRenderer(terrain);
		terrainRenderer->addMaterialLayer(	"data/TienTest/biker/textures/grass_diffuse.png", 
											"data/TienTest/biker/textures/grass_normal.png", 
											"data/TienTest/biker/textures/grass_mask.png");
		terrainRenderer->addMaterialLayer(	"data/TienTest/biker/textures/ground_diffuse.png", 
											"data/TienTest/biker/textures/ground_normal.png", 
											"data/TienTest/biker/textures/ground_mask.png");
		n->addComponent(terrainRenderer);
		n->addComponent(new vrlib::tien::components::TerrainCollider(n));
		n->addComponent(new vrlib::tien::components::RigidBody(0));
	}

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Bike", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(-0, 0, 0), glm::quat(glm::vec3(0,-glm::half_pi<float>(), 0)), glm::vec3(.03f, .03f, .03f)));
		n->addComponent(new vrlib::tien::components::AnimatedModelRenderer("data/TienTest/biker/models/bike/bike.fbx"));
		n->addComponent(new vrlib::tien::components::RigidBody(0));
		n->getComponent<vrlib::tien::components::AnimatedModelRenderer>()->playAnimation("Armature|Draaien");
		bike = n;
	}
	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Main Camera", bike);
		n->addComponent(new vrlib::tien::components::Transform(-bike->transform->position, glm::inverse(bike->transform->rotation), 1.0f / bike->transform->scale));
		n->addComponent(new vrlib::tien::components::Camera());
		n->addComponent(new vrlib::tien::components::DynamicSkyBox());
	}



	{
		vrlib::tien::Node* n = new vrlib::tien::Node("LeftHand", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0)));
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/vrlib/rendermodels/vr_controller_vive_1_5/vr_controller_vive_1_5.obj"));
		n->addComponent(new vrlib::tien::components::TransformAttach(app->vive.controllers[1].transform));
	}
	{
		vrlib::tien::Node* n = new vrlib::tien::Node("RightHand", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0)));
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/vrlib/rendermodels/vr_controller_vive_1_5/vr_controller_vive_1_5.obj"));
		n->addComponent(new vrlib::tien::components::TransformAttach(app->vive.controllers[0].transform));
	}


	route.addNode(glm::vec2(50, -50),	glm::vec2(0, 200));
	route.addNode(glm::vec2(50, 50),	glm::vec2(-200, 0));
	route.addNode(glm::vec2(-50, 50),	glm::vec2(0, -200));
	route.addNode(glm::vec2(-50, -50),	glm::vec2(200, 0));

	route.finish();


}

void Biker::update(float frameTime, vrlib::tien::Scene & scene, TienTest * app)
{
	time += frameTime;

	if (serial.ReadDataWaiting())
	{
		char buf[1024];
		int rc = serial.ReadData(buf, 1024);
		buffer += std::string(buf, rc);
		while (buffer.find("\n") != std::string::npos)
		{
			std::string line = buffer.substr(0, buffer.find("\n"));
			line = line.substr(line.find("\t") + 1);
			line = line.substr(line.find("\t") + 1);
			line = line.substr(0, line.find("\t"));
			speed = (float)(std::stof(line) / 10.0f);
			buffer = buffer.substr(buffer.find("\n") + 1);
			serial.SendData("ST\r\n", 4);
		}
	}

	if (!serial.IsOpened())
		speed = 50.0f;

	//speed = 50.0f;

	glm::vec2 pos = route.getPosition(position);
	glm::vec2 dir = route.getPosition(position+0.01f) - pos;
	

	scene.castRay(vrlib::math::Ray(glm::vec3(pos.x, 100, pos.y), glm::vec3(0, -1, 0)), 
		[this](vrlib::tien::Node* node, const glm::vec3 &hitPosition, const glm::vec3 &hitNormal)
	{
		bike->transform->position.y = hitPosition.y;
		return true;
	});

	bike->transform->position.x = pos.x;
	bike->transform->position.z = pos.y;

	bike->transform->rotation = glm::quat(glm::vec3(0, glm::pi<float>() - glm::atan(dir.y, dir.x), 0));


	position += speed / 5000.0f;
	bike->getComponent<vrlib::tien::components::AnimatedModelRenderer>()->animationSpeed = speed / 20.0f;

}


void Route::addNode(const glm::vec2 &position, const glm::vec2 &direction)
{
	nodes.push_back(std::make_tuple(position, direction, 0.0f));
}

void Route::finish()
{
	length = 0;
	for (size_t i = 0; i < nodes.size(); i++)
	{
		int i1 = i;
		int i2 = (i + 1) % nodes.size();
		vrlib::math::HermiteCurve hermiteCurve(std::get<0>(nodes[i1]), std::get<1>(nodes[i1]), std::get<0>(nodes[i2]), std::get<1>(nodes[i2]));
		std::get<2>(nodes[i1]) = hermiteCurve.getLength();
		length += std::get<2>(nodes[i1]);
	}

}

glm::vec2 Route::getPosition(float index)
{
	while (index > length)
		index -= length;

	int i = 0;
	float t = 0;
	while (index > t + std::get<2>(nodes[i]))
	{
		t += std::get<2>(nodes[i]);
		i++;
	}
	index -= t;


	int i1 = (int)glm::floor(i);
	int i2 = (i1 + 1) % nodes.size();

	float fract = index / std::get<2>(nodes[i1]);



	vrlib::math::HermiteCurve hermiteCurve(std::get<0>(nodes[i1]), std::get<1>(nodes[i1]), std::get<0>(nodes[i2]), std::get<1>(nodes[i2]));
	return hermiteCurve.getPoint(fract);


}