#include "api.h"

#include <VrLib/tien/components/DynamicSkyBox.h>
#include <VrLib/tien/components/StaticSkyBox.h>

Api scene_skybox_settime("scene/skybox/settime", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, vrlib::json::Value &data)
{
	auto skybox = engine->tien.scene.cameraNode->getComponent<vrlib::tien::components::DynamicSkyBox>();
	skybox->timeOfDay = data["time"].asFloat();
	sendOk(tunnel, "scene/skybox/settime");
});

Api scene_skybox_update("scene/skybox/update", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, vrlib::json::Value &data)
{
	if (!data.isMember("type"))
	{
		sendError(tunnel, "scene/skybox/update", "No type field added");
		return;
	}
	auto cameraNode = engine->tien.scene.cameraNode;
	if (data["type"] == "dynamic")
	{
		if (cameraNode->getComponent<vrlib::tien::components::DynamicSkyBox>())
		{
			sendError(tunnel, "scene/skybox/update", "Skybox is already dynamic");
			return;
		}
		cameraNode->addComponent(new vrlib::tien::components::DynamicSkyBox());
		if (cameraNode->getComponent<vrlib::tien::components::StaticSkyBox>())
		{
			auto box = cameraNode->getComponent<vrlib::tien::components::StaticSkyBox>();
			cameraNode->removeComponent<vrlib::tien::components::StaticSkyBox>();
			delete box;
		}
	}
	if (data["type"] == "static")
	{
		if (!data["files"].isObject() || data["files"].size() != 6)
		{
			sendError(tunnel, "scene/skybox/update", "No files or not enough files");
			return;
		}
		auto skybox = new vrlib::tien::components::StaticSkyBox();
		cameraNode->addComponent(skybox);
		if (cameraNode->getComponent<vrlib::tien::components::DynamicSkyBox>())
		{
			auto box = cameraNode->getComponent<vrlib::tien::components::DynamicSkyBox>();
			cameraNode->removeComponent<vrlib::tien::components::DynamicSkyBox>();
			delete box;
		}
		skybox->initialize();
		skybox->setTexture(0, data["files"]["xpos"].asString());
		skybox->setTexture(1, data["files"]["xneg"].asString());
		skybox->setTexture(2, data["files"]["ypos"].asString());
		skybox->setTexture(3, data["files"]["yneg"].asString());
		skybox->setTexture(4, data["files"]["zpos"].asString());
		skybox->setTexture(5, data["files"]["zneg"].asString());


	}

	sendOk(tunnel, "scene/skybox/update");

});
	
	