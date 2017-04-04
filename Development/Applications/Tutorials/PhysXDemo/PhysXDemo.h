#pragma once

#include <vrlib/Application.h>
#include <vrlib/gl/shader.h>
#include <vrlib/Device.h>
#include <PxPhysicsAPI.h>

namespace vrlib { class Texture; class Model; }


class PhysXDemo :	public vrlib::Application
{
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

	physx::PxDefaultAllocator		gAllocator;
	physx::PxDefaultErrorCallback	gErrorCallback;

	physx::PxFoundation*			gFoundation = NULL;
	physx::PxPhysics*				gPhysics = NULL;

	physx::PxDefaultCpuDispatcher*	gDispatcher = NULL;
	physx::PxScene*				gScene = NULL;

	physx::PxMaterial*				gMaterial = NULL;

	physx::PxPvd*                  gPvd = NULL;

	physx::PxReal stackZ = 10.0f;

	physx::PxRigidDynamic* createDynamic(const physx::PxTransform& t, const physx::PxGeometry& geometry, const physx::PxVec3& velocity = physx::PxVec3(0));
	void createStack(const physx::PxTransform& t, physx::PxU32 size, physx::PxReal halfExtent);
	bool showDebug;


	std::vector<physx::PxRigidDynamic*> cubes;
public:
	PhysXDemo();
	~PhysXDemo();

	virtual void init() override;
	virtual void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix) override;
	virtual void preFrame(double frameTime, double totalTime) override;
};

