#include "api.h"

#include "Route.h"

#include <algorithm>


Api route_add("route/add", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, vrlib::json::Value &data)
{
	Route* r = new Route();
	if (data.isMember("id"))
	{
		printf("Got forced route id!\n");
		r->id = data["id"];
	}
	for (size_t i = 0; i < data["nodes"].size(); i++)
	{
		r->addNode(glm::vec3(data["nodes"][i]["pos"][0].asFloat(), data["nodes"][i]["pos"][1].asFloat(), data["nodes"][i]["pos"][2].asFloat()),
			glm::vec3(data["nodes"][i]["dir"][0].asFloat(), data["nodes"][i]["dir"][1].asFloat(), data["nodes"][i]["dir"][2].asFloat()));
	}
	r->finish();
	engine->routes.push_back(r);

	data["id"] = r->id;

	vrlib::json::Value packet;
	packet["id"] = "route/add";
	packet["data"]["status"] = "ok";
	packet["data"]["uuid"] = r->id;
	tunnel->send(packet);
});



Api route_update("route/update", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, vrlib::json::Value &data)
{


});


Api route_delete("route/delete", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, vrlib::json::Value &data)
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



Api route_follow("route/follow", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, vrlib::json::Value &data)
{
	vrlib::json::Value packet;
	packet["id"] = "route/follow";
	packet["data"]["status"] = "error";

	bool found = false;
	for (size_t i = 0; i <engine->routes.size(); i++)
	{
		if (engine->routes[i]->id == data["route"].asString())
		{
			found = true;
			vrlib::tien::Node* n = engine->tien.scene.findNodeWithGuid(data["node"].asString());
			if (!n)
			{
				packet["data"]["error"] = "node not found";
				break;
			}
			for (size_t ii = 0; ii < engine->routeFollowers.size(); ii++)
			{
				auto & f = engine->routeFollowers[ii];
				if (f.node == n)
				{
					engine->routeFollowers.erase(engine->routeFollowers.begin() + ii);
					packet["data"]["status"] = "ok";
					packet["data"]["msg"] = "removed";
					tunnel->send(packet);
					return;
				}
			}

			RouteFollower f;
			f.node = n;
			f.route = engine->routes[i];
			f.speed = data["speed"].asFloat();
			f.offset = data.isMember("offset") ? data["offset"] : 0.0f;
			f.followHeight = false;
			if(data.isMember("followHeight"))
				f.followHeight = data["followHeight"].asBool();
			f.rotate = RouteFollower::Rotate::NONE;
			if (data.isMember("rotate"))
			{
				if (data["rotate"] == "XZ")
					f.rotate = RouteFollower::Rotate::XZ;
				else if (data["rotate"] == "XYZ")
					f.rotate = RouteFollower::Rotate::XYZ;
			}
			if (data.isMember("rotateOffset"))
				f.rotateOffset = glm::quat(glm::vec3(data["rotateOffset"][0].asFloat(), data["rotateOffset"][1].asFloat(), data["rotateOffset"][2].asFloat()));
			if (data.isMember("positionOffset"))
				f.positionOffset = glm::vec3(data["positionOffset"][0].asFloat(), data["positionOffset"][1].asFloat(), data["positionOffset"][2].asFloat());

			engine->routeFollowers.push_back(f);
			packet["data"]["status"] = "ok";
		}
	}
	if(!found && !packet["data"].isMember("error"))
		packet["data"]["error"] = "Route not found";
	tunnel->send(packet);
});

Api route_follow_speed("route/follow/speed", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
{
	for (auto &f : engine->routeFollowers)
	{
		if (f.node->guid == data["node"].asString())
		{
			f.speed = data["speed"].asFloat();
			sendOk(tunnel, "route/follow/speed");
			return;
		}
	}
	sendError(tunnel, "route/follow/speed", "Node ID not found");
});

Api route_show("route/show", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
{
	if (!data.isMember("show"))
		engine->showRoutes = !engine->showRoutes;
	else
		engine->showRoutes = data["show"];

	vrlib::json::Value ret;
	ret["id"] = "route/show";
	ret["data"]["status"] = "ok";
	ret["data"]["show"] = engine->showRoutes;

});