#include "api.h"

#include <fstream>

Api scene_get("scene/get", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, vrlib::json::Value &data)
{
	vrlib::json::Value meshes(vrlib::json::Type::arrayValue);

	vrlib::json::Value ret;
	ret["id"] = "scene/get";
	ret["data"] = engine->tien.scene.asJson(meshes);
	tunnel->send(ret);
});


Api scene_save("scene/save", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, vrlib::json::Value &data)
{
	vrlib::json::Value d;
	d["meshes"] = vrlib::json::Value(vrlib::json::Type::arrayValue);
	d["tree"] = engine->tien.scene.asJson(d["meshes"]);

	std::ofstream file("engine.json");
	file << d;
	file.close();


	vrlib::json::Value packet;
	packet["id"] = "scene/save";
	packet["status"] = "error";
	packet["error"] = "not implemented";
	if(tunnel)
		tunnel->send(packet);
});


Api scene_load("scene/load", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, vrlib::json::Value &data)
{
	std::ifstream file("engine.json");
	vrlib::json::Value json = vrlib::json::readJson(file);

	engine->tien.scene.fromJson(json["tree"], json);
	engine->tien.scene.cameraNode = nullptr;

	vrlib::json::Value packet;
	packet["id"] = "scene/load";
	packet["status"] = "error";
	packet["error"] = "not implemented";
	if(tunnel)
		tunnel->send(packet);
});


Api scene_raycast("scene/raycast", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, vrlib::json::Value &data)
{
	vrlib::json::Value packet;
	packet["id"] = "scene/raycast";
	packet["status"] = "error";
	packet["error"] = "not implemented";
	tunnel->send(packet);
});


Api scene_reset("scene/reset", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, vrlib::json::Value &data)
{
	engine->reset();

	std::string nodes[] = { "Camera", "Sunlight", "LeftHand", "RightHand", "Head", "GroundPlane" };
	for (int i = 0; i < sizeof(nodes) / sizeof(std::string); i++)
	{
		if (data.isMember(nodes[i]))
			engine->tien.scene.findNodesWithName(nodes[i])[0]->guid = data[nodes[i]];
		data[nodes[i]] = engine->tien.scene.findNodesWithName(nodes[i])[0]->guid;

	}


	if (tunnel)
	{
		vrlib::json::Value packet;
		packet["id"] = "scene/reset";
		packet["status"] = "ok";
		tunnel->send(packet);
	}
});
