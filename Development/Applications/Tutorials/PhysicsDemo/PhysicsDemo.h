#pragma once

#include <vrlib/Application.h>
#include <vrlib/gl/shader.h>
#include <btBulletDynamicsCommon.h>
#include <vrlib/Device.h>

namespace vrlib { class Texture; class Model; }

class DebugDraw;
class PhysicsDemo :	public vrlib::Application
{
	btBroadphaseInterface*                  broadphase;
	btDefaultCollisionConfiguration*        collisionConfiguration;
	btCollisionDispatcher*                  dispatcher;
	btSequentialImpulseConstraintSolver*    solver;
	btDiscreteDynamicsWorld*                world;
	DebugDraw* debugDraw;

	enum class Uniforms
	{
		modelMatrix,
		projectionMatrix,
		viewMatrix,
		s_texture,
		diffuseColor,
		textureFactor
	};

	vrlib::gl::Shader<Uniforms>* shader;

	vrlib::Texture* gridTexture;
	vrlib::Texture* cubeTexture;
	vrlib::Texture* sphereTexture;
	vrlib::Model* sphereModel;
	vrlib::Model* cubeModel;

	vrlib::DigitalDevice pageUp;
	vrlib::DigitalDevice pageDown;



	std::vector<btRigidBody*> balls;
	std::vector<btRigidBody*> cubes;
	bool showDebug;
public:
	PhysicsDemo();
	~PhysicsDemo();

	virtual void init() override;
	virtual void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix) override;
	virtual void preFrame(double frameTime, double totalTime) override;
	void spawnStuff();

};

