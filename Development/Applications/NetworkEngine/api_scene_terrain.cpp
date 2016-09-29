#include "api.h"

#include <VrLib/tien/components/TerrainRenderer.h>
#include <VrLib/tien/Terrain.h>

Api scene_terrain_add("scene/terrain/add", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, vrlib::json::Value &data)
{
	if (!data.isMember("size") || !data["size"].isArray())
	{
		sendError(tunnel, "scene/terrain/add", "Size parameter is needed");
		return;
	}

	if (data["size"][0].asInt() > 1024 || data["size"][1].asInt() > 1024)
	{
		sendError(tunnel, "scene/terrain/add", "Size of terrain too big");
		return;
	}
	if (data.isMember("heights") && data["heights"].size() < data["size"][0].asInt() * data["size"][1].asInt())
	{
		sendError(tunnel, "scene/terrain/add", "Not enough terrain height data");
		return;
	}



	if (!engine->terrain) //todo: multiple terrains with a seperate guid?
		engine->terrain = new vrlib::tien::Terrain();
	engine->terrain->setSize(data["size"][0].asInt(), data["size"][1].asInt());
	if (data.isMember("heights"))
		for (int y = 0; y < engine->terrain->getHeight(); y++)
			for (int x = 0; x < engine->terrain->getWidth(); x++)
				(*engine->terrain)[x][y] = data["heights"][x + engine->terrain->getWidth() * y].asFloat();
	auto renderer = engine->tien.scene.findNodeWithComponent<vrlib::tien::components::TerrainRenderer>();
	if (renderer)
		renderer->getComponent<vrlib::tien::components::TerrainRenderer>()->rebuildBuffers();
	sendOk(tunnel, "scene/terrain/add");
});


Api scene_terrain_update("scene/terrain/update", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, vrlib::json::Value &data)
{
	vrlib::json::Value packet;
	packet["id"] = "scene/terrain/update";
	packet["status"] = "error";
	packet["error"] = "not implemented";
	tunnel->send(packet);
});


Api scene_terrain_delete("scene/terrain/delete", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, vrlib::json::Value &data)
{
	auto renderer = engine->tien.scene.findNodeWithComponent<vrlib::tien::components::TerrainRenderer>();
	vrlib::json::Value packet;
	packet["id"] = "scene/terrain/delete";
	vrlib::tien::Node* node = engine->tien.scene.findNodeWithGuid(data["id"]);
	if (renderer)
	{
		packet["data"]["status"] = "error";
		packet["data"]["error"] = "terrain is still in use";
	}
	else
	{
		delete engine->terrain;
		engine->terrain = nullptr;
		packet["data"]["status"] = "ok";
	}
	tunnel->send(packet);
});





Api scene_terrain_getheight("scene/terrain/getheight", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, vrlib::json::Value &data)
{
	vrlib::json::Value packet;
	packet["id"] = "scene/terrain/getheight";

	if (engine->terrain)
	{
		packet["status"] = "ok";
		if(data.isMember("position"))
			packet["data"]["height"] = engine->terrain->getPosition(glm::vec2(128 + data["position"][0].asFloat(), 128 + data["position"][1].asFloat())).y;
		else if(data.isMember("positions"))
		{
			for(int i = 0; i < data["positions"].size(); i++)
				packet["data"]["heights"].push_back(engine->terrain->getPosition(glm::vec2(128 + data["positions"][i][0].asFloat(), 128 + data["positions"][i][1].asFloat())).y);
		}
	}
	else
		packet["status"] = "error";


	tunnel->send(packet);
});
