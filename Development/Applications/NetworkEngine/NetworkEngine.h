#pragma once

#include <VrLib/Application.h>
#include <VrLib/tien/Tien.h>
#include <VrLib/util.h>
#include <VrLib/ClusterData.h>
#include <glm/gtc/quaternion.hpp>
#include <VrLib/HtcVive.h>

#include <fstream>



namespace vrlib {
	class Tunnel;
	class Texture;
	namespace json { class Value; }
	namespace tien { class Terrain;  class Node; }
}
class Route;
class NetworkEngine;
std::map<std::string, std::function<void(NetworkEngine*, vrlib::Tunnel*, vrlib::json::Value &)>> &callbacks();

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

	bool followHeight;

	glm::quat rotateOffset;
	glm::vec3 positionOffset;


};

class Mover
{
public:
	vrlib::tien::Node* node;
	glm::vec3 position;

	bool followHeight;
	enum class Rotate
	{
		XZ,
		XYZ,
		NONE,
	} rotate = Rotate::NONE;

	float speed;
	float time;
	enum class Interpolate
	{
		Linear,
		Exponential
	} interpolate = Interpolate::Linear;
	glm::quat rotateOffset;
};

class NetworkData : public vrlib::SerializableObject
{
public:
	std::vector<std::string> networkPackets;
	virtual void writeObject(vrlib::BinaryStream & writer) override;
	virtual void readObject(vrlib::BinaryStream & reader) override;
	virtual int getEstimatedSize();
};


class NetworkEngine : public vrlib::Application
{
public:
	std::string tunnelKey = "";
	enum class DebugUniform
	{
		projectionMatrix,
		modelViewMatrix,
	};
	vrlib::gl::Shader<DebugUniform>* debugShader;

	vrlib::ClusterData<NetworkData> clusterData;


	std::ofstream logFile;


	std::vector<vrlib::Tunnel*> tunnels;


	std::vector<Route*> routes;
	bool showRoutes = true;
	std::vector<glm::vec4> routeColors;

	std::vector<RouteFollower> routeFollowers;
	std::vector<Mover> movers;

	vrlib::Vive vive;

	enum CallbackMasks
	{
		LeftTrigger = 1,
		LeftTouchPad = 2,
		LeftApplication = 4,
		LeftGrip = 8,
		RightTrigger = 16,
		RightTouchPad = 32,
		RightApplication = 64,
		RightGrip = 128,
	};
	int callbackMask = 0;

	NetworkEngine();
	~NetworkEngine();
	
	vrlib::tien::Tien tien;

	vrlib::tien::Terrain* terrain = nullptr;

	double frameTime;
	// Inherited via Demo
	virtual void init() override;
	virtual void preFrame(double frameTime, double totalTime) override;
	virtual void latePreFrame();
	virtual void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix) override;

	void reset();

};

