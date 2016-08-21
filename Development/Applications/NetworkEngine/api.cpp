#include "api.h"
#include <map>

#include <VrLib/Log.h>
using vrlib::logger; using vrlib::Log;


Api::Api(const std::string &route, const std::function<void(NetworkEngine*, vrlib::Tunnel*, const vrlib::json::Value &)> &callback)
{
	callbacks()[route] = callback;
}


Api version("version", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
{
});



Api play("play", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
{
	engine->tien.start();
});


Api pause("pause", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
{
	engine->tien.pause();
});

