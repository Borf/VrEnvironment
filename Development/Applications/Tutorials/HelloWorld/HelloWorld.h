#pragma once

#include <VrLib/Application.h>
#include <VrLib/Device.h>

#include <list>


class HelloWorld : public vrlib::Application
{
	vrlib::DigitalDevice leftButton;
public:
	HelloWorld();


	virtual void init() override;
	virtual void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix) override;
	virtual void preFrame(double frameTime, double totalTime) override;
};

