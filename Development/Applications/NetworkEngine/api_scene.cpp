#include "api.h"


Api scene_get("scene/get", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
{
	vrlib::json::Value ret;
	ret["id"] = "scene/get";
	ret["data"] = engine->tien.scene.asJson();
	tunnel->send(ret);
});


Api scene_save("scene/save", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
{
	vrlib::json::Value packet;
	packet["id"] = "scene/save";
	packet["status"] = "error";
	packet["error"] = "not implemented";
	tunnel->send(packet);
});


Api scene_load("scene/load", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
{
	vrlib::json::Value packet;
	packet["id"] = "scene/load";
	packet["status"] = "error";
	packet["error"] = "not implemented";
	tunnel->send(packet);
});


Api scene_raycast("scene/raycast", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
{
	vrlib::json::Value packet;
	packet["id"] = "scene/raycast";
	packet["status"] = "error";
	packet["error"] = "not implemented";
	tunnel->send(packet);
});


Api scene_reset("scene/reset", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
{
	engine->reset();
	if (tunnel)
	{
		vrlib::json::Value packet;
		packet["id"] = "scene/reset";
		packet["status"] = "ok";
		tunnel->send(packet);
	}
});
