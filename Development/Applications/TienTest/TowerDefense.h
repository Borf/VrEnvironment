#pragma once

#include "Demo.h"

#include <VrLib/tien/Node.h>
#include <glm/glm.hpp>


class GameObject
{
public:
	glm::vec2 getPosition();
	vrlib::tien::Node* node;
};

class Enemy : public GameObject
{
public:
	int pathIndex;
	float speed;
	int health;
};



class Tower : public GameObject
{
public:
	enum class Type
	{
		Archer = 0,
		Bomb = 1,
		Slow = 2,
		Rapid
	} type;
	int level;
	float shootTime;
};

class Bullet : public GameObject
{
public:
	Bullet(vrlib::tien::Node* node, Tower* src, Enemy* enemy) : src(src), targetEnemy(enemy) { this->node = node; }

	bool dead = false;
	Tower* src;
	Enemy* targetEnemy;
	glm::vec3 target;
};




class TowerDefense : public Demo
{
	std::vector<vrlib::tien::Node*> movingLights;
	vrlib::tien::Node* sunLight;

	std::vector<Enemy*> enemies;
	std::vector<glm::vec2> path;
	std::vector<Tower*> towers;
	std::vector<Bullet> bullets;

	vrlib::tien::Node* leftHand;
	vrlib::tien::Node* rightHand;

	vrlib::tien::Node* terrain;

	vrlib::tien::Node* archerTowerButton;
	glm::vec3 newTowerPrevPosition;
	vrlib::tien::Node* newTower;
	float newTowerTime;



	int wave;
	int spawnTotal;
	float spawnDelay;

	int spawnCount;
	float nextSpawn;

public:
	TowerDefense();
	~TowerDefense();


	void init(vrlib::tien::Scene& scene, TienTest* app) override;
	void update(float frameTime, vrlib::tien::Scene& scene, TienTest* app) override;
};

