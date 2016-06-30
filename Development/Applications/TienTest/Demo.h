#pragma once

namespace vrlib
{
	namespace tien
	{
		class Scene;
	}
}
class TienTest;

class Demo
{
public:
	Demo();
	virtual ~Demo();

	virtual void init(vrlib::tien::Scene& scene, TienTest* app) = 0;
	virtual void update(float frameTime, vrlib::tien::Scene& scene, TienTest* app) = 0;
};

