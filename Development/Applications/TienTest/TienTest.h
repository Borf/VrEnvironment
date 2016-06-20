#pragma once

#include <VrLib/Application.h>
#include <VrLib/Device.h>
#include <VrLib/tien/Tien.h>
#include <VrLib/tien/Node.h>
#include <list>
#include <vector>


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

	Tower* src;
	Enemy* targetEnemy;
	glm::vec3 target;
};


class TienTest : public vrlib::Application
{
	vrlib::DigitalDevice leftButton;
	vrlib::PositionalDevice mHead;
	vrlib::PositionalDevice  mWand;
	vrlib::PositionalDevice  mWandLeft;

	vrlib::tien::Tien tien;

	std::vector<vrlib::tien::Node*> movingLights;
	vrlib::tien::Node* sunLight;

	std::vector<Enemy*> enemies;
	std::vector<glm::vec2> path;
	std::vector<Tower*> towers;
	std::vector<Bullet> bullets;


	int wave;
	int spawnTotal;
	float spawnDelay;

	int spawnCount;
	float nextSpawn;

public:
	TienTest();


	virtual void init() override;
	virtual void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix) override;
	virtual void preFrame(double frameTime, double totalTime) override;
};

