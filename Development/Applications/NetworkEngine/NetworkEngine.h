#pragma once

#include <VrLib/Application.h>
#include <VrLib/tien/Tien.h>

namespace vrlib {
	class Tunnel;
	class Texture;
	namespace json { class Value; }
	namespace tien { class Terrain;  class Node; }
}






class NetworkEngine : public vrlib::Application
{
public:
	std::vector<vrlib::Tunnel*> tunnels;

	std::map<std::string, std::function<void(vrlib::Tunnel* tunnel, const vrlib::json::Value &)>> callbacks;


	NetworkEngine();
	~NetworkEngine();
	
	vrlib::tien::Tien tien;

	vrlib::tien::Terrain* terrain = nullptr;

	// Inherited via Demo
	virtual void init() override;
	virtual void preFrame(double frameTime, double totalTime) override;
	virtual void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix, const glm::mat4 &userMatrix) override;
};

