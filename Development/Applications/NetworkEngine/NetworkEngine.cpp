#include "NetworkEngine.h"
#include "Route.h"
#include "WaterComponent.h"

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
#include <VrLib/Font.h>

inline std::map<std::string, std::function<void(NetworkEngine*, vrlib::Tunnel*, vrlib::json::Value &)>> &callbacks()
{
	static std::map<std::string, std::function<void(NetworkEngine*, vrlib::Tunnel*, vrlib::json::Value &)>> callbacks;
	return callbacks;
}


vrlib::TrueTypeFont* font;

NetworkEngine::NetworkEngine()
{
}


NetworkEngine::~NetworkEngine()
{
}

void NetworkEngine::init()
{
	tien.init();
	vive.init();

	clusterData.init();

	if (clusterData.isLocal())
	{
		logger << "I'm the master, I can tunnel" << Log::newline;
		vrlib::Kernel::getInstance()->serverConnection->onTunnelCreate(tunnelKey, [this](vrlib::Tunnel* tunnel)
		{
			logger << "Got a tunnel" << Log::newline;
			tunnels.push_back(tunnel);
		});

		vrlib::json::Value packet;
		packet["id"] = "scene/reset";
		packet["data"];
		callbacks()["scene/reset"](this, nullptr, packet["data"]);

		std::string jsonStr;
		jsonStr << packet;
		clusterData->networkPackets.push_back(jsonStr);

	}



/*	vrlib::json::Value save;
	save["filename"] = "default.json";
	save["overwrite"] = true;
	callbacks()["scene/save"](this, nullptr, save);
	callbacks()["scene/load"](this, nullptr, save);*/


	debugShader = new vrlib::gl::Shader<DebugUniform>("data/vrlib/tien/shaders/physicsdebug.vert", "data/vrlib/tien/shaders/physicsdebug.frag");
	debugShader->bindAttributeLocation("a_position", 0);
	debugShader->bindAttributeLocation("a_texture", 1);
	debugShader->link();
	debugShader->registerUniform(DebugUniform::projectionMatrix, "projectionMatrix");
	debugShader->registerUniform(DebugUniform::modelViewMatrix, "modelViewMatrix");

	logFile.open("log.txt", std::ofstream::out);
	logFile << "[";

	font = new vrlib::TrueTypeFont("segoeui");
	tien.start();
}



void NetworkEngine::reset()
{
	while (tien.scene.getFirstChild())
		delete tien.scene.getFirstChild();
	for (auto r : routes)
		delete r;
	routes.clear();
	routeFollowers.clear();
	movers.clear();

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


	{
		vrlib::tien::Node* n = new vrlib::tien::Node("LeftHand", &tien.scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0)));
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/vrlib/rendermodels/vr_controller_vive_1_5/vr_controller_vive_1_5.obj"));
		n->addComponent(new vrlib::tien::components::TransformAttach(vive.controllers[1].transform));
	}
	{
		vrlib::tien::Node* n = new vrlib::tien::Node("RightHand", &tien.scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0)));
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/vrlib/rendermodels/vr_controller_vive_1_5/vr_controller_vive_1_5.obj"));
		n->addComponent(new vrlib::tien::components::TransformAttach(vive.controllers[0].transform));
	}
	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Head", &tien.scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0)));
//		n->addComponent(new vrlib::tien::components::ModelRenderer("data/vrlib/rendermodels/generic_hmd/generic_hmd.obj"));
		n->addComponent(new vrlib::tien::components::TransformAttach(vive.hmd));
	}



	if (false)
	{
		terrain = new vrlib::tien::Terrain();
		terrain->setSize(32, 32);
		for (int x = 0; x < 32; x++)
			for (int y = 0; y < 32; y++)
				(*terrain)[x][y] = 2 + sin(x / 2.0f) + cos(y / 2.0f);// rand() / (float)RAND_MAX;
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
		vrlib::tien::Node* n = new vrlib::tien::Node("GroundPlane", &tien.scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, -0.01f, 0)));

		vrlib::tien::components::MeshRenderer::Mesh* mesh = new vrlib::tien::components::MeshRenderer::Mesh();
		mesh->material.texture = vrlib::Texture::loadCached("data/NetworkEngine/textures/grid.png");
		mesh->indices = { 2, 1, 0, 2, 0, 3 };
		vrlib::gl::VertexP3N2B2T2T2 v;
		vrlib::gl::setN3(v, glm::vec3(0, 1, 0));
		vrlib::gl::setTan3(v, glm::vec3(1, 0, 0));
		vrlib::gl::setBiTan3(v, glm::vec3(0, 0, 1));

		vrlib::gl::setP3(v, glm::vec3(-100, 0, -100));		vrlib::gl::setT2(v, glm::vec2(-25, -25));		mesh->vertices.push_back(v);
		vrlib::gl::setP3(v, glm::vec3(100, 0, -100));		vrlib::gl::setT2(v, glm::vec2(25, -25));		mesh->vertices.push_back(v);
		vrlib::gl::setP3(v, glm::vec3(100, 0, 100));		vrlib::gl::setT2(v, glm::vec2(25, 25));		mesh->vertices.push_back(v);
		vrlib::gl::setP3(v, glm::vec3(-100, 0, 100));		vrlib::gl::setT2(v, glm::vec2(-25, 25));		mesh->vertices.push_back(v);

		n->addComponent(new vrlib::tien::components::MeshRenderer(mesh));
	}
}



void NetworkEngine::preFrame(double frameTime, double totalTime)
{
	for (auto t : tunnels)
	{
		while (t->available())
		{
			vrlib::json::Value v = t->recv();
			if (logFile.is_open())
			{
				logFile << v << std::endl;
				logFile << "," << std::endl << std::endl;
				logFile.flush();
			}

			if (v.isObject() && v.isMember("id"))
			{
				logger << "Got packet " << v["id"] << "Through tunnel"<<Log::newline;
				if (callbacks().find(v["id"].asString()) != callbacks().end())
					callbacks()[v["id"].asString()](this, t, v["data"]);
				else
					logger << "No callback registered for packet " << v["id"] << Log::newline;
			}
			else
				logger << "Packet received is not an object, or no ID field found!" << Log::newline;


			std::string jsonStr;
			jsonStr << v;
			clusterData->networkPackets.push_back(jsonStr);
		}
	}

	for (auto &f : routeFollowers)
	{
		float dist = f.speed * (float)(frameTime / 1000.0f);
		if (dist > 1)
			dist = 1;

		float inc = dist;

		glm::vec3 pos = f.route->getPosition(f.offset);
		glm::vec3 dir;
		dir = f.route->getPosition(f.offset + inc) - pos;

		float ff = 1.5f;
		int i = 0;
		while (glm::abs(glm::length(dir) - dist) > 0.0001f && i < 100)
		{
			float l = glm::length(dir) - dist;
			if (l > 0)
				inc /= ff;
			else
				inc *= ff;
			dir = f.route->getPosition(f.offset + inc) - pos;
			ff = 1 + (ff - 1) * .9f;
			i++;
		}
		
		f.offset += inc;
		f.node->transform->position = f.route->getPosition(f.offset);

		
		if (f.rotate != RouteFollower::Rotate::NONE)
		{
			glm::vec3 dir = f.route->getPosition(f.offset + 0.05f) - f.node->transform->position;
			if (f.rotate == RouteFollower::Rotate::XZ)
				f.node->transform->rotation = glm::quat(glm::vec3(0, glm::pi<float>() - glm::atan(dir.z, dir.x), 0)) * f.rotateOffset;
			if (f.rotate == RouteFollower::Rotate::XYZ)
				f.node->transform->rotation = glm::quat(glm::inverse(glm::lookAt(f.node->transform->position, f.node->transform->position + dir, glm::vec3(0,1,0)))) * f.rotateOffset;
		}

		if (terrain && f.followHeight)
		{
			vrlib::tien::Node* terrainRenderingNode = tien.scene.findNodeWithComponent<vrlib::tien::components::TerrainRenderer>();

			glm::vec2 offset(0, 0);
			if (terrainRenderingNode)
				offset = glm::vec2(terrainRenderingNode->transform->position.x, terrainRenderingNode->transform->position.z);

			f.node->transform->position = terrain->getPosition(glm::vec2(f.node->transform->position.x - offset.x, f.node->transform->position.z - offset.y)) + glm::vec3(offset.x, 0, offset.y);
		}
	}

	for (int i = 0; i < (int)movers.size(); i++)
	{
		Mover& m = movers[i];
		if (m.interpolate == Mover::Interpolate::Linear)
		{
			float movement = (float)(frameTime / 1000.0f) * m.speed;
			glm::vec3 diff = m.position - m.node->transform->position;
			if (glm::length(diff) > movement)
				m.node->transform->position += movement * glm::normalize(diff);
			else
				m.node->transform->position = m.position;
		}
		else if (m.interpolate == Mover::Interpolate::Exponential)
		{
			float movement = glm::pow(m.speed, (float)(frameTime / 1000.0));
			glm::vec3 diff = m.node->transform->position - m.position;
			if (glm::length(diff) < 0.01f)
				m.node->transform->position = glm::mix(m.node->transform->position, m.position, movement);
			else
				m.node->transform->position = m.position;
		}
		if (m.rotate != Mover::Rotate::NONE)
		{
			glm::vec3 dir = m.node->transform->position - m.position;
			if (m.rotate == Mover::Rotate::XZ)
				m.node->transform->rotation = glm::quat(glm::vec3(0, glm::pi<float>() - glm::atan(dir.z, dir.x), 0)) * m.rotateOffset;
			if (m.rotate == Mover::Rotate::XYZ)
				m.node->transform->rotation = glm::quat(glm::lookAt(m.node->transform->position, m.node->transform->position + dir, glm::vec3(0, 1, 0))) * m.rotateOffset;
		}
		if (terrain && m.followHeight)
		{
			m.node->transform->position = terrain->getPosition(glm::vec2(128 + m.node->transform->position.x, 128 + m.node->transform->position.z)) + glm::vec3(-128, 0, -128);
		}

		if (glm::distance(m.node->transform->position, m.position) < 0.001f)
		{
			vrlib::json::Value packet;
			packet["id"] = "scene/node/moveto";
			packet["data"]["status"] = "done";
			packet["data"]["node"] = m.node->guid;
			for (auto t : tunnels)
				t->send(packet);
			movers.erase(movers.begin() + i);
			i--;
		}
	}

	auto checkButtonCallback = [this](CallbackMasks callback, vrlib::DigitalDevice& button)
	{
		vrlib::DigitalState state = button.getData();
		if (state == vrlib::DigitalState::TOGGLE_ON)
		{
			vrlib::json::Value packet;
			packet["id"] = "callback";
			packet["data"]["button"] = button.name;
			packet["data"]["state"] = "on";
			for (auto t : tunnels)
				t->send(packet);
		}
		if (state == vrlib::DigitalState::TOGGLE_OFF)
		{
			vrlib::json::Value packet;
			packet["id"] = "callback";
			packet["data"]["button"] = button.name;
			packet["data"]["state"] = "off";
			for (auto t : tunnels)
				t->send(packet);
		}
	};


	checkButtonCallback(LeftTrigger,		vive.controllers[0].triggerButton);
	checkButtonCallback(LeftApplication,	vive.controllers[0].applicationMenuButton);
	checkButtonCallback(LeftTouchPad,		vive.controllers[0].touchButton);
	checkButtonCallback(LeftGrip,			vive.controllers[0].gripButton);

	checkButtonCallback(RightTrigger,		vive.controllers[1].triggerButton);
	checkButtonCallback(RightApplication,	vive.controllers[1].applicationMenuButton);
	checkButtonCallback(RightTouchPad,		vive.controllers[1].touchButton);
	checkButtonCallback(RightGrip,			vive.controllers[1].gripButton);

	this->frameTime = frameTime;
}


void NetworkEngine::latePreFrame()
{
	if (!clusterData.isLocal())
	{
		for (size_t i = 0; i < clusterData->networkPackets.size(); i++)
		{
			vrlib::json::Value v = vrlib::json::readJson(clusterData->networkPackets[i]);

			if (v.isObject() && v.isMember("id"))
			{
				logger << "Got packet " << v["id"] << "Through tunnel" << Log::newline;
				if (callbacks().find(v["id"].asString()) != callbacks().end())
					callbacks()[v["id"].asString()](this, nullptr, v["data"]);
				else
					logger << "No callback registered for packet " << v["id"] << Log::newline;
			}
			else
				logger << "Packet received is not an object, or no ID field found!" << Log::newline;

		}
	}
	clusterData->networkPackets.clear();
	tien.update((float)(frameTime / 1000.0f));
}

void NetworkEngine::draw(const glm::mat4 & projectionMatrix, const glm::mat4 & modelViewMatrix)
{
	tien.render(projectionMatrix, modelViewMatrix);

	if (showRoutes && routes.size() > 0)
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



		glEnable(GL_DEPTH_TEST);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		debugShader->use();
		debugShader->setUniform(DebugUniform::modelViewMatrix, modelViewMatrix * glm::inverse(tien.scene.cameraNode->transform->globalTransform));
		debugShader->setUniform(DebugUniform::projectionMatrix, projectionMatrix);

		vrlib::gl::setAttributes<vrlib::gl::VertexP3C4>(&verts[0]);
		glLineWidth(1.0f);
		glDrawArrays(GL_LINES, 0, verts.size());
		glEnable(GL_DEPTH_TEST);



	}


}

void NetworkData::writeObject(vrlib::BinaryStream & writer)
{
	writer << (int)networkPackets.size();
	for (const auto & s : networkPackets)
		writer << s;
}

void NetworkData::readObject(vrlib::BinaryStream & reader)
{
	int count;
	reader >> count;
	networkPackets.clear();
	for (int i = 0; i < count; i++)
	{
		std::string packet;
		reader >> packet;
		networkPackets.push_back(packet);
	}
}

int NetworkData::getEstimatedSize()
{
	int sum = 10;
	for (size_t i = 0; i < networkPackets.size(); i++)
		sum += networkPackets[i].size() + 4;
	return sum;
}
