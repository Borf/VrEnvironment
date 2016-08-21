#include "api.h"

#include "Route.h"


Api scene_road_add("scene/road/add", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
{
	for (size_t i = 0; i < engine->routes.size(); i++)
	{
		if (engine->routes[i]->id == data["id"].asString())
		{
			vrlib::json::Value packet;
			packet["id"] = "scene/road/add";
			packet["data"]["status"] = "ok";
			tunnel->send(packet);
			return;
		}
	}
	vrlib::json::Value packet;
	packet["id"] = "route/delete";
	packet["data"]["status"] = "error";
	packet["data"]["error"] = "Route not found";
	tunnel->send(packet);






});

Api scene_road_delete("scene/road/delete", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
{


});
