#pragma once

#include <vrlib/NormalApp.h>
#include <vrlib/tien/Tien.h>
#include <VrLib/math/Ray.h>
#include <VrLib/tien/components/MeshRenderer.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

namespace vrlib
{
	class Kernel;
	namespace tien
	{
		class Node;
	}
}

struct MouseState
{
	glm::ivec2 pos;
	glm::ivec2 lastClickPos;
	glm::ivec2 mouseDownPos;
	int clickCount = 1;
	DWORD lastClickTime;
	union
	{
		struct
		{
			bool left;
			bool middle;
			bool right;
		};
		bool buttons[3] = { false, false, false };
	};
};

class NormalModelViewer : public vrlib:: NormalApp
{
public:
	vrlib::Kernel* kernel;
	vrlib::tien::Tien tien;

	std::string fileName;

	enum class EditorShaderUniforms
	{
		projectionMatrix,
		modelViewMatrix,
		s_texture,
		textureFactor,
		color
	};
	vrlib::gl::Shader<EditorShaderUniforms>* shader;


	vrlib::tien::Node* cameraNode;
	vrlib::tien::Node* modelNode;

	glm::quat cameraRot;
	glm::quat cameraRotTo;
	glm::vec3 cameraPos;

	vrlib::math::Ray ray;

	NormalModelViewer(const std::string &filename);
	~NormalModelViewer();

	virtual void init() override;
	virtual void preFrame(double frameTime, double totalTime) override;

	virtual void draw() override;

	virtual void mouseMove(int x, int y) override;
	virtual void mouseScroll(int offset) override;
	virtual void mouseUp(MouseButton button) override;
	virtual void mouseDown(MouseButton button) override;
	virtual void keyDown(int button) override;
	virtual void keyUp(int button) override;
	virtual void keyChar(char character) override;


	static MouseState mouseState;
	static MouseState lastMouseState;

};
