#include "TowerDefense.h"
#include "TienTest.h"

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
#include <VrLib/math/Ray.h>
#include <VrLib/util.h>

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


TowerDefense::TowerDefense()
{
}


TowerDefense::~TowerDefense()
{
}


void TowerDefense::init(vrlib::tien::Scene& scene, TienTest* app)
{
	newTower = nullptr;

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Main Camera", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 5)));
		n->addComponent(new vrlib::tien::components::Camera());
		//		n->addComponent(new vrlib::tien::components::TransformAttach(mHead));
	}

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Sunlight", &scene);
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
		vrlib::tien::Node* n = new vrlib::tien::Node("FloorCollider", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, -.5f, 0)));
		n->addComponent(new vrlib::tien::components::BoxCollider(glm::vec3(50, 1, 50)));
		n->addComponent(new vrlib::tien::components::RigidBody(0));
	}

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("LeftHand", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0)));
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/vrlib/rendermodels/vr_controller_vive_1_5/vr_controller_vive_1_5.obj"));
		n->addComponent(new vrlib::tien::components::BoxCollider(n));
		n->addComponent(new vrlib::tien::components::RigidBody(0));
		n->addComponent(new vrlib::tien::components::TransformAttach(app->vive.controllers[1].transform));
		leftHand = n;
	}
	{
		vrlib::tien::Node* n = new vrlib::tien::Node("RightHand", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0)));
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/vrlib/rendermodels/vr_controller_vive_1_5/vr_controller_vive_1_5.obj"));
		n->addComponent(new vrlib::tien::components::RigidBody(0));
		n->addComponent(new vrlib::tien::components::BoxCollider(n));
		n->addComponent(new vrlib::tien::components::TransformAttach(app->vive.controllers[0].transform));
		rightHand = n;
	}


	for (int y = 0; y < 7; y++)
	{
		for (int x = 0; x < 7 - y; x++)
		{
			vrlib::tien::Node* nn = new vrlib::tien::Node("box", &scene);
			nn->addComponent(new vrlib::tien::components::Transform(glm::vec3(.3f * (x + .6 * y) - 1, .25f * y + .15f, -3.25), glm::quat(), glm::vec3(0.25f, 0.25f, 0.25f)));
			nn->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/WoodenBox02.obj"));
			nn->addComponent(new vrlib::tien::components::RigidBody(5));
			nn->addComponent(new vrlib::tien::components::BoxCollider(nn));

		}
	}


	float dirs[4][2] = { { -1, 0 },{ 1, 0 },{ 0, -1 } ,{ 0, 1 } };
	for (int ii = 0; ii < 4; ii++)
		for (int i = -3; i <= 3; i += 2)
		{
			vrlib::tien::Node* n = new vrlib::tien::Node("Lamp1", &scene);
			n->addComponent(new vrlib::tien::components::Transform(glm::vec3(dirs[ii][0] == 0 ? i : dirs[ii][0] * 4.5f, 2, dirs[ii][1] == 0 ? i : dirs[ii][1] * 4.5f)));
			vrlib::tien::components::Light* light = new vrlib::tien::components::Light();
			light->color = glm::vec4(vrlib::util::randomHsv(), 1);// glm::vec4(1, 1, 0.9f, 1);
			light->intensity = 20.0f;
			light->range = 1;
			light->type = vrlib::tien::components::Light::Type::point;
			n->addComponent(light);
		}
	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Lamp1", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 3.5, 0)));
		vrlib::tien::components::Light* light = new vrlib::tien::components::Light();
		light->color = glm::vec4(1, 1, 0.9f, 1);
		light->intensity = 20.0f;
		light->range = 10;
		light->type = vrlib::tien::components::Light::Type::point;
		light->shadow = vrlib::tien::components::Light::Shadow::shadowmap;
		n->addComponent(light);
	}

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Environment", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0), glm::quat(glm::vec3(0, 0, 0)), glm::vec3(1.0f, 1.f, 1.f)));
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/Room/Room.obj"));
		n->getComponent<vrlib::tien::components::ModelRenderer>()->castShadow = false;
	}
	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Table right", &scene);
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/Table/Table.fbx"));
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0.55f, 0, -1), glm::quat(glm::vec3(0, glm::radians(-90.0f), 0)), glm::vec3(0.5f, 0.5f, 0.5f)));
		n->addComponent(new vrlib::tien::components::RigidBody(0));
		n->addComponent(new vrlib::tien::components::BoxCollider(n));
	}
	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Table left", &scene);
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/Table/Table.fbx"));
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(-0.55f, 0, -1), glm::quat(glm::vec3(0, glm::radians(90.0f), 0)), glm::vec3(0.5f, 0.5f, 0.5f)));
		n->addComponent(new vrlib::tien::components::RigidBody(0));
		n->addComponent(new vrlib::tien::components::BoxCollider(n));
	}


	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Terrain", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(-.8f, 0.85f, -1.81f), glm::quat(), glm::vec3(0.013f, 0.013f, 0.013f)));
		auto terrainRenderer = new vrlib::tien::components::TerrainRenderer(new vrlib::tien::Terrain("data/TienTest/Textures/heightmap.png", 10.0f));
		terrainRenderer->addMaterialLayer("data/TienTest/textures/grass_diffuse.png", "data/TienTest/textures/grass_normal.png", "data/TienTest/textures/grass_mask.png");
		terrainRenderer->addMaterialLayer("data/TienTest/textures/ground_diffuse.png", "data/TienTest/textures/ground_normal.png", "data/TienTest/textures/ground_mask.png");
		n->addComponent(terrainRenderer);
		n->addComponent(new vrlib::tien::components::RigidBody(0));
		n->addComponent(new vrlib::tien::components::TerrainCollider(n));
		terrain = n;
	}

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Tower", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(.64f, 0.88f, -1.60f), glm::quat(), glm::vec3(0.0005f, 0.0005f, 0.0005f)));
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/Tower/tower.obj"));
	}


	{
		vrlib::tien::Node* n = new vrlib::tien::Node("ArcherTowerButton", &scene);
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
		vrlib::tien::Node* n = new vrlib::tien::Node("NewArcherTower", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(-.4f, 2, -.52f), glm::quat(), glm::vec3(0.05f, 0.05f, 0.05f)));
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/ArcherTower/archertower.obj"));
		n->addComponent(new vrlib::tien::components::RigidBody(1.0f));
		n->addComponent(new vrlib::tien::components::BoxCollider(n));
		n->rigidBody->setLinearFactor(glm::vec3(0, 1, 0));
		n->rigidBody->setAngularFactor(glm::vec3(0, 0, 0));
		newTower = n;
		newTowerTime = 1;
	}

	{
		vrlib::tien::Node* nn = new vrlib::tien::Node("Dude", &scene);
		nn->addComponent(new vrlib::tien::components::Transform(glm::vec3(-3, 0, 1), glm::quat(glm::vec3(0, glm::radians(90.0f), 0)), glm::vec3(0.0025f, 0.0025f, 0.0025f)));
		nn->addComponent(new vrlib::tien::components::AnimatedModelRenderer("data/TienTest/models/dude/testgastje.fbx"));
		nn->getComponent<vrlib::tien::components::AnimatedModelRenderer>()->playAnimation("Armature|idle", true);
	}

	{
		vrlib::tien::Node* nn = new vrlib::tien::Node("Dude2", &scene);
		nn->addComponent(new vrlib::tien::components::Transform(glm::vec3(-3, 0, 2), glm::quat(glm::vec3(0, glm::radians(90.0f), 0)), glm::vec3(0.25f, 0.25f, 0.25f)));
		nn->addComponent(new vrlib::tien::components::AnimatedModelRenderer("data/TienTest/models/spider/spider3.fbx"));
		nn->getComponent<vrlib::tien::components::AnimatedModelRenderer>()->playAnimation("Armature|ArmatureAction", true);
	}

}



void TowerDefense::update(float frameTime, vrlib::tien::Scene& scene, TienTest* app)
{
	//sunLight->getComponent<vrlib::tien::components::Transform>()->position = glm::normalize(glm::vec3(
	//	cos(totalTime / 1000.0f), 1, sin(totalTime / 1000.0f)));

	for (Enemy* e : enemies)
	{
		auto t = e->node->getComponent<vrlib::tien::components::Transform>();
		glm::vec3 target = glm::vec3(path[e->pathIndex + 1].x, 0.85f, path[e->pathIndex + 1].y);
		t->lookAt(target, glm::vec3(0, 1, 0));
		t->rotate(glm::vec3(0, glm::radians(180.0f), 0));
		if (t->moveTo(target, frameTime * e->speed))
		{
			if (e->pathIndex < (int)path.size() - 2)
				e->pathIndex++;
		}
	}


	nextSpawn -= frameTime;
	if (nextSpawn <= 0)
	{
		if (spawnCount < spawnTotal)
		{
			logger << "Spawning enemy" << Log::newline;
			{
				vrlib::tien::Node* n = new vrlib::tien::Node("Enemy", &scene);
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
		t->shootTime = glm::max(0.0f, t->shootTime - frameTime);
		if (t->shootTime <= 0)
		{
			glm::vec2 tp = t->getPosition();
			int i = 0;
			for (Enemy* e : enemies)
			{
				glm::vec2 ep = e->getPosition();

				if (glm::distance(ep, tp) < towerTemplates[(int)t->type].range)
				{
					vrlib::tien::Node* n = new vrlib::tien::Node("Bullet", &scene);
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

					t->node->getFirstChild()->transform->rotation = glm::quat(glm::vec3(0, -angle, 0));

					t->node->getFirstChild()->getComponent<vrlib::tien::components::AnimatedModelRenderer>()->playAnimation("Armature|attack", [this, t]()
					{
						t->node->getFirstChild()->getComponent<vrlib::tien::components::AnimatedModelRenderer>()->playAnimation("Armature|idle", true);
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
		if (b.node->getComponent<vrlib::tien::components::Transform>()->moveTo(target, frameTime))
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
		if (scene.testBodyCollision(leftHand, t->node) || scene.testBodyCollision(rightHand, t->node))
		{
			if (app->vive.controllers[0].triggerButton.getData() != vrlib::DigitalState::OFF)
				t->node->getComponent<vrlib::tien::components::Transform>()->rotate(glm::vec3(0, frameTime * 2, 0));
		}
	}

	bool left;
	if (newTower &&
		((left = scene.testBodyCollision(leftHand, newTower) && app->vive.controllers[1].triggerButton.getData() == vrlib::DigitalState::ON) ||
		(scene.testBodyCollision(rightHand, newTower) && app->vive.controllers[0].triggerButton.getData() == vrlib::DigitalState::ON)))
	{
		newTower->getComponent<vrlib::tien::components::Transform>()->position = (left ? leftHand : rightHand)->getComponent<vrlib::tien::components::Transform>()->position;
		newTower->getComponent<vrlib::tien::components::RigidBody>()->body->setMassProps(0, btVector3(0, 0, 0));
		newTower->getComponent<vrlib::tien::components::RigidBody>()->body->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
		newTower->getComponent<vrlib::tien::components::RigidBody>()->body->setGravity(btVector3(0, 0, 0));
	}
	else if (newTower &&
		(app->vive.controllers[1].triggerButton.getData() == vrlib::DigitalState::TOGGLE_OFF ||
			app->vive.controllers[0].triggerButton.getData() == vrlib::DigitalState::TOGGLE_OFF))
	{
		btVector3 inertia;
		scene.world->removeRigidBody(newTower->getComponent<vrlib::tien::components::RigidBody>()->body);
		newTower->getComponent<vrlib::tien::components::RigidBody>()->body->getCollisionShape()->calculateLocalInertia(1, inertia);
		newTower->getComponent<vrlib::tien::components::RigidBody>()->body->setMassProps(1, inertia);
		newTower->getComponent<vrlib::tien::components::RigidBody>()->body->setCollisionFlags(0);
		newTower->getComponent<vrlib::tien::components::RigidBody>()->body->setGravity(btVector3(0, -1, 0));
		newTower->getComponent<vrlib::tien::components::RigidBody>()->body->setLinearVelocity(btVector3(0, 0, 0));
		newTower->getComponent<vrlib::tien::components::RigidBody>()->body->setLinearFactor(btVector3(0, 1, 0));
		newTower->getComponent<vrlib::tien::components::RigidBody>()->body->setAngularFactor(btVector3(0, 0, 0));

		newTower->getComponent<vrlib::tien::components::RigidBody>()->body->activate(true);
		scene.world->addRigidBody(newTower->getComponent<vrlib::tien::components::RigidBody>()->body);
	}
	else if (newTower && app->vive.controllers[1].triggerButton.getData() == vrlib::DigitalState::OFF && app->vive.controllers[0].triggerButton.getData() == vrlib::DigitalState::OFF)
	{
		glm::vec3 pos = newTower->getComponent<vrlib::tien::components::Transform>()->position;
		if (glm::distance(pos, newTowerPrevPosition) < 0.0001f &&
			newTower->getComponent<vrlib::tien::components::RigidBody>()->body->getLinearVelocity().length() < 0.01f &&
			scene.testBodyCollision(newTower, terrain))
		{
			newTowerTime -= frameTime;
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
		(scene.testBodyCollision(leftHand, archerTowerButton) && app->vive.controllers[1].triggerButton.getData() == vrlib::DigitalState::TOGGLE_ON) ||
		(scene.testBodyCollision(rightHand, archerTowerButton) && app->vive.controllers[0].triggerButton.getData() == vrlib::DigitalState::TOGGLE_ON))
	{
		//		archerTowerButton->getComponent<vrlib::tien::components::Transform>()->rotate(glm::vec3(0, glm::radians(90.0f), 0));
		{
			vrlib::tien::Node* n = new vrlib::tien::Node("ArcherTower", &scene);
			n->addComponent(new vrlib::tien::components::Transform(archerTowerButton->getComponent<vrlib::tien::components::Transform>()->position, glm::quat(), glm::vec3(0.05f, 0.05f, 0.05f)));
			n->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/models/ArcherTower/archertower.obj"));
			n->addComponent(new vrlib::tien::components::RigidBody(0.0f));
			n->addComponent(new vrlib::tien::components::BoxCollider(n));
			newTower = n;
			newTowerTime = 2;
		}
	}
}


glm::vec2 GameObject::getPosition()
{
	glm::vec3 pos = node->getComponent<vrlib::tien::components::Transform>()->position;
	return glm::vec2(pos.x, pos.z);
}
