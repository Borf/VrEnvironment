#include "PhysicsDemo.h"

#include <gl/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include <vrlib/gl/Vertex.h>
#include <vrlib/gl/shader.h>
#include <vrlib/Texture.h>
#include <vrlib/Model.h>

class DebugDraw : public btIDebugDraw
{
	int debugmode;
	std::vector<vrlib::gl::VertexP3N3T2> verts;
public:

	void flush()
	{
		if (!verts.empty())
		{
			glLineWidth(2);
			glColor3f(1.0, 0, 0);
			vrlib::gl::setAttributes<vrlib::gl::VertexP3N3T2>(&verts[0]);
			glDrawArrays(GL_LINES, 0, verts.size());
			verts.clear();
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
		}
	}

	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override
	{
		verts.push_back(vrlib::gl::VertexP3N3T2(glm::vec3(from.x(), from.y(), from.z()), glm::vec3(0, 1, 0), glm::vec2(0, 0)));
		verts.push_back(vrlib::gl::VertexP3N3T2(glm::vec3(to.x(), to.y(), to.z()), glm::vec3(0, 1, 0), glm::vec2(0, 0)));
	}

	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override
	{
	}
	virtual void reportErrorWarning(const char* warningString) override {	}
	virtual void draw3dText(const btVector3& location, const char* textString) override {	}
	virtual void setDebugMode(int debugMode) override { this->debugmode = debugMode; }
	virtual int getDebugMode() const override { return this->debugmode; }

};



PhysicsDemo::PhysicsDemo()
{
}


PhysicsDemo::~PhysicsDemo()
{
}

void PhysicsDemo::init()
{
	shader = new vrlib::gl::Shader<Uniforms>("data/PhysicsDemo/shaders/default.vert", "data/PhysicsDemo/shaders/default.frag");
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

	gridTexture = vrlib::Texture::loadCached("data/PhysicsDemo/textures/grid.png");
	sphereTexture = vrlib::Texture::loadCached("data/PhysicsDemo/textures/sphere.png");
	cubeTexture = vrlib::Texture::loadCached("data/PhysicsDemo/textures/cube.png");
	sphereModel = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2>("sphere.shape");
	cubeModel = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2>("cube.shape");


	pageDown.init("KeyPageDown");
	pageUp.init("KeyPageUp");

//physics
	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver();
	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	world->setGravity(btVector3(0, -9.8f, 0));

	debugDraw = new DebugDraw();
	debugDraw->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	world->setDebugDrawer(debugDraw);



	btBoxShape* groundShape = new btBoxShape(btVector3(32, 1, 32));
	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0, -1, 0));
	float mass = 0;
	btVector3 fallInertia;
	groundShape->calculateLocalInertia(mass, fallInertia);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(groundTransform);
	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, groundMotionState, groundShape, fallInertia);
	btRigidBody* floorBody = new btRigidBody(cInfo);
	world->addRigidBody(floorBody);
	floorBody->setFriction(0.99f);
	floorBody->setRestitution(0.5);


	spawnStuff();
	showDebug = false;
}


void PhysicsDemo::spawnStuff()
{
	for (auto b : balls)
	{
		world->removeRigidBody(b);
		delete b;
	}
	for (auto c : cubes)
	{
		world->removeRigidBody(c);
		delete c;
	}
	balls.clear();
	cubes.clear();


	for (int i = 0; i < 50; i++)
	{
		bool isBall = (rand() % 2 == 0);
		btCollisionShape* ballShape = isBall ? (btCollisionShape*)(new btSphereShape(0.25f)) : (btCollisionShape*)(new btBoxShape(btVector3(0.25f, 0.25f, 0.25f)));
		btTransform ballTransform;
		ballTransform.setIdentity();
		ballTransform.setOrigin(btVector3(sin(i), 5 + i, cos(i)));
		float mass = 1;
		btVector3 fallInertia;
		ballShape->calculateLocalInertia(mass, fallInertia);
		btDefaultMotionState* ballMotionState = new btDefaultMotionState(ballTransform);
		btRigidBody::btRigidBodyConstructionInfo cInfo(mass, ballMotionState, ballShape, fallInertia);
		btRigidBody* ball = new btRigidBody(cInfo);
		world->addRigidBody(ball);
		ball->setFriction(0.99f);
		ball->setRestitution(0.5f);

		if (isBall)
			balls.push_back(ball);
		else
			cubes.push_back(ball);
	}
}


void PhysicsDemo::draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
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
		
		btScalar m[16];
		c->getWorldTransform().getOpenGLMatrix(m);
		glm::mat4 matrix = glm::make_mat4(m);
		matrix = glm::scale(matrix, 0.5f * glm::vec3(0.3333f, 0.3333f, 0.3333f));
		
		cubeModel->draw([this, &matrix](const glm::mat4 &modelMatrix) 
		{
			shader->setUniform(Uniforms::modelMatrix, matrix * modelMatrix);
		}, [this](const vrlib::Material &material)
		{

		});
	}
	sphereTexture->bind();
	for (auto c : balls)
	{

		btScalar m[16];
		c->getWorldTransform().getOpenGLMatrix(m);
		glm::mat4 matrix = glm::make_mat4(m);
		matrix = glm::scale(matrix, glm::vec3(0.25f, 0.25f, 0.25f));

		sphereModel->draw([this, &matrix](const glm::mat4 &modelMatrix)
		{
			shader->setUniform(Uniforms::modelMatrix, matrix * modelMatrix);
		}, [this](const vrlib::Material &material)
		{

		});
	}

	if (showDebug)
	{
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		shader->setUniform(Uniforms::diffuseColor, glm::vec4(1, 1, 1, 1));
		shader->setUniform(Uniforms::textureFactor, 0.0f);
		shader->setUniform(Uniforms::modelMatrix, glm::mat4());
		world->debugDrawWorld();
		debugDraw->flush();
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
	}
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);


}

void PhysicsDemo::preFrame(double frameTime, double totalTime)
{
	world->stepSimulation(frameTime / 1000.0f);

	if (pageUp.getData() == vrlib::DigitalState::TOGGLE_ON)
		spawnStuff();
	if (pageDown.getData() == vrlib::DigitalState::TOGGLE_ON)
		showDebug = !showDebug;

}
