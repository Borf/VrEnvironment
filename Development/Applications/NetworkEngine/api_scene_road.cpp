#include "api.h"

#include "Route.h"
#include "NetworkEngine.h"

#include <VrLib/tien/components/TerrainRenderer.h>
#include <VrLib/tien/components/MeshRenderer.h>
#include <VrLib/tien/components/Transform.h>
#include <VrLib/tien/Terrain.h>
#include <VrLib/Texture.h>

Api scene_road_add("scene/road/add", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, vrlib::json::Value &data)
{
	float heightOffset = 0.01f;
	if (data.isMember("heightoffset"))
		heightOffset = data["heightoffset"];

	for (size_t i = 0; i < engine->routes.size(); i++)
	{
		if (engine->routes[i]->id == data["route"].asString())
		{
			vrlib::tien::Node* terrainRenderingNode = engine->tien.scene.findNodeWithComponent<vrlib::tien::components::TerrainRenderer>();

			const Route& route = *engine->routes[i];
			auto getPos = [&engine, &terrainRenderingNode](const glm::vec2 &p)
			{
				if (!engine->terrain)
					return glm::vec3(p.x, 0.01, p.y);
				glm::vec2 offset(0, 0);
				if (terrainRenderingNode)
					offset = glm::vec2(terrainRenderingNode->transform->position.x, terrainRenderingNode->transform->position.z);

				return engine->terrain->getPosition(p - offset) + glm::vec3(offset.x, 0.01f, offset.y);

			};

			{
				auto mesh = new vrlib::tien::components::MeshRenderer::Mesh();
				mesh->material.texture = vrlib::Texture::loadCached("data/NetworkEngine/textures/tarmac_diffuse.png");
				mesh->material.normalmap = vrlib::Texture::loadCached("data/NetworkEngine/textures/tarmac_normal.png");



				float inc = 0.1f;
				glm::vec2 lastPos = route.getPositionXY(0);
				vrlib::gl::VertexP3N2B2T2T2 v;
				vrlib::gl::setN3(v, glm::vec3(0, 1, 0));
				vrlib::gl::setTan3(v, glm::vec3(1, 0, 0));
				vrlib::gl::setBiTan3(v, glm::vec3(0, 0, 1));

				float t = 0;
				int count = 0;

				for (float f = inc; f < route.length; f += inc)
				{
					int i = mesh->vertices.size();

					glm::vec2 pos = route.getPositionXY(f);
					glm::vec2 diff = lastPos - pos;

					glm::vec2 angled = glm::normalize(glm::vec2(diff.y, -diff.x));

					for (float ff = -2.5f; ff <= 2.5f; ff += 0.1f)
					{
						vrlib::gl::setP3(v, getPos(pos + angled * ff));			vrlib::gl::setT2(v, glm::vec2(t, ff));			mesh->vertices.push_back(v);
						if (count <= 0)
							count--;
					}
					if (count < 0)
						count = -count;

					lastPos = pos;
					t += inc * 1.0f;
				}
				for (size_t i = 0; i < mesh->vertices.size() / count; i++)
				{
					for (int ii = 0; ii < count - 1; ii++)
					{
						mesh->indices.push_back((count * i + ii + 0) % mesh->vertices.size());
						mesh->indices.push_back((count * i + ii + 1) % mesh->vertices.size());
						mesh->indices.push_back((count * i + ii + count) % mesh->vertices.size());

						mesh->indices.push_back((count * i + ii + 1) % mesh->vertices.size());
						mesh->indices.push_back((count * i + ii + count + 1) % mesh->vertices.size());
						mesh->indices.push_back((count * i + ii + count) % mesh->vertices.size());
					}
				}


				for (int i = 0; i < mesh->indices.size(); i+=3)
				{
					glm::vec3 normal(0,1,0), tan(1,0,0), bitan(0,0,1);

					


					for (int ii = 0; ii < 3; ii++)
					{
						vrlib::gl::setN3(mesh->vertices[mesh->indices[i+ii]], normal);
						vrlib::gl::setTan3(mesh->vertices[mesh->indices[i + ii]], tan);
						vrlib::gl::setBiTan3(mesh->vertices[mesh->indices[i + ii]], bitan);
					}
				}



				vrlib::tien::Node* n = new vrlib::tien::Node("Road", &engine->tien.scene);
				n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0)));
				n->addComponent(new vrlib::tien::components::MeshRenderer(mesh));
			}



			vrlib::json::Value packet;
			packet["id"] = "scene/road/add";
			packet["data"]["status"] = "ok";
			tunnel->send(packet);
			return;
		}
	}
	vrlib::json::Value packet;
	packet["id"] = "route/delete";
	packet["data"]["status"] = "error";
	packet["data"]["error"] = "Route not found";
	tunnel->send(packet);






});

Api scene_road_delete("scene/road/delete", [](NetworkEngine* engine, vrlib::Tunnel* tunnel, vrlib::json::Value &data)
{
	sendError(tunnel, "scene/road/delete", "Not implemented");
});
