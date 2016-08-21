#include "api.h"

#include <VrLib/tien/components/DynamicSkyBox.h>

Api scene_skybox_settime("scene/skybox/settime", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
{
	auto skybox = engine->tien.scene.cameraNode->getComponent<vrlib::tien::components::DynamicSkyBox>();
	skybox->timeOfDay = data["time"].asFloat();
	vrlib::json::Value packet;
	packet["id"] = "scene/skybox/settime";
	packet["data"]["status"] = "ok";
	tunnel->send(packet);
});

Api scene_skybox_update("scene/skybox/update", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
{


});
	
	