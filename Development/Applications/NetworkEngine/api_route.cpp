#include "api.h"

#include "Route.h"

Api route_add("route/add", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
{
	Route* r = new Route();
	for (size_t i = 0; i < data["nodes"].size(); i++)
	{
		r->addNode(glm::vec3(data["nodes"][i]["pos"][0].asFloat(), data["nodes"][i]["pos"][1].asFloat(), data["nodes"][i]["pos"][2].asFloat()),
			glm::vec3(data["nodes"][i]["dir"][0].asFloat(), data["nodes"][i]["dir"][1].asFloat(), data["nodes"][i]["dir"][2].asFloat()));
	}
	r->finish();
	engine->routes.push_back(r);

	vrlib::json::Value packet;
	packet["id"] = "route/add";
	packet["data"]["status"] = "ok";
	packet["data"]["id"] = r->id;
	tunnel->send(packet);
});



Api route_update("route/update", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
{


});


Api route_delete("route/delete", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
{
	for (size_t i = 0; i < engine->routeFollowers.size(); i++)
	{
		if (engine->routeFollowers[i].route->id == data["id"].asString())
		{
			vrlib::json::Value packet;
			packet["id"] = "route/delete";
			packet["data"]["status"] = "error";
			packet["data"]["error"] = "Route is still in use";
			tunnel->send(packet);
			return;
		}
	}

	for (size_t i = 0; i <engine->routes.size(); i++)
	{
		if (engine->routes[i]->id == data["id"].asString())
		{
			engine->routes.erase(engine->routes.begin() + i);
			vrlib::json::Value packet;
			packet["id"] = "route/delete";
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