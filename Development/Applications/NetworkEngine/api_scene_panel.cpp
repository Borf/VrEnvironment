#include "api.h"
#include "PanelComponent.h"

#include <glm/gtc/matrix_transform.hpp>

#include <VrLib/tien/components/DynamicSkyBox.h>


Api scene_panel_clear("scene/panel/clear", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
{
	vrlib::tien::Node* node = engine->tien.scene.findNodeWithGuid(data["id"]);
	assert(node);
	PanelComponent* panel = node->getComponent<PanelComponent>();
	assert(panel);

	panel->clear();

});


Api scene_panel_swap("scene/panel/swap", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
{
	vrlib::tien::Node* node = engine->tien.scene.findNodeWithGuid(data["id"]);
	assert(node);
	PanelComponent* panel = node->getComponent<PanelComponent>();
	assert(panel);

	panel->swap();

});

Api scene_panel_drawlines("scene/panel/drawlines", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
{
	vrlib::tien::Node* node = engine->tien.scene.findNodeWithGuid(data["id"]);
	assert(node);

	PanelComponent* panel = node->getComponent<PanelComponent>();



	std::vector<vrlib::gl::VertexP3C4> verts;
	for (size_t i = 0; i < data["lines"].size(); i++)
	{
		glm::vec4 color(data["lines"][i][4].asFloat(),
						data["lines"][i][5].asFloat(),
						data["lines"][i][6].asFloat(),
						data["lines"][i][7].asFloat());

		verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(data["lines"][i][0].asFloat(), data["lines"][i][1].asFloat(), 0), color));
		verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(data["lines"][i][2].asFloat(), data["lines"][i][3].asFloat(), 0), color));
	}
	
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, panel->backFbo->getWidth(), panel->backFbo->getHeight());
	panel->backFbo->bind();

	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	engine->debugShader->use();
	engine->debugShader->setUniform(NetworkEngine::DebugUniform::projectionMatrix, glm::ortho(0.0f, 512.0f, 512.0f, 0.0f));
	engine->debugShader->setUniform(NetworkEngine::DebugUniform::modelViewMatrix, glm::mat4());

	vrlib::gl::setAttributes<vrlib::gl::VertexP3C4>(&verts[0]);
	glLineWidth(data["width"].asFloat());
	glDrawArrays(GL_LINES, 0, verts.size());
	glEnable(GL_DEPTH_TEST);



	panel->backFbo->unbind();
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

	vrlib::json::Value packet;
	packet["id"] = "scene/panel/drawlines";
	packet["data"]["status"] = "ok";
	tunnel->send(packet);
});


Api scene_panel_drawtext("scene/panel/drawtext", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, const vrlib::json::Value &data)
{
	vrlib::tien::Node* node = engine->tien.scene.findNodeWithGuid(data["id"]);
	assert(node);
	PanelComponent* panel = node->getComponent<PanelComponent>();
	assert(panel);

	panel->clear();

});