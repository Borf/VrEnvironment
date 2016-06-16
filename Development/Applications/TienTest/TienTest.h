#pragma once

#include <VrLib/Application.h>
#include <VrLib/Device.h>
#include <VrLib/tien/Tien.h>
#include <VrLib/tien/Node.h>
#include <list>
#include <vector>

class TienTest : public vrlib::Application
{
	vrlib::DigitalDevice leftButton;
	vrlib::PositionalDevice mHead;
	vrlib::PositionalDevice  mWand;
	vrlib::PositionalDevice  mWandLeft;

	vrlib::tien::Tien tien;

	std::vector<vrlib::tien::Node*> movingLights;
	vrlib::tien::Node* sunLight;
public:
	TienTest();


	virtual void init() override;
	virtual void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix) override;
	virtual void preFrame(double frameTime, double totalTime) override;
};

