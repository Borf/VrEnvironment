#include "Route.h"
#include <VrLib/math/HermiteCurve.h>

#include <tuple>


void Route::addNode(const glm::vec3 &position, const glm::vec3 &direction)
{
	nodes.push_back(std::make_tuple(position, direction, 0.0f));
}

void Route::finish()
{
	length = 0;
	for (size_t i = 0; i < nodes.size(); i++)
	{
		int i1 = i;
		int i2 = (i + 1) % nodes.size();
		vrlib::math::HermiteCurve<glm::vec3> hermiteCurve(std::get<0>(nodes[i1]), std::get<1>(nodes[i1]), std::get<0>(nodes[i2]), std::get<1>(nodes[i2]));
		std::get<2>(nodes[i1]) = hermiteCurve.getLength();
		length += std::get<2>(nodes[i1]);
	}

}

glm::vec3 Route::getPosition(float index) const
{
	while (index > length)
		index -= length;

	int i = 0;
	float t = 0;
	while (index > t + std::get<2>(nodes[i]))
	{
		t += std::get<2>(nodes[i]);
		i++;
	}
	index -= t;


	int i1 = (int)glm::floor(i);
	int i2 = (i1 + 1) % nodes.size();

	float fract = index / std::get<2>(nodes[i1]);



	vrlib::math::HermiteCurve<glm::vec3> hermiteCurve(std::get<0>(nodes[i1]), std::get<1>(nodes[i1]), std::get<0>(nodes[i2]), std::get<1>(nodes[i2]));
	return hermiteCurve.getPoint(fract);


}


