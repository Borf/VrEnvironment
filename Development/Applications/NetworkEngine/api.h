#pragma once

#include <string>
#include <functional>
#include <VrLib/json.h>
#include <VrLib/ServerConnection.h>
#include "NetworkEngine.h"

struct Api
{
	Api(const std::string &route, const std::function<void(NetworkEngine*, vrlib::Tunnel*, const vrlib::json::Value &)> &callback);
};