#include "api.h"

#include <VrLib/tien/components/Transform.h>
#include <VrLib/tien/components/ModelRenderer.h>
#include <VrLib/tien/components/TerrainRenderer.h>


Api scene_node_add("scene/node/add", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
{
	vrlib::tien::Node* parent = &engine->tien.scene;
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
});



Api scene_node_moveto("scene/node/moveto", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
{
	vrlib::json::Value packet;
	packet["id"] = "scene/node/moveto";
	packet["status"] = "error";
	packet["error"] = "not implemented";
	tunnel->send(packet);
});


Api scene_node_update("scene/node/update", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
{
	vrlib::json::Value packet;
	packet["id"] = "scene/node/update";
	packet["status"] = "error";
	packet["error"] = "not implemented";
	tunnel->send(packet);
});



Api scene_node_delete("scene/node/delete", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
{
	vrlib::json::Value packet;
	packet["id"] = "scene/node/delete";
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



