#include "TienTest.h"

#include <GL/glew.h>
#include <fstream>
#include <algorithm>

#include <VrLib/tien/Scene.h>
#include <VrLib/tien/Terrain.h>
#include <VrLib/tien/components/Transform.h>
#include <VrLib/tien/components/TransformAttach.h>
#include <VrLib/tien/components/ModelRenderer.h>
#include <VrLib/tien/components/AnimatedModelRenderer.h>
#include <VrLib/tien/components/BoxCollider.h>
#include <VrLib/tien/components/TerrainCollider.h>
#include <VrLib/tien/components/RigidBody.h>
#include <VrLib/tien/components/Camera.h>
#include <VrLib/tien/components/Light.h>
#include <VrLib/tien/components/TerrainRenderer.h>

#include <VrLib/Model.h>
#include <VrLib/util.h>
#include <VrLib/json.h>
#include <VrLib/math/Ray.h>

#include <VrLib/Log.h>
using vrlib::Log;
using vrlib::logger;


class TowerTemplate
{
public:
	float shootDelay;
	int damage;
	float range;
} towerTemplates[3] = {
	{ 1.0f, 3, 0.2f },
	{ 2.0f, 2, 0.2f },
	{ 1.5f, 1, 0.15f },
};


TienTest::TienTest()
{
	clearColor = glm::vec4(0.0f, 0.5f, 0.9f, 1.0f);
}

void TienTest::init()
{
	vive.init();
	leftButton.init("LeftButton");
	tien.init();

	newTower = nullptr;

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Main Camera", &tien.scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 5)));
		n->addComponent(new vrlib::tien::components::Camera());
		//		n->addComponent(new vrlib::tien::components::TransformAttach(mHead));
	}

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Sunlight", &tien.scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(1, 1, 1)));
		vrlib::tien::components::Light* light = new vrlib::tien::components::Light();
		light->color = glm::vec4(1, 1, 0.8627f, 1);
		light->intensity = 20.0f;
		light->type = vrlib::tien::components::Light::Type::directional;
		light->shadow = vrlib::tien::components::Light::Shadow::shadowmap;
		n->addComponent(light);
		sunLight = n;
	}


	{
		vrlib::tien::Node* n = new vrlib::tien::Node("FloorCollider", &tien.scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, -.5f, 0)));
		n->addComponent(new vrlib::tien::components::BoxCollider(glm::vec3(50, 1, 50)));
		n->addComponent(new vrlib::tien::components::RigidBody(0));
	}

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("LeftHand", &tien.scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0)));
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/vrlib/rendermodels/vr_controller_vive_1_5/vr_controller_vive_1_5.obj"));
		n->addComponent(new vrlib::tien::components::BoxCollider(n));
		n->addComponent(new vrlib::tien::components::RigidBody(0));
		n->addComponent(new vrlib::tien::components::TransformAttach(vive.controllers[1].transform));
		leftHand = n;
	}
	{
		vrlib::tien::Node* n = new vrlib::tien::Node("RightHand", &tien.scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0)));
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/vrlib/rendermodels/vr_controller_vive_1_5/vr_controller_vive_1_5.obj"));
		n->addComponent(new vrlib::tien::components::RigidBody(0));
		n->addComponent(new vrlib::tien::components::BoxCollider(n));
		n->addComponent(new vrlib::tien::components::TransformAttach(vive.controllers[0].transform));
		rightHand = n;
	}


	for (int y = 0; y < 7; y++)
	{
		for (int x = 0; x < 7 - y; x++)
		{
			vrlib::tien::Node* nn = new vrlib::tien::Node("box", &tien.scene);
			nn->addComponent(new vrlib::tien::components::Transform(glm::vec3(.3f * (x + .6 * y) - 1, .25f * y + .15f, -3.25), glm::quat(), glm::vec3(0.25f, 0.25f, 0.25f)));
			nn->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/WoodenBox02.obj"));
			nn->addComponent(new vrlib::tien::components::RigidBody(5));
			nn->addComponent(new vrlib::tien::components::BoxCollider(nn));

		}
	}

#if 0
	for (float x = -3; x <= 3; x += 3)
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

	float dirs[4][2] = { { -1, 0 }, { 1, 0 }, { 0, -1 } , { 0, 1 } };
	for (int ii = 0; ii < 4; ii++)
		for (int i = -3; i <= 3; i += 2)
		{
			vrlib::tien::Node* n = new vrlib::tien::Node("Lamp1", &tien.scene);
			n->addComponent(new vrlib::tien::components::Transform(glm::vec3(dirs[ii][0] == 0 ? i : dirs[ii][0] * 4.5f, 2, dirs[ii][1] == 0 ? i : dirs[ii][1] * 4.5f)));
			vrlib::tien::components::Light* light = new vrlib::tien::components::Light();
			light->color = glm::vec4(vrlib::util::randomHsv(), 1);// glm::vec4(1, 1, 0.9f, 1);
			light->intensity = 20.0f;
			light->range = 1;
			light->type = vrlib::tien::components::Light::Type::point;
			n->addComponent(light);
		}

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Lamp1", &tien.scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 2, 0)));
		vrlib::tien::components::Light* light = new vrlib::tien::components::Light();
		light->color = glm::vec4(1, 1, 0.9f, 1);
		light->intensity = 20.0f;
		light->range = 20;
		light->type = vrlib::tien::components::Light::Type::point;
		light->shadow = vrlib::tien::components::Light::Shadow::shadowmap;
		n->addComponent(light);
	}

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Environment", &tien.scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0), glm::quat(glm::vec3(0, 0, 0)), glm::vec3(1.0f, 1.f, 1.f)));
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/Room/Room.obj"));
	}
	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Table right", &tien.scene);
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/Table/Table.fbx"));
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0.55f, 0, -1), glm::quat(glm::vec3(0, glm::radians(-90.0f), 0)), glm::vec3(0.5f, 0.5f, 0.5f)));
		n->addComponent(new vrlib::tien::components::RigidBody(0));
		n->addComponent(new vrlib::tien::components::BoxCollider(n));
	}
	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Table left", &tien.scene);
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/Table/Table.fbx"));
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(-0.55f, 0, -1), glm::quat(glm::vec3(0, glm::radians(90.0f), 0)), glm::vec3(0.5f, 0.5f, 0.5f)));
		n->addComponent(new vrlib::tien::components::RigidBody(0));
		n->addComponent(new vrlib::tien::components::BoxCollider(n));
	}


	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Terrain", &tien.scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(-.8f, 0.85f, -1.81f), glm::quat(), glm::vec3(0.013f, 0.013f, 0.013f)));
		auto terrainRenderer = new vrlib::tien::components::TerrainRenderer(new vrlib::tien::Terrain("data/TienTest/Textures/heightmap.png"));
		terrainRenderer->addMaterialLayer("data/TienTest/textures/grass_diffuse.png", "data/TienTest/textures/grass_normal.png", "data/TienTest/textures/grass_mask.png");
		terrainRenderer->addMaterialLayer("data/TienTest/textures/ground_diffuse.png", "data/TienTest/textures/ground_normal.png", "data/TienTest/textures/ground_mask.png");
		n->addComponent(terrainRenderer);
		n->addComponent(new vrlib::tien::components::RigidBody(0));
		n->addComponent(new vrlib::tien::components::TerrainCollider(n));
		terrain = n;
	}

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Tower", &tien.scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(.64f, 0.88f, -1.60f), glm::quat(), glm::vec3(0.0005f, 0.0005f, 0.0005f)));
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/Tower/tower.obj"));
	}


	{
		vrlib::tien::Node* n = new vrlib::tien::Node("ArcherTowerButton", &tien.scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(.8f, 0.9f, -.1f), glm::quat(), glm::vec3(0.1f, 0.1f, 0.1f)));
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/buttons/ArrowTower.obj"));
		n->addComponent(new vrlib::tien::components::RigidBody(0));
		n->addComponent(new vrlib::tien::components::BoxCollider(n));
		archerTowerButton = n;
	}

	path.push_back(glm::vec2(-.55f, -1.6f));
	path.push_back(glm::vec2(-.55f, -.4f));
	path.push_back(glm::vec2(0.57f, -.4f));
	path.push_back(glm::vec2(0.57f, -.8f));
	path.push_back(glm::vec2(-.075f, -.8f));
	path.push_back(glm::vec2(-.075f, -1.15f));
	path.push_back(glm::vec2(0.65f, -1.2f));
	path.push_back(glm::vec2(0.65f, -1.7f));

	wave = 1;
	spawnTotal = 10;
	spawnDelay = 2.75f;

	spawnCount = 0;
	nextSpawn = 10;

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("NewArcherTower", &tien.scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(-.4f, 2, -.52f), glm::quat(), glm::vec3(0.05f, 0.05f, 0.05f)));
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/ArcherTower/archertower.obj"));
		n->addComponent(new vrlib::tien::components::RigidBody(1.0f));
		n->addComponent(new vrlib::tien::components::BoxCollider(n));
		n->rigidBody->setLinearFactor(glm::vec3(0,1,0));
		n->rigidBody->setAngularFactor(glm::vec3(0, 0, 0));
		newTower = n;
		newTowerTime = 1;
	}

	{
		vrlib::tien::Node* nn = new vrlib::tien::Node("Dude", &tien.scene);
		nn->addComponent(new vrlib::tien::components::Transform(glm::vec3(-3, 0, 1), glm::quat(glm::vec3(0, glm::radians(90.0f), 0)), glm::vec3(0.0025f, 0.0025f, 0.0025f)));
		nn->addComponent(new vrlib::tien::components::AnimatedModelRenderer("data/TienTest/models/dude/testgastje.fbx"));
		nn->getComponent<vrlib::tien::components::AnimatedModelRenderer>()->playAnimation("Armature|idle", true);
	}


	logger << "Initialized" << Log::newline;

	tien.start();
	logger << "Started" << Log::newline;
	//tien.pause();
}

void TienTest::draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
{
	tien.render(projectionMatrix, modelViewMatrix);
}

void TienTest::preFrame(double frameTime, double totalTime)
{
	//sunLight->getComponent<vrlib::tien::components::Transform>()->position = glm::normalize(glm::vec3(
	//	cos(totalTime / 1000.0f), 1, sin(totalTime / 1000.0f)));


	for (Enemy* e : enemies)
	{
		auto t = e->node->getComponent<vrlib::tien::components::Transform>();
		glm::vec3 target = glm::vec3(path[e->pathIndex + 1].x, 0.85f, path[e->pathIndex + 1].y);
		t->lookAt(target, glm::vec3(0, 1, 0));
		t->rotate(glm::vec3(0, glm::radians(180.0f), 0));
		if (t->moveTo(target, (float)(frameTime / 1000.0f * e->speed)))
		{
			if (e->pathIndex < (int)path.size() - 2)
				e->pathIndex++;
		}
	}


	nextSpawn -= (float)frameTime / 1000.0f;
	if (nextSpawn <= 0)
	{
		if (spawnCount < spawnTotal)
		{
			logger << "Spawning enemy" << Log::newline;
			{
				vrlib::tien::Node* n = new vrlib::tien::Node("Enemy", &tien.scene);
				//n->addComponent(new vrlib::tien::components::Transform(glm::vec3(-.55f, 0.85f, -1.60f), glm::quat(glm::vec3(0, glm::radians(-90.0f), 0)), glm::vec3(0.02f, 0.02f, 0.02f)));
				//n->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/mier.3ds"));
				
				if (wave % 3 == 1)
				{
					n->addComponent(new vrlib::tien::components::Transform(glm::vec3(-.55f, 0.85f, -1.60f), glm::quat(glm::vec3(0, glm::radians(90.0f), 0)), glm::vec3(0.01f, 0.01f, 0.01f)));
					n->addComponent(new vrlib::tien::components::AnimatedModelRenderer("data/TienTest/models/spider/spider3.fbx"));
					n->getComponent<vrlib::tien::components::AnimatedModelRenderer>()->playAnimation("Armature|ArmatureAction", true);
					vrlib::tien::Node* nn = new vrlib::tien::Node("Lampje", n);
					nn->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0.1f)));
					vrlib::tien::components::Light* light = new vrlib::tien::components::Light();
					light->type = vrlib::tien::components::Light::Type::point;
					light->color = glm::vec4(0.5f, 1.0f, 0.5f, 1.0f);
					light->range = 0.25f;
					nn->addComponent(light);
				}
				else if (wave % 3 == 2)
				{
					n->addComponent(new vrlib::tien::components::Transform(glm::vec3(-.55f, 0.85f, -1.60f), glm::quat(glm::vec3(0, glm::radians(90.0f), 0)), glm::vec3(0.0002f, 0.0002f, 0.0002f)));
					n->addComponent(new vrlib::tien::components::AnimatedModelRenderer("data/TienTest/models/Robot/Robots_Prowler.fbx"));
					n->getComponent<vrlib::tien::components::AnimatedModelRenderer>()->playAnimation("Prowler_Rig Bipe", true);
				}
				else if (wave % 3 == 0)
				{
					n->addComponent(new vrlib::tien::components::Transform(glm::vec3(-.55f, 0.85f, -1.60f), glm::quat(glm::vec3(0, glm::radians(-90.0f), 0)), glm::vec3(0.02f, 0.02f, 0.02f)));
					n->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/mier.3ds"));
				}



				n->addComponent(new vrlib::tien::components::RigidBody(0));
				n->addComponent(new vrlib::tien::components::BoxCollider(n));


				Enemy* e = new Enemy();
				e->node = n;
				e->pathIndex = 0;
				e->speed = 0.025f;
				e->health = 5 + wave;
				enemies.push_back(e);
			}

			spawnCount++;
			nextSpawn = spawnDelay;
		}
	}

	for (auto t : towers)
	{
		t->shootTime = glm::max(0.0f, (float)(t->shootTime - frameTime / 1000.0f));
		if (t->shootTime <= 0)
		{
			glm::vec2 tp = t->getPosition();
			int i = 0;
			for (Enemy* e : enemies)
			{
				glm::vec2 ep = e->getPosition();

				if (glm::distance(ep, tp) < towerTemplates[(int)t->type].range)
				{
					vrlib::tien::Node* n = new vrlib::tien::Node("Bullet", &tien.scene);
					n->addComponent(new vrlib::tien::components::Transform(t->node->getComponent<vrlib::tien::components::Transform>()->position + glm::vec3(0, 0.17, 0), glm::quat(glm::vec3(0, 0, 0)), glm::vec3(0.003f, 0.003f, 0.003f)));
					n->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/bullet/bullet.obj"));
					vrlib::tien::components::Light* l = new vrlib::tien::components::Light();
					l->color = glm::vec4(1, 0, 0, 1);
					l->type = vrlib::tien::components::Light::Type::point;
					l->range = 0.2f;
					n->addComponent(l);


					bullets.push_back(Bullet(n, t, e));
					logger << "Shoot!" << Log::newline;
					t->shootTime = towerTemplates[(int)t->type].shootDelay;

					float angle = atan2(tp.y - ep.y, tp.x - ep.x) + glm::radians(90.0f);

					t->node->children.front()->transform->rotation = glm::quat(glm::vec3(0, -angle,0));

					t->node->children.front()->getComponent<vrlib::tien::components::AnimatedModelRenderer>()->playAnimation("Armature|attack", [this, t]()
					{
						t->node->children.front()->getComponent<vrlib::tien::components::AnimatedModelRenderer>()->playAnimation("Armature|idle", true);
					});

					break;
				}
				i++;
			}
		}
	}

	for (int i = 0; i < (int)bullets.size(); i++)
	{
		Bullet &b = bullets[i];
		if (b.dead)
			continue;

		glm::vec3 target = b.target;
		if (b.targetEnemy)
			target = b.targetEnemy->node->getComponent<vrlib::tien::components::Transform>()->position;
		if (b.node->getComponent<vrlib::tien::components::Transform>()->moveTo(target, (float)frameTime / 1000.0f))
		{
			if (b.targetEnemy)
			{
				Enemy* e = b.targetEnemy;
				e->health -= towerTemplates[(int)b.src->type].damage;
				if (e->health <= 0)
				{ // enemy die
					delete e->node;
					enemies.erase(std::find(enemies.begin(), enemies.end(), e));
					for (int ii = 0; ii < (int)bullets.size(); ii++)
						if (bullets[ii].targetEnemy == e)
							bullets[ii].dead = true;
					continue;
				}
			}
			bullets[i].dead = true;
		}
	}

	for (int i = 0; i < (int)bullets.size(); i++)
	{
		if (bullets[i].dead)
		{
			delete bullets[i].node;
			bullets.erase(bullets.begin() + i);
			i--;
		}
	}

	if (enemies.empty() && nextSpawn <= 0)
	{
		wave++;
		spawnTotal = 10 + (int)glm::pow(wave, 0.5);
		spawnDelay = 2.5f * 1.0f / ((wave + 10) / 10.0f);

		spawnCount = 0;
		nextSpawn = 4;
	}



	for (auto t : towers)
	{
		if (tien.scene.testBodyCollision(leftHand, t->node) || tien.scene.testBodyCollision(rightHand, t->node))
		{
			if (vive.controllers[0].triggerButton.getData() != vrlib::DigitalState::OFF)
				t->node->getComponent<vrlib::tien::components::Transform>()->rotate(glm::vec3(0, frameTime / 500.0f, 0));
		}
	}

	bool left;
	if (newTower &&
		((left = tien.scene.testBodyCollision(leftHand, newTower) && vive.controllers[1].triggerButton.getData() == vrlib::DigitalState::ON) ||
		(tien.scene.testBodyCollision(rightHand, newTower) && vive.controllers[0].triggerButton.getData() == vrlib::DigitalState::ON)))
	{
		newTower->getComponent<vrlib::tien::components::Transform>()->position = (left ? leftHand: rightHand)->getComponent<vrlib::tien::components::Transform>()->position;
		newTower->getComponent<vrlib::tien::components::RigidBody>()->body->setMassProps(0, btVector3(0, 0, 0));
		newTower->getComponent<vrlib::tien::components::RigidBody>()->body->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
		newTower->getComponent<vrlib::tien::components::RigidBody>()->body->setGravity(btVector3(0, 0, 0));
	}
	else if (newTower &&
			(vive.controllers[1].triggerButton.getData() == vrlib::DigitalState::TOGGLE_OFF ||
			vive.controllers[0].triggerButton.getData() == vrlib::DigitalState::TOGGLE_OFF))
	{
		btVector3 inertia;
		tien.scene.world->removeRigidBody(newTower->getComponent<vrlib::tien::components::RigidBody>()->body);
		newTower->getComponent<vrlib::tien::components::RigidBody>()->body->getCollisionShape()->calculateLocalInertia(1, inertia);
		newTower->getComponent<vrlib::tien::components::RigidBody>()->body->setMassProps(1, inertia);
		newTower->getComponent<vrlib::tien::components::RigidBody>()->body->setCollisionFlags(0);
		newTower->getComponent<vrlib::tien::components::RigidBody>()->body->setGravity(btVector3(0, -1, 0));
		newTower->getComponent<vrlib::tien::components::RigidBody>()->body->setLinearVelocity(btVector3(0, 0, 0));
		newTower->getComponent<vrlib::tien::components::RigidBody>()->body->setLinearFactor(btVector3(0, 1, 0));
		newTower->getComponent<vrlib::tien::components::RigidBody>()->body->setAngularFactor(btVector3(0, 0, 0));

		newTower->getComponent<vrlib::tien::components::RigidBody>()->body->activate(true);
		tien.scene.world->addRigidBody(newTower->getComponent<vrlib::tien::components::RigidBody>()->body);
	}
	else if (newTower && vive.controllers[1].triggerButton.getData() == vrlib::DigitalState::OFF && vive.controllers[0].triggerButton.getData() == vrlib::DigitalState::OFF)
	{
		glm::vec3 pos = newTower->getComponent<vrlib::tien::components::Transform>()->position;
		if (glm::distance(pos, newTowerPrevPosition) < 0.0001f && 
			newTower->getComponent<vrlib::tien::components::RigidBody>()->body->getLinearVelocity().length() < 0.01f &&
			tien.scene.testBodyCollision(newTower, terrain))
		{
			newTowerTime -= (float)(frameTime / 1000.0f);
			if (newTowerTime < 0)
			{
				Tower* t = new Tower();
				t->node = newTower;
				t->level = 1;
				t->type = Tower::Type::Archer;
				t->shootTime = 0;
				towers.push_back(t);
				newTower->getComponent<vrlib::tien::components::RigidBody>()->body->setMassProps(0, btVector3(0, 0, 0));
				newTower->getComponent<vrlib::tien::components::RigidBody>()->body->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
				newTower->getComponent<vrlib::tien::components::RigidBody>()->body->setGravity(btVector3(0, 0, 0));

				vrlib::tien::Node* nn = new vrlib::tien::Node("ArcherTowerDude", newTower);
				nn->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 2.5f, 0), glm::quat(), glm::vec3(0.001f, 0.001f, 0.001f)));
				nn->addComponent(new vrlib::tien::components::AnimatedModelRenderer("data/TienTest/models/dude/testgastje.fbx"));
				nn->getComponent<vrlib::tien::components::AnimatedModelRenderer>()->playAnimation("Armature|idle", true);


				newTower = nullptr;
			}
		}
		else
			newTowerTime = glm::max(newTowerTime, 2.0f);
		newTowerPrevPosition = pos;
	}


	if (!newTower &&
		(tien.scene.testBodyCollision(leftHand, archerTowerButton) && vive.controllers[1].triggerButton.getData() == vrlib::DigitalState::TOGGLE_ON) ||
		(tien.scene.testBodyCollision(rightHand, archerTowerButton) && vive.controllers[0].triggerButton.getData() == vrlib::DigitalState::TOGGLE_ON))
	{
//		archerTowerButton->getComponent<vrlib::tien::components::Transform>()->rotate(glm::vec3(0, glm::radians(90.0f), 0));
		{
			vrlib::tien::Node* n = new vrlib::tien::Node("ArcherTower", &tien.scene);
			n->addComponent(new vrlib::tien::components::Transform(archerTowerButton->getComponent<vrlib::tien::components::Transform>()->position, glm::quat(), glm::vec3(0.05f, 0.05f, 0.05f)));
			n->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/ArcherTower/archertower.obj"));
			n->addComponent(new vrlib::tien::components::RigidBody(0.0f));
			n->addComponent(new vrlib::tien::components::BoxCollider(n));
			newTower = n;
			newTowerTime = 2;
		}
	}


	tien.update((float)(frameTime / 1000.0f));
}

glm::vec2 GameObject::getPosition()
{
	glm::vec3 pos = node->getComponent<vrlib::tien::components::Transform>()->position;
	return glm::vec2(pos.x, pos.z);
}
