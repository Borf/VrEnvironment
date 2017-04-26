#include "PhysXDemo.h"

#include <gl/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include <vrlib/gl/Vertex.h>
#include <vrlib/gl/shader.h>
#include <vrlib/Texture.h>
#include <vrlib/Model.h>

#ifndef _DEBUG
//#pragma comment(lib, "PhysX3GpuCHECKED_x86.lib")
#pragma comment(lib, "PhysX3CHECKED_x86.lib")
#pragma comment(lib, "PhysX3CookingCHECKED_x86.lib")
#pragma comment(lib, "PhysX3CommonCHECKED_x86.lib")
#pragma comment(lib, "PhysX3CharacterKinematicCHECKED_x86.lib")
#pragma comment(lib, "PhysX3VehicleCHECKED.lib")
#pragma comment(lib, "PhysX3ExtensionsCHECKED.lib")
#pragma comment(lib, "PxTaskCHECKED_x86.lib")
#pragma comment(lib, "PxPvdSDKCHECKED_x86.lib")
#pragma comment(lib, "PxFoundationCHECKED_x86.lib")
#pragma comment(lib, "PxPvdSDKCHECKED_x86.lib")
#pragma comment(lib, "PxTaskCHECKED_x86.lib")
#else
//#pragma comment(lib, "PhysX3GpuDEBUG_x86.lib")
#pragma comment(lib, "PhysX3DEBUG_x86.lib")
#pragma comment(lib, "PhysX3CookingDEBUG_x86.lib")
#pragma comment(lib, "PhysX3CommonDEBUG_x86.lib")
#pragma comment(lib, "PhysX3CharacterKinematicDEBUG_x86.lib")
#pragma comment(lib, "PhysX3VehicleDEBUG.lib")
#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib, "PxTaskDEBUG_x86.lib")
#pragma comment(lib, "PxPvdSDKDEBUG_x86.lib")
#pragma comment(lib, "PxFoundationDEBUG_x86.lib")
#pragma comment(lib, "PxPvdSDKDEBUG_x86.lib")
#pragma comment(lib, "PxTaskDEBUG_x86.lib")
#endif

PhysXDemo::PhysXDemo()
{
}


PhysXDemo::~PhysXDemo()
{
}


physx::PxRigidDynamic* PhysXDemo::createDynamic(const physx::PxTransform& t, const physx::PxGeometry& geometry, const physx::PxVec3& velocity)
{
	physx::PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, 10.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);
	return dynamic;
}

void PhysXDemo::createStack(const physx::PxTransform& t, physx::PxU32 size, physx::PxReal halfExtent)
{
	physx::PxShape* shape = gPhysics->createShape(physx::PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);
	for (physx::PxU32 i = 0; i<size; i++)
	{
		for (physx::PxU32 j = 0; j<size - i; j++)
		{
			physx::PxTransform localTm(physx::PxVec3(physx::PxReal(j * 2) - physx::PxReal(size - i), physx::PxReal(i * 2 + 1), 0) * halfExtent);
			physx::PxRigidDynamic* body = gPhysics->createRigidDynamic(t.transform(localTm));
			body->attachShape(*shape);
			physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
			gScene->addActor(*body);
			cubes.push_back(body);
		}
	}
	shape->release();
}


void PhysXDemo::init()
{
	shader = new vrlib::gl::Shader<Uniforms>("data/PhysXDemo/shaders/default.vert", "data/PhysXDemo/shaders/default.frag");
	shader->bindAttributeLocation("a_position", 0);
	shader->bindAttributeLocation("a_normal", 1);
	shader->bindAttributeLocation("a_texcoord", 2);
	shader->link();
	shader->bindFragLocation("fragColor", 0);
	shader->registerUniform(Uniforms::modelMatrix, "modelMatrix");
	shader->registerUniform(Uniforms::projectionMatrix, "projectionMatrix");
	shader->registerUniform(Uniforms::viewMatrix, "viewMatrix");
	shader->registerUniform(Uniforms::s_texture, "s_texture");
	shader->registerUniform(Uniforms::diffuseColor, "diffuseColor");
	shader->registerUniform(Uniforms::textureFactor, "textureFactor");
	shader->use();
	shader->setUniform(Uniforms::s_texture, 0);

	gridTexture = vrlib::Texture::loadCached("data/PhysXDemo/textures/grid.png");
	sphereTexture = vrlib::Texture::loadCached("data/PhysXDemo/textures/sphere.png");
	cubeTexture = vrlib::Texture::loadCached("data/PhysXDemo/textures/cube.png");
	sphereModel = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2>("sphere.shape");
	cubeModel = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2>("cube.shape");


	pageDown.init("KeyPageDown");
	pageUp.init("KeyPageUp");

	showDebug = true;


	gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);
	gPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(), true, gPvd);

	physx::PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	gDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	gScene = gPhysics->createScene(sceneDesc);

	physx::PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	physx::PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, physx::PxPlane(0, 1, 0, 0), *gMaterial);
	gScene->addActor(*groundPlane);

	for (physx::PxU32 i = 0; i<5; i++)
		createStack(physx::PxTransform(physx::PxVec3(0, 0, stackZ -= 10.0f)), 10, .5f);

	//createDynamic(physx::PxTransform(physx::PxVec3(0, 15, 100)), physx::PxSphereGeometry(1), physx::PxVec3(0, -5, -100));

}





void PhysXDemo::draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
{
	shader->use();
	shader->setUniform(Uniforms::projectionMatrix, projectionMatrix);
	shader->setUniform(Uniforms::viewMatrix, glm::translate(modelViewMatrix, glm::vec3(0,0,0)));
	shader->setUniform(Uniforms::modelMatrix, glm::mat4());

	int size = 32;
	int repeat = 16;
	gridTexture->bind();
	shader->use();
	shader->setUniform(Uniforms::textureFactor, 1.0f);
	shader->setUniform(Uniforms::diffuseColor, glm::vec4(1, 1, 1, 1));
	shader->setUniform(Uniforms::modelMatrix, glm::mat4());
	std::vector<vrlib::gl::VertexP3N3T2> verts;
	verts.push_back(vrlib::gl::VertexP3N3T2(glm::vec3(-size, 0, -size), glm::vec3(0, 1, 0), glm::vec2(0, 0)));
	verts.push_back(vrlib::gl::VertexP3N3T2(glm::vec3(size, 0, -size), glm::vec3(0, 1, 0), glm::vec2(repeat, 0)));
	verts.push_back(vrlib::gl::VertexP3N3T2(glm::vec3(size, 0, size), glm::vec3(0, 1, 0), glm::vec2(repeat, repeat)));
	verts.push_back(vrlib::gl::VertexP3N3T2(glm::vec3(-size, 0, size), glm::vec3(0, 1, 0), glm::vec2(0, repeat)));
	vrlib::gl::setAttributes<vrlib::gl::VertexP3N3T2>(&verts[0]);
	glDrawArrays(GL_QUADS, 0, 4);



	cubeTexture->bind();
	shader->setUniform(Uniforms::diffuseColor, glm::vec4(1, 1, 1, 1));
	for (auto c : cubes)
	{
		auto mat = physx::PxMat44(c->getGlobalPose());
		glm::mat4 matrix = glm::make_mat4(mat.front());
		matrix = glm::scale(matrix, glm::vec3(0.3333f, 0.3333f, 0.3333f));

		cubeModel->draw([this, &matrix](const glm::mat4 &modelMatrix)
		{
			shader->setUniform(Uniforms::modelMatrix, matrix * modelMatrix);
		});
	}

}

void PhysXDemo::preFrame(double frameTime, double totalTime)
{
	gScene->simulate(1.0f / 60.0f);
	gScene->fetchResults(true);


	if (pageDown.getData() == vrlib::DigitalState::TOGGLE_ON)
		showDebug = !showDebug;

}
