#include "NetworkEngine.h"

#include <VrLib/ServerConnection.h>
#include <VrLib/Kernel.h>
#include <VrLib/Log.h>

using vrlib::logger;

#include <VrLib/Texture.h>
#include <VrLib/math/HermiteCurve.h>
#include <VrLib/math/Ray.h>
#include <VrLib/tien/Scene.h>
#include <VrLib/tien/Node.h>
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

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Camera", &tien.scene);
		n->addComponent(new vrlib::tien::components::Transform());
		n->addComponent(new vrlib::tien::components::Camera());
		n->addComponent(new vrlib::tien::components::DynamicSkyBox());
		tien.scene.cameraNode = n;
	}

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Sunlight", &tien.scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 1, 1)));
		vrlib::tien::components::Light* light = new vrlib::tien::components::Light();
		light->color = glm::vec4(1, 1, 0.8627f, 1);
		light->intensity = 20.0f;
		light->type = vrlib::tien::components::Light::Type::directional;
		light->shadow = vrlib::tien::components::Light::Shadow::shadowmap;
		n->addComponent(light);
	}

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("GroundPlane", &tien.scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 1, 1)));

		vrlib::tien::components::MeshRenderer::Mesh* mesh = new vrlib::tien::components::MeshRenderer::Mesh();
		mesh->material.texture = vrlib::Texture::loadCached("data/TienTest/textures/grid.png");
		mesh->indices = { 2, 1, 0, 2, 0, 3};
		vrlib::gl::VertexP3N2B2T2T2 v;
		vrlib::gl::setN3(v, glm::vec3(0, 1, 0));
		vrlib::gl::setTan3(v, glm::vec3(1, 0, 0));
		vrlib::gl::setBiTan3(v, glm::vec3(0, 0, 1));

		vrlib::gl::setP3(v, glm::vec3(-40, 0, -40));		vrlib::gl::setT2(v, glm::vec2(-10, -10));		mesh->vertices.push_back(v);
		vrlib::gl::setP3(v, glm::vec3( 40, 0, -40));		vrlib::gl::setT2(v, glm::vec2( 10, -10));		mesh->vertices.push_back(v);
		vrlib::gl::setP3(v, glm::vec3( 40, 0,  40));		vrlib::gl::setT2(v, glm::vec2( 10,  10));		mesh->vertices.push_back(v);
		vrlib::gl::setP3(v, glm::vec3(-40, 0,  40));		vrlib::gl::setT2(v, glm::vec2(-10,  10));		mesh->vertices.push_back(v);

		n->addComponent(new vrlib::tien::components::MeshRenderer(mesh));
	}

	callbacks["scene/get"] = [this](vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
	{
		vrlib::json::Value ret;
		ret["id"] = "scene/get";
		ret["data"] = tien.scene.asJson();
		tunnel->send(ret);
	};
	
	callbacks["scene/save"] = [](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};
	callbacks["scene/load"] = [](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};

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
		}
		vrlib::json::Value ret;
		ret["id"] = "scene/node/add";
		ret["data"]["uuid"] = n->guid;
		ret["data"]["status"] = "ok";
		tunnel->send(ret);
	};
	callbacks["scene/node/moveto"] = [this](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};
	callbacks["scene/node/update"] = [this](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};
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
			packet["data"]["status"] = "error: node not found";
		tunnel->send(packet);
	};

	callbacks["scene/terrain/add"] = [](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};
	callbacks["scene/terrain/update"] = [](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};
	callbacks["scene/terrain/delete"] = [](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};
	callbacks["scene/terrain/addlayer"] = [](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};
	callbacks["scene/terrain/dellayer"] = [](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};
	callbacks["scene/terrain/getheight"] = [](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};

	callbacks["scene/skybox/settime"] = [](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};
	callbacks["scene/skybox/update"] = [](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};

	callbacks["scene/road/add"] = [](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};
	callbacks["scene/road/delete"] = [](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};

	callbacks["scene/raycast"] = [](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};

	callbacks["path/add"] = [](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};
	callbacks["path/edit"] = [](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};
	callbacks["path/delete"] = [](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};

	callbacks["play"] = [](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};
	callbacks["pause"] = [](vrlib::Tunnel* tunnel, const vrlib::json::Value &data) {};

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


	tien.update((float)(frameTime / 1000.0f));
}

void NetworkEngine::draw(const glm::mat4 & projectionMatrix, const glm::mat4 & modelViewMatrix, const glm::mat4 & userMatrix)
{
	tien.render(projectionMatrix, modelViewMatrix);
}
