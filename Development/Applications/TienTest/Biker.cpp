#include "Biker.h"
#include <VrLib/tien/Scene.h>
#include <VrLib/tien/Node.h>
#include <VrLib/tien/components/Transform.h>
#include <VrLib/tien/components/Light.h>
#include <VrLib/tien/components/Camera.h>
#include <VrLib/tien/components/TerrainRenderer.h>
#include <VrLib/tien/components/RigidBody.h>
#include <VrLib/tien/components/TerrainCollider.h>


Biker::Biker()
{
}


Biker::~Biker()
{
}

void Biker::init(vrlib::tien::Scene & scene, TienTest * app)
{
	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Main Camera", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 3, 5)));
		n->addComponent(new vrlib::tien::components::Camera());
	}

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
		auto terrain = new vrlib::tien::Terrain("data/TienTest/biker/Textures/heightmap.png", 50.0f);

		vrlib::tien::Node* n = new vrlib::tien::Node("Terrain", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(-terrain->getWidth()/2.0f, -5, -terrain->getHeight() / 2.0f), glm::quat(), glm::vec3(.75f, .75f, .75f)));
		auto terrainRenderer = new vrlib::tien::components::TerrainRenderer(terrain);
		terrainRenderer->addMaterialLayer(	"data/TienTest/biker/textures/grass_diffuse.png", 
											"data/TienTest/biker/textures/grass_normal.png", 
											"data/TienTest/biker/textures/grass_mask.png");
		terrainRenderer->addMaterialLayer(	"data/TienTest/biker/textures/ground_diffuse.png", 
											"data/TienTest/biker/textures/ground_normal.png", 
											"data/TienTest/biker/textures/ground_mask.png");
		n->addComponent(terrainRenderer);
		n->addComponent(new vrlib::tien::components::RigidBody(0));
		n->addComponent(new vrlib::tien::components::TerrainCollider(n));
	}


}

void Biker::update(float frameTime, vrlib::tien::Scene & scene, TienTest * app)
{
}
