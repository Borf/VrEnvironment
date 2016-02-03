#include "HelloWorld.h"

#include <gl/glew.h>

#include <VrLib/Log.h>
using vrlib::Log;
using vrlib::logger;


HelloWorld::HelloWorld()
{
	clearColor = glm::vec4(0.0f, 0.5f, 0.9f, 1.0f);
}

void HelloWorld::init()
{
	leftButton.init("LeftButton");
	logger << "Initialized" << Log::newline;
}

void HelloWorld::draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
{
	if (leftButton.getData() == vrlib::DigitalState::OFF)
	{
		glBegin(GL_QUADS);
		glVertex3f(0, 0, 0);
		glVertex3f(1, 0, 0);
		glVertex3f(1, 1, 0);
		glVertex3f(0, 1, 0);
		glEnd();
	}
}

void HelloWorld::preFrame(double frameTime, double totalTime)
{

}


