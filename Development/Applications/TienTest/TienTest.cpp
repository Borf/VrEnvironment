#include "TienTest.h"
#include "Demo.h"
#include "TowerDefense.h"
#include "VirtueelPD.h"

#include <GL/glew.h>

#include <VrLib/tien/Scene.h>

#include <VrLib/Log.h>
using vrlib::Log;
using vrlib::logger;



TienTest::TienTest()
{
	clearColor = glm::vec4(0.0f, 0.5f, 0.9f, 1.0f);
}

void TienTest::init()
{
	vive.init();
	tien.init();

	logger << "Initialized" << Log::newline;
	tien.start();
	demo = new TowerDefense();
	demo->init(tien.scene, this);
	logger << "Started" << Log::newline;
}


void TienTest::draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
{

//	tien.saveCubeMap(glm::vec3(0, 1.5f, 0), "towerdefense");



	tien.render(projectionMatrix, modelViewMatrix);
}

void TienTest::preFrame(double frameTime, double totalTime)
{
	demo->update((float)(frameTime / 1000.0f), tien.scene, this);

	tien.update((float)(frameTime / 1000.0f));
}
