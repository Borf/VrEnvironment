#pragma once

#include <vector>
#include <glm/glm.hpp>

class Route
{
	std::vector<std::tuple<glm::vec2, glm::vec2, float> > nodes;
public:
	void addNode(const glm::vec2 &position, const glm::vec2 &direction);
	void finish();

	glm::vec2 getPosition(float index);
	float length = 0;


};