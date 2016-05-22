#pragma once

#include <VrLib/Application.h>
#include <VrLib/Device.h>
#include <VrLib/tien/Renderer.h>
#include <list>

class TienTest : public vrlib::Application
{
	vrlib::DigitalDevice leftButton;

	vrlib::tien::Renderer renderer;

public:
	TienTest();


	virtual void init() override;
	virtual void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix) override;
	virtual void preFrame(double frameTime, double totalTime) override;
};

