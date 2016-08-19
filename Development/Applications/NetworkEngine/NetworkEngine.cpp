#include "NetworkEngine.h"
#include "Route.h"

#include <glm/gtc/matrix_transform.hpp>

#include <VrLib/ServerConnection.h>
#include <VrLib/Kernel.h>
#include <VrLib/Log.h>
#include <VrLib/gl/Vertex.h>

using vrlib::logger;

#include <VrLib/Texture.h>
#include <VrLib/math/HermiteCurve.h>
#include <VrLib/math/Ray.h>
#include <VrLib/tien/Scene.h>
#include <VrLib/tien/Node.h>
#include <VrLib/tien/Terrain.h>
#include <VrLib/tien/components/Transform.h>
#include <VrLib/tien/components/Light.h>
#include <VrLib/tien/components/Camera.h>
#include <VrLib/tien/components/TerrainRenderer.h>
#include <VrLib/tien/components/RigidBody.h>
#include <VrLib/tien/components/TerrainCollider.h>
#include <VrLib/tien/components/ModelRenderer.h>
#include <VrLib/tien/components/MeshRenderer.h>
#include <VrLib/tien/components/AnimatedModelRenderer.h>
#include <VrLib/tien/components/DynamicSkyBox.h>
#include <VrLib/tien/components/TransformAttach.h>

NetworkEngine::NetworkEngine()
{
}


NetworkEngine::~NetworkEngine()
{
}

void NetworkEngine::init()
{
	tien.init();
	vrlib::Kernel::getInstance()->serverConnection->onTunnelCreate([this](vrlib::Tunnel* tunnel)
	{
		logger << "Got a tunnel" << Log::newline;
		tunnels.push_back(tunnel);
	});


	vrlib::tien::Node* sunlight;
	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Sunlight", &tien.scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 1, 1)));
		vrlib::tien::components::Light* light = new vrlib::tien::components::Light();
		light->color = glm::vec4(1, 1, 0.8627f, 1);
		light->intensity = 20.0f;
		light->type = vrlib::tien::components::Light::Type::directional;
		light->shadow = vrlib::tien::components::Light::Shadow::shadowmap;
		n->addComponent(light);
		sunlight = n;
	}

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Camera", &tien.scene);
		n->addComponent(new vrlib::tien::components::Transform());
		n->addComponent(new vrlib::tien::components::Camera());
		n->addComponent(new vrlib::tien::components::DynamicSkyBox());
		n->getComponent<vrlib::tien::components::DynamicSkyBox>()->light = sunlight;
		tien.scene.cameraNode = n;
	}


	if(false)
	{
		terrain = new vrlib::tien::Terrain();
		terrain->setSize(32, 32);
		for (int x = 0; x < 32; x++)
			for (int y = 0; y < 32; y++)
				(*terrain)[x][y] = 2+sin(x / 2.0f) + cos(y / 2.0f);// rand() / (float)RAND_MAX;
				//(*terrain)[x][y] = 6 + 3 * sin(y / 3.0f);

		{
			vrlib::tien::Node* n = new vrlib::tien::Node("terrain", &tien.scene);
			n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0)));
			auto renderer = new vrlib::tien::components::TerrainRenderer(terrain);
			n->addComponent(renderer);

			renderer->addMaterialLayer("data/TienTest/biker/textures/grass_diffuse.png", "data/TienTest/biker/textures/grass_normal.png", "data/vrlib/tien/textures/white.png");
		}
	}


	{
		float roundness = 5.0f;
		Route* r = new Route();
		r->addNode(glm::vec3(10, 0.1f, -10), glm::vec3(1, 0, 1) * roundness);
		r->addNode(glm::vec3(10, 0.1f, 10), glm::vec3(-1, 0, 1) * roundness);
		r->addNode(glm::vec3(-10, 0.1f, 10), glm::vec3(-1, 0, -1) * roundness);
		r->addNode(glm::vec3(-10, 0.1f, -10), glm::vec3(1, 0, -1) * roundness);
		r->finish();
		routes.push_back(r);
	}

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Character", &tien.scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0), glm::quat(), glm::vec3(0.0025f, 0.0025f, 0.0025f)));
		n->addComponent(new vrlib::tien::components::AnimatedModelRenderer("data/TienTest/models/dude/testgastje.fbx"));

		n->getComponent<vrlib::tien::components::AnimatedModelRenderer>()->playAnimation("Armature|walk", true);

		RouteFollower f;

		f.node = n;
		f.route = routes[0];
		f.offset = 0;
		f.speed = 3.0f;
		f.rotate = RouteFollower::Rotate::XZ;
		f.rotateOffset = glm::quat(glm::vec3(0, -glm::radians(90.0f), 0));
		routeFollowers.push_back(f);
	}






	{
		vrlib::tien::Node* n = new vrlib::tien::Node("GroundPlane", &tien.scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0)));

		vrlib::tien::components::MeshRenderer::Mesh* mesh = new vrlib::tien::components::MeshRenderer::Mesh();
		mesh->material.texture = vrlib::Texture::loadCached("data/TienTest/textures/grid.png");
		mesh->indices = { 2, 1, 0, 2, 0, 3};
		vrlib::gl::VertexP3N2B2T2T2 v;
		vrlib::gl::setN3(v, glm::vec3(0, 1, 0));
		vrlib::gl::setTan3(v, glm::vec3(1, 0, 0));
		vrlib::gl::setBiTan3(v, glm::vec3(0, 0, 1));

		vrlib::gl::setP3(v, glm::vec3(-100, 0, -100));		vrlib::gl::setT2(v, glm::vec2(-25, -25));		mesh->vertices.push_back(v);
		vrlib::gl::setP3(v, glm::vec3( 100, 0, -100));		vrlib::gl::setT2(v, glm::vec2( 25, -25));		mesh->vertices.push_back(v);
		vrlib::gl::setP3(v, glm::vec3( 100, 0,  100));		vrlib::gl::setT2(v, glm::vec2( 25,  25));		mesh->vertices.push_back(v);
		vrlib::gl::setP3(v, glm::vec3(-100, 0,  100));		vrlib::gl::setT2(v, glm::vec2(-25,  25));		mesh->vertices.push_back(v);

		n->addComponent(new vrlib::tien::components::MeshRenderer(mesh));
	}

	callbacks["scene/get"] = [this](vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
	{
		vrlib::json::Value ret;
		ret["id"] = "scene/get";
		ret["data"] = tien.scene.asJson();
		tunnel->send(ret);
	};
	
	callbacks["scene/save"] = [](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) 
	{
		vrlib::json::Value packet;
		packet["id"] = "scene/save";
		packet["status"] = "error";
		packet["error"] = "not implemented";
		tunnel->send(packet);
	};
	callbacks["scene/load"] = [](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) 
	{
		vrlib::json::Value packet;
		packet["id"] = "scene/load";
		packet["status"] = "error";
		packet["error"] = "not implemented";
		tunnel->send(packet);
	};

	callbacks["scene/node/add"] = [this](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) 
	{
		vrlib::tien::Node* parent = &tien.scene;
		if (data.isMember("parent"))
			parent = parent->findNodeWithGuid(data["parent"]);
		if (!parent)
			return;
		
		vrlib::tien::Node* n = new vrlib::tien::Node(data["name"], parent);
		n->addComponent(new vrlib::tien::components::Transform());
		if (data.isMember("components"))
		{
			if (data["components"].isMember("transform"))
			{
				if (data["components"]["transform"].isMember("position"))
					n->transform->position = glm::vec3(data["components"]["transform"]["position"][0].asFloat(), data["components"]["transform"]["position"][1].asFloat(), data["components"]["transform"]["position"][2].asFloat());
				if (data["components"]["transform"].isMember("rotation"))
					n->transform->rotation = glm::quat(glm::vec3(glm::radians(data["components"]["transform"]["rotation"][0].asFloat()), glm::radians(data["components"]["transform"]["rotation"][1].asFloat()), glm::radians(data["components"]["transform"]["rotation"][2].asFloat())));
				if (data["components"]["transform"].isMember("scale"))
					n->transform->scale = glm::vec3(data["components"]["transform"]["scale"].asFloat(), data["components"]["transform"]["scale"].asFloat(), data["components"]["transform"]["scale"].asFloat());
			}
			if (data["components"].isMember("model"))
			{
				auto renderer = new vrlib::tien::components::ModelRenderer(data["components"]["model"]["file"]);
				if (data["components"]["model"].isMember("cullbackfaces"))
					renderer->cullBackFaces = data["components"]["model"]["cullbackfaces"];
				n->addComponent(renderer);
			}
			if (data["components"].isMember("terrain"))
			{
				if (!terrain)
				{
					delete n;
					vrlib::json::Value ret;
					ret["id"] = "scene/node/add";
					ret["data"]["status"] = "error";
					ret["data"]["error"] = "No terrain added";
					tunnel->send(ret);
					return;
				}
				auto renderer = new vrlib::tien::components::TerrainRenderer(terrain);
				if (data["components"]["model"].isMember("smoothnormals"))
					renderer->smoothNormals = data["components"]["model"]["smoothnormals"];
				n->addComponent(renderer);
			}
		}
		vrlib::json::Value ret;
		ret["id"] = "scene/node/add";
		ret["data"]["uuid"] = n->guid;
		ret["data"]["status"] = "ok";
		tunnel->send(ret);
	};
	callbacks["scene/node/moveto"] = [this](vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
	{
		vrlib::json::Value packet;
		packet["id"] = "scene/node/moveto";
		packet["status"] = "error";
		packet["error"] = "not implemented";
		tunnel->send(packet);
	};
	callbacks["scene/node/update"] = [this](vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
	{
		vrlib::json::Value packet;
		packet["id"] = "scene/node/update";
		packet["status"] = "error";
		packet["error"] = "not implemented";
		tunnel->send(packet);
	};
	callbacks["scene/node/delete"] = [this](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) 
	{
		vrlib::json::Value packet;
		packet["id"] = "scene/node/delete";
		vrlib::tien::Node* node = tien.scene.findNodeWithGuid(data["id"]);
		if (node)
		{
			delete node;
			packet["data"]["status"] = "ok";
		}
		else
		{
			packet["data"]["status"] = "error";
			packet["data"]["error"] = "node not found";
		}
		tunnel->send(packet);
	};

	callbacks["scene/terrain/add"] = [this](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {
		if (!terrain) //todo: multiple terrains with a seperate guid?
			terrain = new vrlib::tien::Terrain();
		terrain->setSize(data["size"][0].asInt(), data["size"][1].asInt());
		if(data.isMember("heights"))
			for (int y = 0; y < terrain->getHeight(); y++)
				for (int x = 0; x < terrain->getWidth(); x++)
					(*terrain)[x][y] = data["heights"][x + terrain->getWidth() * y].asFloat();
		auto renderer = tien.scene.findNodeWithComponent<vrlib::tien::components::TerrainRenderer>();
		if (renderer)
			renderer->getComponent<vrlib::tien::components::TerrainRenderer>()->rebuildBuffers();
	};
	callbacks["scene/terrain/update"] = [this](vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
	{
		vrlib::json::Value packet;
		packet["id"] = "scene/terrain/update";
		packet["status"] = "error";
		packet["error"] = "not implemented";
		tunnel->send(packet);
	};
	callbacks["scene/terrain/delete"] = [this](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {
		auto renderer = tien.scene.findNodeWithComponent<vrlib::tien::components::TerrainRenderer>();
		vrlib::json::Value packet;
		packet["id"] = "scene/terrain/delete";
		vrlib::tien::Node* node = tien.scene.findNodeWithGuid(data["id"]);
		if (renderer)
		{
			packet["data"]["status"] = "error";
			packet["data"]["error"] = "terrain is still in use";
		}
		else
		{
			delete terrain;
			terrain = nullptr;
			packet["data"]["status"] = "ok";
		}
		tunnel->send(packet);
	};
	callbacks["scene/terrain/addlayer"] = [this](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {
		vrlib::json::Value packet;
		packet["id"] = "scene/terrain/addlayer";
		packet["status"] = "error";
		packet["error"] = "not implemented";
		tunnel->send(packet);
	};
	callbacks["scene/terrain/dellayer"] = [this](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {
		vrlib::json::Value packet;
		packet["id"] = "scene/terrain/dellayer";
		packet["status"] = "error";
		packet["error"] = "not implemented";
		tunnel->send(packet);
	};
	callbacks["scene/terrain/getheight"] = [this](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {
		vrlib::json::Value packet;
		packet["id"] = "scene/terrain/getheight";
		packet["status"] = "error";
		packet["error"] = "not implemented";
		tunnel->send(packet);
	};

	callbacks["scene/skybox/settime"] = [this](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) 
	{
		auto skybox = tien.scene.cameraNode->getComponent<vrlib::tien::components::DynamicSkyBox>();
		skybox->timeOfDay = data["time"].asFloat();
		logger << "Time of day: " << skybox->timeOfDay << Log::newline;
		vrlib::json::Value packet;
		packet["id"] = "scene/skybox/settime";
		packet["data"]["status"] = "ok";
		tunnel->send(packet);
	};
	callbacks["scene/skybox/update"] = [this](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};

	callbacks["scene/road/add"] = [this](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};
	callbacks["scene/road/delete"] = [this](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};

	callbacks["scene/raycast"] = [this](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};

	callbacks["path/add"] = [this](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};
	callbacks["path/edit"] = [this](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};
	callbacks["path/delete"] = [this](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};

	callbacks["play"] = [this](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {
		tien.start();
	};
	callbacks["pause"] = [this](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {
		tien.pause();
	};

	debugShader = new vrlib::gl::Shader<DebugUniform>("data/vrlib/tien/shaders/physicsdebug.vert", "data/vrlib/tien/shaders/physicsdebug.frag");
	debugShader->bindAttributeLocation("a_position", 0);
	debugShader->bindAttributeLocation("a_color", 1);
	debugShader->link();
	debugShader->registerUniform(DebugUniform::projectionMatrix, "projectionMatrix");
	debugShader->registerUniform(DebugUniform::modelViewMatrix, "modelViewMatrix");

	tien.start();
}

void NetworkEngine::preFrame(double frameTime, double totalTime)
{
	for (auto t : tunnels)
	{
		while (t->available())
		{
			vrlib::json::Value v = t->recv();
			if (callbacks.find(v["id"].asString()) != callbacks.end())
				callbacks[v["id"].asString()](t, v["data"]);
		}
	}

	for (auto &f : routeFollowers)
	{
		float dist = f.speed * (frameTime / 1000.0f);
		if (dist > 1)
			dist = 1;

		float inc = dist;

		glm::vec3 pos = f.route->getPosition(f.offset);
		glm::vec3 dir;
		dir = f.route->getPosition(f.offset + inc) - pos;

		float ff = 1.5f;

		while (glm::abs(glm::length(dir) - dist) > 0.0001f)
		{
			float l = glm::length(dir) - dist;
			if (l > 0)
				inc /= ff;
			else
				inc *= ff;
			dir = f.route->getPosition(f.offset + inc) - pos;
			ff = 1 + (ff - 1) * .9f;
		}
		
		f.offset += inc;
		f.node->transform->position = f.route->getPosition(f.offset);

		
		if (f.rotate != RouteFollower::Rotate::NONE)
		{
			glm::vec3 dir = f.route->getPosition(f.offset + 0.05f) - f.node->transform->position;
			if (f.rotate == RouteFollower::Rotate::XZ)
				f.node->transform->rotation = glm::quat(glm::vec3(0, glm::pi<float>() - glm::atan(dir.z, dir.x), 0)) * f.rotateOffset;
			if (f.rotate == RouteFollower::Rotate::XYZ)
				f.node->transform->rotation = glm::quat(glm::lookAt(f.node->transform->position, f.node->transform->position + dir, glm::vec3(0,1,0))) * f.rotateOffset;
		}

	}

	//tien.scene.cameraNode->transform->lookAt(routeFollowers[0].node->transform->position);
	tien.scene.cameraNode->transform->position = routeFollowers[0].node->transform->position;
	tien.scene.cameraNode->transform->rotation = glm::slerp(routeFollowers[0].node->transform->rotation, tien.scene.cameraNode->transform->rotation, 0.9f);

	tien.update((float)(frameTime / 1000.0f));
}

void NetworkEngine::draw(const glm::mat4 & projectionMatrix, const glm::mat4 & modelViewMatrix, const glm::mat4 & userMatrix)
{
	tien.render(projectionMatrix, modelViewMatrix);

	if (showRoutes)
	{
		std::vector<vrlib::gl::VertexP3C4> verts;

		for (size_t i = 0; i < routes.size(); i++)
		{
			while(routeColors.size() <= i)
				routeColors.push_back(glm::vec4(vrlib::util::randomHsv(), 1.0f));
			glm::vec4 color = routeColors[i];
			for (float f = 0; f < routes[i]->length; f += 0.1f)
			{
				verts.push_back(vrlib::gl::VertexP3C4(routes[i]->getPosition(f), color));
				verts.push_back(vrlib::gl::VertexP3C4(routes[i]->getPosition(f+0.1f), color));
			}
		}



		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		debugShader->use();
		debugShader->setUniform(DebugUniform::modelViewMatrix, glm::inverse(tien.scene.cameraNode->transform->globalTransform) * modelViewMatrix);
		debugShader->setUniform(DebugUniform::projectionMatrix, projectionMatrix);

		vrlib::gl::setAttributes<vrlib::gl::VertexP3C4>(&verts[0]);
		glLineWidth(1.0f);
		glDrawArrays(GL_LINES, 0, verts.size());
		glEnable(GL_DEPTH_TEST);



	}

}
