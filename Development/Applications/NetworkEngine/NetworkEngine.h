#pragma once

#include <VrLib/Application.h>
#include <VrLib/tien/Tien.h>
#include <VrLib/util.h>
#include <glm/gtc/quaternion.hpp>




namespace vrlib {
	class Tunnel;
	class Texture;
	namespace json { class Value; }
	namespace tien { class Terrain;  class Node; }
}
class Route;
class NetworkEngine;
std::map<std::string, std::function<void(NetworkEngine*, vrlib::Tunnel*, const vrlib::json::Value &)>> &callbacks();

class RouteFollower
{
public:
	vrlib::tien::Node* node;
	Route* route;
	float speed;
	float offset;
	std::string id = vrlib::util::getGuid();
	enum class Rotate
	{
		XZ,
		XYZ,
		NONE,
	} rotate = Rotate::NONE;

	glm::quat rotateOffset;


};



class NetworkEngine : public vrlib::Application
{
public:
	enum class DebugUniform
	{
		projectionMatrix,
		modelViewMatrix,
	};
	vrlib::gl::Shader<DebugUniform>* debugShader;

	std::vector<vrlib::Tunnel*> tunnels;


	std::vector<Route*> routes;
	bool showRoutes = true;
	std::vector<glm::vec4> routeColors;

	std::vector<RouteFollower> routeFollowers;

	



	NetworkEngine();
	~NetworkEngine();
	
	vrlib::tien::Tien tien;

	vrlib::tien::Terrain* terrain = nullptr;

	// Inherited via Demo
	virtual void init() override;
	virtual void preFrame(double frameTime, double totalTime) override;
	virtual void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix, const glm::mat4 &userMatrix) override;
};

