#pragma once

#include "Demo.h"
#include <vector>
#include <glm/glm.hpp>

namespace vrlib { namespace tien { class Node;  } }

class Route
{
	std::vector<std::tuple<glm::vec2, glm::vec2, float> > nodes;
	float length = 0;
public:
	void addNode(const glm::vec2 &position, const glm::vec2 &direction);
	void finish();
	
	glm::vec2 getPosition(float index);


};


class Biker : public Demo
{
public:
	Biker();
	~Biker();
	
	vrlib::tien::Node* bike;

	Route route;

	float time = 0;
	float position = 0;

	// Inherited via Demo
	virtual void init(vrlib::tien::Scene & scene, TienTest * app) override;
	virtual void update(float frameTime, vrlib::tien::Scene & scene, TienTest * app) override;
};

