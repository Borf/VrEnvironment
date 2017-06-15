#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include "NormalModelViewer.h"

#include <VrLib/gl/shader.h>
#include <vrlib/gl/FBO.h>
#include <vrlib/gl/Vertex.h>
#include <vrlib/Log.h>
#include <VrLib/Kernel.h>
#include <VrLib/math/aabb.h>
#include <VrLib/math/Plane.h>
#include <VrLib/util.h>
#include <VrLib/Texture.h>

#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>


#include <vrlib/tien/components/Camera.h>
#include <vrlib/tien/components/Transform.h>
#include <vrlib/tien/components/Light.h>
#include <vrlib/tien/components/StaticSkyBox.h>
#include <vrlib/tien/components/DynamicSkyBox.h>
#include <vrlib/tien/components/AnimatedModelRenderer.h>
#include <vrlib/tien/components/ModelRenderer.h>
#include <vrlib/tien/components/MeshRenderer.h>
#include <vrlib/tien/components/TerrainRenderer.h>
#include <VrLib/tien/components/RigidBody.h>
#include <VrLib/tien/components/BoxCollider.h>
#include <VrLib/tien/components/MeshCollider.h>
#include <VrLib/tien/components/SphereCollider.h>
#include <VrLib/tien/components/TerrainCollider.h>
#include <VrLib/tien/components/postprocess/Bloom.h>
#include <VrLib/tien/components/postprocess/Gamma.h>
#include <VrLib/tien/components/postprocess/DoF.h>

MouseState NormalModelViewer::mouseState;
MouseState NormalModelViewer::lastMouseState;


NormalModelViewer::NormalModelViewer(const std::string &fileName) : NormalApp("Model Viewer")
{
	this->fileName = fileName;
}


NormalModelViewer::~NormalModelViewer()
{
}






void NormalModelViewer::init()
{
	kernel = vrlib::Kernel::getInstance();
	//initialize Open dialog
	CoInitializeEx(NULL, COINIT_MULTITHREADED);


	tien.init();

	vrlib::tien::Node* sunlight;
	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Sunlight", &tien.scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0), glm::quat(glm::vec3(0,0,-glm::quarter_pi<float>()))));
		vrlib::tien::components::Light* light = new vrlib::tien::components::Light();
		light->color = glm::vec4(1, 1, 0.8627f, 1);
		light->intensity = 10.0f;
		light->directionalAmbient = 0.7f;
		light->type = vrlib::tien::components::Light::Type::directional;
		light->shadow = vrlib::tien::components::Light::Shadow::shadowmap;
		n->addComponent(light);
		sunlight = n;
	}

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Camera", &tien.scene);
		n->addComponent(new vrlib::tien::components::Transform());
		n->addComponent(new vrlib::tien::components::Camera());
		n->addComponent(new vrlib::tien::components::DynamicSkyBox());
		n->getComponent<vrlib::tien::components::DynamicSkyBox>()->light = sunlight;
		cameraNode = n;
	}

	{
		modelNode = new vrlib::tien::Node("Model", &tien.scene);
		modelNode->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0)));
		modelNode->addComponent(new vrlib::tien::components::ModelRenderer(fileName));
	}

	tien.scene.cameraNode = cameraNode;

	cameraPos = glm::vec3(0, 1.8f, 8.0f);
	tien.start();
}

void NormalModelViewer::preFrame(double frameTime, double totalTime)
{
	//camera movement
	if (mouseState.middle)
	{
		glm::vec3 cameraMovement(0, 0, 0);
		if (KeyboardDeviceDriver::isPressed(KeyboardButton::KEY_W))		cameraMovement.z = -1;
		if (KeyboardDeviceDriver::isPressed(KeyboardButton::KEY_S))		cameraMovement.z = 1;
		if (KeyboardDeviceDriver::isPressed(KeyboardButton::KEY_A))		cameraMovement.x = -1;
		if (KeyboardDeviceDriver::isPressed(KeyboardButton::KEY_D))		cameraMovement.x = 1;
		if (KeyboardDeviceDriver::isPressed(KeyboardButton::KEY_Q))		cameraMovement.y = 1;
		if (KeyboardDeviceDriver::isPressed(KeyboardButton::KEY_Z))		cameraMovement.y = -1;

		cameraMovement *= frameTime / 400.0f;
		if (KeyboardDeviceDriver::isModPressed(KeyboardModifiers::KEYMOD_SHIFT))
			cameraMovement *= 10.0f;


		cameraPos += cameraMovement * cameraRot;

		cameraRot = cameraRot * glm::quat(glm::vec3(0, .01f * (mouseState.pos.x - lastMouseState.pos.x), 0));
		cameraRot = glm::quat(glm::vec3(.01f * (mouseState.pos.y - lastMouseState.pos.y), 0, 0)) * cameraRot;
	}

	tien.update((float)(frameTime / 1000.0f));


	lastMouseState = mouseState;
}


void NormalModelViewer::draw()
{
	glViewport(0, 0, kernel->getWindowWidth(), kernel->getWindowHeight());

	glm::mat4 projectionMatrix = glm::perspective(70.0f, this->kernel->getWindowWidth() / (float)this->kernel->getWindowHeight(), 0.01f, 500.0f);

	glm::mat4 cameraMat = glm::translate(glm::toMat4(cameraRot), -cameraPos);

	tien.render(projectionMatrix, cameraMat);




}

void NormalModelViewer::mouseMove(int x, int y)
{
	mouseState.pos.x = x;
	mouseState.pos.y = y;
}

void NormalModelViewer::mouseScroll(int offset)
{
}


void NormalModelViewer::mouseDown(MouseButton button)
{
	mouseState.mouseDownPos = mouseState.pos;
	mouseState.buttons[(int)button] = true;

}

void NormalModelViewer::keyDown(int button)
{
	NormalApp::keyDown(button);
}

void NormalModelViewer::keyUp(int button)
{
	NormalApp::keyUp(button);


}

void NormalModelViewer::keyChar(char character)
{
	NormalApp::keyChar(character);
}

void NormalModelViewer::mouseUp(MouseButton button)
{
	mouseState.buttons[(int)button] = false;

}
