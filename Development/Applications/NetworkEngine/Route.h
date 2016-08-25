#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <VrLib/util.h>

class Route
{
	std::vector<std::tuple<glm::vec3, glm::vec3, float> > nodes;
public:
	std::string id = vrlib::util::getGuid();

	void addNode(const glm::vec3 &position, const glm::vec3 &direction);
	void finish();

	glm::vec3 getPosition(float index) const;
	inline glm::vec2 getPositionXY(float index) const { glm::vec3 p = getPosition(index); return glm::vec2(p.x, p.z); }
	float length = 0;


};