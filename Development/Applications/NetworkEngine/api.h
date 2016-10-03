#pragma once

#include <string>
#include <functional>
#include <VrLib/json.h>
#include <VrLib/ServerConnection.h>
#include "NetworkEngine.h"

struct Api
{
	Api(const std::string &route, const std::function<void(NetworkEngine*, vrlib::Tunnel*, vrlib::json::Value &)> &callback);
};


void sendError(vrlib::Tunnel* tunnel, const std::string &packet, const std::string &error);
void sendOk(vrlib::Tunnel* tunnel, const std::string &packet);