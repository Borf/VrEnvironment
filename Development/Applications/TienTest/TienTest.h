#pragma once

#include <VrLib/Application.h>
#include <VrLib/Device.h>
#include <VrLib/tien/Tien.h>
#include <VrLib/tien/Node.h>
#include <VrLib/HtcVive.h>
#include <list>
#include <vector>
class Demo;


class TienTest : public vrlib::Application
{
	Demo* demo;
public:
	vrlib::Vive	vive;
	vrlib::tien::Tien tien;

	TienTest();


	virtual void init() override;
	virtual void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix) override;
	virtual void preFrame(double frameTime, double totalTime) override;
};

