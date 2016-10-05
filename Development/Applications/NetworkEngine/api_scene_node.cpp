#include "api.h"

#include "PanelComponent.h"
#include "WaterComponent.h"

#include <VrLib/tien/components/Transform.h>
#include <VrLib/tien/components/ModelRenderer.h>
#include <VrLib/tien/components/AnimatedModelRenderer.h>
#include <VrLib/tien/components/TerrainRenderer.h>

#include <VrLib/Log.h>
using vrlib::logger;

Api scene_node_add("scene/node/add", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, vrlib::json::Value &data)
{
	vrlib::tien::Node* parent = &engine->tien.scene;
	if (data.isMember("parent"))
		parent = parent->findNodeWithGuid(data["parent"]);
	if (!parent)
	{
		sendError(tunnel, "scene/node/add", "Parent not found");
		return;
	}
	if (!data.isMember("name"))
	{
		sendError(tunnel, "scene/node/add", "Name not specified");
		return;
	}


	vrlib::tien::Node* n = new vrlib::tien::Node(data["name"], parent);
	if (data.isMember("id"))
		n->guid = data["id"];
	n->addComponent(new vrlib::tien::components::Transform());
	if (data.isMember("components"))
	{
		if (data["components"].isMember("transform"))
		{
			if (!data["components"]["transform"]["scale"].isFloat() && !data["components"]["transform"]["scale"].isInt())
			{
				sendError(tunnel, "scene/node/add", "transform/scale should be a float");
				return;
			}

			if (data["components"]["transform"].isMember("position"))
				n->transform->position = glm::vec3(data["components"]["transform"]["position"][0].asFloat(), data["components"]["transform"]["position"][1].asFloat(), data["components"]["transform"]["position"][2].asFloat());
			if (data["components"]["transform"].isMember("rotation"))
				n->transform->rotation = glm::quat(glm::vec3(glm::radians(data["components"]["transform"]["rotation"][0].asFloat()), glm::radians(data["components"]["transform"]["rotation"][1].asFloat()), glm::radians(data["components"]["transform"]["rotation"][2].asFloat())));
			if (data["components"]["transform"].isMember("scale"))
				n->transform->scale = glm::vec3(data["components"]["transform"]["scale"].asFloat(), data["components"]["transform"]["scale"].asFloat(), data["components"]["transform"]["scale"].asFloat());
		}
		if (data["components"].isMember("model"))
		{
			if (data["components"]["model"].isMember("animated") && data["components"]["model"]["animated"].asBool())
			{
				auto renderer = new vrlib::tien::components::AnimatedModelRenderer(data["components"]["model"]["file"]);

				if (data["components"]["model"].isMember("animation"))
					renderer->playAnimation(data["components"]["model"]["animation"], true);

				n->addComponent(renderer);
			}
			else
			{
				if (!data["components"]["model"].isMember("file"))
				{
					sendError(tunnel, "scene/node/add", "no file field found in model");
					return;
				}
				auto renderer = new vrlib::tien::components::ModelRenderer(data["components"]["model"]["file"].asString());
				if (data["components"]["model"].isMember("cullbackfaces"))
					renderer->cullBackFaces = data["components"]["model"]["cullbackfaces"];
				n->addComponent(renderer);
			}
		}
		if (data["components"].isMember("terrain"))
		{
			if (!engine->terrain)
			{
				delete n;
				vrlib::json::Value ret;
				ret["id"] = "scene/node/add";
				ret["data"]["status"] = "error";
				ret["data"]["error"] = "No terrain added";
				tunnel->send(ret);
				return;
			}
			auto renderer = new vrlib::tien::components::TerrainRenderer(engine->terrain);
			if (data["components"]["terrain"].isMember("smoothnormals"))
				renderer->smoothNormals = data["components"]["terrain"]["smoothnormals"];
			n->addComponent(renderer);
		}
		if (data["components"].isMember("water"))
		{
			glm::vec2 size = glm::vec2(data["components"]["water"]["size"][0].asFloat(), data["components"]["water"]["size"][1].asFloat());
			float resolution = 0.1f;
			if (data["components"]["water"].isMember("resolution"))
				resolution = data["components"]["water"]["resolution"].asFloat();
			n->addComponent(new WaterComponent(size, resolution));
		}
		if (data["components"].isMember("panel"))
		{
			auto panel = new PanelComponent(glm::vec2(data["components"]["panel"]["size"][0].asFloat(), data["components"]["panel"]["size"][1].asFloat()), 
											glm::ivec2(data["components"]["panel"]["resolution"][0].asInt(), data["components"]["panel"]["resolution"][0].asInt()));

			if (data["components"]["panel"].isMember("background"))
			{
				panel->clearColor = glm::vec4(	data["components"]["panel"]["background"][0].asFloat(), 
												data["components"]["panel"]["background"][1].asFloat(), 
												data["components"]["panel"]["background"][2].asFloat(), 
												data["components"]["panel"]["background"][3].asFloat());
				panel->clear();
			}


			n->addComponent(panel);
			n->addComponent(new vrlib::tien::components::MeshRenderer(panel));
		}
	}

	data["id"] = n->guid;

	vrlib::json::Value ret;
	ret["id"] = "scene/node/add";
	ret["data"]["uuid"] = n->guid;
	ret["data"]["name"] = n->name;
	ret["data"]["status"] = "ok";
	tunnel->send(ret);
});



Api scene_node_moveto("scene/node/moveto", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, vrlib::json::Value &data)
{
	if (!data.isMember("id") || !data["id"].isString())
	{
		sendError(tunnel, "scene/node/moveto", "ID field not set");
		return;
	}

	vrlib::json::Value packet;
	packet["id"] = "scene/node/moveto";
	vrlib::tien::Node* node = engine->tien.scene.findNodeWithGuid(data["id"]);
	if (node)
	{
		if (data.isMember("stop"))
		{
			for (int i = 0; i < (int)engine->movers.size(); i++)
				if (engine->movers[i].node == node)
				{
					engine->movers.erase(engine->movers.begin() + i);
					i--;
				}
		}
		else
		{
			for(size_t i = 0; i < engine->movers.size(); i++)
				if (engine->movers[i].node == node)
				{
					engine->movers.erase(engine->movers.begin() + i);
					break;
				}

			Mover m;
			m.node = node;
			m.position = glm::vec3(data["position"][0].asFloat(), data["position"][1].asFloat(), data["position"][2].asFloat());
			m.speed = data.isMember("speed") ? data["speed"].asFloat() : -1;
			m.time = data.isMember("time") ? data["time"].asFloat() : -1;
			m.interpolate = Mover::Interpolate::Linear;
			if (data.isMember("interpolate") && data["interpolate"] == "exponential")
				m.interpolate = Mover::Interpolate::Exponential;
			m.followHeight = false;
			if(data.isMember("followheight"))
				m.followHeight = data["followheight"].asBool();
			m.rotate = Mover::Rotate::NONE;
			if (data.isMember("rotate"))
			{
				if (data["rotate"] == "XZ")
					m.rotate = Mover::Rotate::XZ;
				else if (data["rotate"] == "XYZ")
					m.rotate = Mover::Rotate::XYZ;
			}
			engine->movers.push_back(m);
		}
		packet["data"]["status"] = "ok";
	}
	else
	{
		packet["data"]["status"] = "error";
		packet["data"]["error"] = "node not found";
	}
	tunnel->send(packet);
});


Api scene_node_update("scene/node/update", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, vrlib::json::Value &data)
{
	vrlib::json::Value packet;
	packet["id"] = "scene/node/update";
	if (!data.isMember("id"))
	{
		sendError(tunnel, "scene/node/update", "id not specified");
		return;
	}

	vrlib::tien::Node* node = engine->tien.scene.findNodeWithGuid(data["id"]);
	if (node)
	{
		packet["data"]["status"] = "ok";
		if (data.isMember("parent"))
		{
			vrlib::tien::Node* newParent = engine->tien.scene.findNodeWithGuid(data["parent"]);
			if (newParent)
				node->setParent(newParent);
			else
				logger << "Could not find new parent " << data["parent"].asString() << " in scene/node/update, for node " << data["id"] << " with name " << node->name << Log::newline;

		}
		if (data.isMember("transform"))
		{
			if(data["transform"].isMember("position"))
				node->transform->position = glm::vec3(data["transform"]["position"][0].asFloat(), data["transform"]["position"][1].asFloat(), data["transform"]["position"][2].asFloat());
			if (data["transform"].isMember("rotation"))
				node->transform->rotation = glm::quat(glm::vec3(glm::radians(data["transform"]["rotation"][0].asFloat()), glm::radians(data["transform"]["rotation"][1].asFloat()), glm::radians(data["transform"]["rotation"][2].asFloat())));
			if (data["transform"].isMember("scale"))
				node->transform->scale = glm::vec3(data["transform"]["scale"].asFloat(), data["transform"]["scale"].asFloat(), data["transform"]["scale"].asFloat());
		}
		if (data.isMember("animation"))
		{
			if (data["animation"].isMember("name"))
				node->getComponent<vrlib::tien::components::AnimatedModelRenderer>()->playAnimation(data["animation"]["name"].asString());
			if (data["animation"].isMember("speed"))
				node->getComponent<vrlib::tien::components::AnimatedModelRenderer>()->animationSpeed = data["animation"]["speed"].asFloat();
		}




	}
	else
	{
		packet["data"]["status"] = "error";
		packet["data"]["error"] = "node not found";
	}
	tunnel->send(packet);

});



Api scene_terrain_addlayer("scene/node/addlayer", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, vrlib::json::Value &data)
{
	if (!data.isMember("id") || data["id"].isNull())
	{
		sendError(tunnel, "scene/node/addlayer", "No ID field");
		return;
	}
	vrlib::json::Value packet;
	packet["id"] = "scene/node/addlayer";

	vrlib::tien::Node* node = engine->tien.scene.findNodeWithGuid(data["id"]);
	if (node)
	{
		auto renderer = node->getComponent<vrlib::tien::components::TerrainRenderer>();
		if (!renderer)
		{
			packet["data"]["status"] = "error";
			packet["data"]["error"] = "node has no terrain renderer";
		}
		else
		{
			packet["data"]["status"] = "ok";
			renderer->addMaterialLayer(data["diffuse"].asString(), data["normal"].asString(), data["minHeight"].asFloat(), data["maxHeight"].asFloat(), data["fadeDist"].asFloat());
		}




	}
	else
	{
		packet["data"]["status"] = "error";
		packet["data"]["error"] = "node not found";
	}
	tunnel->send(packet);
});




Api scene_terrain_dellayer("scene/node/dellayer", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, vrlib::json::Value &data)
{
	vrlib::json::Value packet;
	packet["id"] = "scene/node/dellayer";
	packet["status"] = "error";
	packet["error"] = "not implemented";
	tunnel->send(packet);
});



Api scene_node_delete("scene/node/delete", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, vrlib::json::Value &data)
{
	vrlib::json::Value packet;
	packet["id"] = "scene/node/delete";
	if (!data.isMember("id"))
	{
		sendError(tunnel, "scene/node/delete", "id not specified");
		return;
	}
	vrlib::tien::Node* node = engine->tien.scene.findNodeWithGuid(data["id"]);
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
});





Api scene_node_find("scene/node/find", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, vrlib::json::Value &data)
{
	vrlib::json::Value packet;
	packet["id"] = "scene/node/find";
	if (data.isMember("name"))
	{
		vrlib::json::Value meshes(vrlib::json::Type::arrayValue);

		std::vector<vrlib::tien::Node*> nodes = engine->tien.scene.findNodesWithName(data["name"]);
		for (auto n : nodes)
			packet["data"].push_back(n->asJson(meshes));
	}
		
	tunnel->send(packet);
});


