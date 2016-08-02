#include "Biker.h"
#include "TienTest.h"
#include "Serial.h"

#include <VrLib/Texture.h>
#include <VrLib/math/HermiteCurve.h>
#include <VrLib/math/Ray.h>
#include <VrLib/tien/Scene.h>
#include <VrLib/tien/Node.h>
#include <VrLib/tien/components/Transform.h>
#include <VrLib/tien/components/Light.h>
#include <VrLib/tien/components/Camera.h>
#include <VrLib/tien/components/TerrainRenderer.h>
#include <VrLib/tien/components/RigidBody.h>
#include <VrLib/tien/components/TerrainCollider.h>
#include <VrLib/tien/components/ModelRenderer.h>
#include <VrLib/tien/components/MeshRenderer.h>
#include <VrLib/tien/components/AnimatedModelRenderer.h>
#include <VrLib/tien/components/DynamicSkyBox.h>
#include <VrLib/tien/components/TransformAttach.h>
#include <VrLib/json.h>
CSerial serial;
std::string buffer;
float speed;

Biker::Biker()
{
}


Biker::~Biker()
{
}

void Biker::init(vrlib::tien::Scene & scene, TienTest * app)
{
	serial.Open(5, 9600);
	Sleep(100);
	serial.SendData("ST\r\n", 4);


	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Sunlight", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 1, 1)));
		vrlib::tien::components::Light* light = new vrlib::tien::components::Light();
		light->color = glm::vec4(1, 1, 0.8627f, 1);
		light->intensity = 20.0f;
		light->type = vrlib::tien::components::Light::Type::directional;
		light->shadow = vrlib::tien::components::Light::Shadow::shadowmap;
		n->addComponent(light);
	}

	{
		float scale = 1;// .075f;
		terrain = new vrlib::tien::Terrain("data/TienTest/biker/Textures/heightmap.png", 50.0f);

		vrlib::tien::Node* n = new vrlib::tien::Node("Terrain", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(-terrain->getWidth() / 2.0f * scale, -15, -terrain->getHeight() / 2.0f* scale), glm::quat(), glm::vec3(scale, scale, scale)));
		auto terrainRenderer = new vrlib::tien::components::TerrainRenderer(terrain);
		terrainRenderer->addMaterialLayer(	"data/TienTest/biker/textures/grass_diffuse.png", 
											"data/TienTest/biker/textures/grass_normal.png", 
											"data/TienTest/biker/textures/grass_mask.png");
		terrainRenderer->addMaterialLayer(	"data/TienTest/biker/textures/ground_diffuse.png", 
											"data/TienTest/biker/textures/ground_normal.png", 
											"data/TienTest/biker/textures/ground_mask.png");
		n->addComponent(terrainRenderer);
		n->addComponent(new vrlib::tien::components::TerrainCollider(n));
		n->addComponent(new vrlib::tien::components::RigidBody(0));

		vrlib::tien::Node* n2 = new vrlib::tien::Node("Grass", &scene);
		n2->addComponent(new vrlib::tien::components::Transform(glm::vec3(-terrain->getWidth() / 2.0f * scale, -15, -terrain->getHeight() / 2.0f* scale), glm::quat(), glm::vec3(scale, scale, scale)));
		n2->addComponent(new GrassComponent(*terrain));

	}


	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Bike", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(-0, 0, 0), glm::quat(glm::vec3(0,-glm::half_pi<float>(), 0)), glm::vec3(1, 1, 1)));
		n->addComponent(new vrlib::tien::components::AnimatedModelRenderer("data/TienTest/biker/models/bike/bikeAnimTest.fbx"));
		n->addComponent(new vrlib::tien::components::RigidBody(0));
		n->getComponent<vrlib::tien::components::AnimatedModelRenderer>()->playAnimation("Armature|ArmatureAction.001");
		bike = n;
	}
	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Main Camera", bike);
		n->addComponent(new vrlib::tien::components::Transform(-bike->transform->position, glm::inverse(bike->transform->rotation), 1.0f / bike->transform->scale));
		n->addComponent(new vrlib::tien::components::Camera());
		n->addComponent(new vrlib::tien::components::DynamicSkyBox());
	}



	{
		vrlib::tien::Node* n = new vrlib::tien::Node("LeftHand", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0)));
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/vrlib/rendermodels/vr_controller_vive_1_5/vr_controller_vive_1_5.obj"));
		n->addComponent(new vrlib::tien::components::TransformAttach(app->vive.controllers[1].transform));
	}
	{
		vrlib::tien::Node* n = new vrlib::tien::Node("RightHand", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0)));
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/vrlib/rendermodels/vr_controller_vive_1_5/vr_controller_vive_1_5.obj"));
		n->addComponent(new vrlib::tien::components::TransformAttach(app->vive.controllers[0].transform));
	}

	route.addNode(glm::vec2(50, -50),	glm::vec2(0, 200));
	route.addNode(glm::vec2(50, 50),	glm::vec2(-200, 0));
	route.addNode(glm::vec2(-50, 50),	glm::vec2(0, -200));
	route.addNode(glm::vec2(-50, -50),	glm::vec2(200, 0));
	route.finish();

	auto getPos = [&scene,this](const glm::vec2 &p)
	{
	/*	float h = 0;
		scene.castRay(vrlib::math::Ray(glm::vec3(p.x, 50, p.y+1) * (128 / 126.0f), glm::vec3(0, -1, 0)), [&h](vrlib::tien::Node* node, const glm::vec3 &hitPosition, const glm::vec3 &hitNormal)
		{
			h = hitPosition.y + 0.05f;
			return false;
		});
		return glm::vec3(p.x, h, p.y);
		*/
		return terrain->getPosition(p + glm::vec2(128, 128)) + glm::vec3(-128, -14.85f, -128);

	};

	for (int i = 0; i < 100; i++)
	{
		int tree = rand() % 8;
		if (tree == 0)
			tree = rand() % 8;
		if (tree == 0)
			tree = rand() % 8;
		if (tree == 0)
			tree = 10;


		float scale = 0.9f + ((float)rand() / RAND_MAX) * 0.2f;

		vrlib::tien::Node* n = new vrlib::tien::Node("Tree", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(-50, 0, 0), glm::quat(), glm::vec3(1, 1, 1)));
		n->addComponent(new vrlib::tien::components::ModelRenderer("data/TienTest/biker/models/trees/2/tree"+std::to_string(tree)+".obj"));
		n->transform->position = getPos(glm::vec2((rand() % 100) - 50, (rand() % 100) - 50)) + glm::vec3(0,-0.25f,0);
	}

	glm::vec2 startPos = route.getPosition(0);
	bike->transform->position = getPos(startPos);

	{
		auto mesh = new vrlib::tien::components::MeshRenderer::Mesh();
		mesh->material.texture = vrlib::Texture::loadCached("data/TienTest/biker/textures/tarmac_diffuse.png");
		mesh->material.normalmap = vrlib::Texture::loadCached("data/TienTest/biker/textures/tarmac_normal.png");



		float inc = 0.1f;
		glm::vec2 lastPos = route.getPosition(0);
		vrlib::gl::VertexP3N2B2T2T2 v;
		vrlib::gl::setN3(v, glm::vec3(0, 1, 0));

		float t = 0;
		int count = 0;

		for (float f = inc; f < route.length; f += inc)
		{
			int i = mesh->vertices.size();

			glm::vec2 pos = route.getPosition(f);
			glm::vec2 diff = lastPos - pos;

			glm::vec2 angled = glm::normalize(glm::vec2(diff.y, -diff.x));

			for (float ff = -2.5f; ff <= 2.5f; ff+=0.1f)
			{
				vrlib::gl::setP3(v, getPos(pos + angled * ff));			vrlib::gl::setT2(v, glm::vec2(t, ff));			mesh->vertices.push_back(v);
				if(count <= 0)
					count--;
			}
			if (count < 0)
				count = -count;

			lastPos = pos;
			t += inc * 1.0f;
		}
		for (int i = 0; i < mesh->vertices.size() / count; i++)
		{
			for (int ii = 0; ii < count - 1; ii++)
			{
				mesh->indices.push_back((count * i + ii + 0) % mesh->vertices.size());
				mesh->indices.push_back((count * i + ii + 1) % mesh->vertices.size());
				mesh->indices.push_back((count * i + ii + count) % mesh->vertices.size());

				mesh->indices.push_back((count * i + ii + 1) % mesh->vertices.size());
				mesh->indices.push_back((count * i + ii + count+1) % mesh->vertices.size());
				mesh->indices.push_back((count * i + ii + count) % mesh->vertices.size());
			}
		}



		vrlib::tien::Node* n = new vrlib::tien::Node("Road", &scene);
		n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0,0,0)));
		n->addComponent(new vrlib::tien::components::MeshRenderer(mesh));
	}
	glm::vec2 pos = route.getPosition(position);
	glm::vec2 dir = route.getPosition(position + 0.05f) - pos;
	bike->transform->rotation = glm::quat(glm::vec3(0, glm::pi<float>() - glm::atan(dir.y, dir.x), 0));


}

void Biker::update(float frameTime, vrlib::tien::Scene & scene, TienTest * app)
{
	time += frameTime;

	if (serial.ReadDataWaiting())
	{
		char buf[1024];
		int rc = serial.ReadData(buf, 1024);
		buffer += std::string(buf, rc);
		while (buffer.find("\n") != std::string::npos)
		{
			std::string line = buffer.substr(0, buffer.find("\n"));
			buffer = buffer.substr(buffer.find("\n") + 1);
			serial.SendData("ST\r\n", 4);
			if (line[0] == 'E')
				continue;
			line = line.substr(line.find("\t") + 1);
			line = line.substr(line.find("\t") + 1);
			line = line.substr(0, line.find("\t"));
			speed = (float)(std::stof(line) / 10.0f);
		}
	}

	//if (!serial.IsOpened())
	//	speed = 50.0f;

	if (app->vive.controllers[0].triggerButton.getData() == vrlib::DigitalState::ON)
		speed = 10;
	if (app->vive.controllers[0].triggerButton.getData() == vrlib::DigitalState::TOGGLE_OFF)
		speed = 0;

	//speed = 10.0f;

	float dist = glm::pow(speed, 1.75f) * frameTime * 0.05f;
	
	if (dist > 0)
	{
		float inc = dist;

		glm::vec2 pos = route.getPosition(position);
		glm::vec2 dir;
		dir = route.getPosition(position + inc) - pos;

		float f = 1.1f;

		while (glm::abs(glm::length(dir) - dist) > 0.0001f)
		{
			float l = glm::length(dir) - dist;
			if (l > 0)
				inc /= f;
			else
				inc *= f;
			dir = route.getPosition(position + inc) - pos;
			f = 1 + (f - 1) * .9f;
		}

		glm::vec2 diff(bike->transform->position.x - pos.x, bike->transform->position.z - pos.y);
		float len = glm::length(diff);

		bike->transform->position.x = pos.x;
		bike->transform->position.z = pos.y;
		glm::vec3 up;
		scene.castRay(vrlib::math::Ray(glm::vec3(pos.x, 100, pos.y), glm::vec3(0, -1, 0)),
			[this, &up](vrlib::tien::Node* node, const glm::vec3 &hitPosition, const glm::vec3 &hitNormal)
		{
			bike->transform->position.y = hitPosition.y + 0.1f;
			up = glm::normalize(hitNormal);
			return true;
		});

		glm::quat newRot(glm::vec3(0, 1, 0), up);
		newRot = newRot * glm::quat(glm::vec3(0, glm::pi<float>() - glm::atan(dir.y, dir.x), 0));


		bike->transform->rotation = glm::slerp(bike->transform->rotation, newRot, 0.01f);





	//	bike->transform->rotation = glm::slerp(bike->transform->rotation, glm::quat(glm::vec3(0, glm::pi<float>() - glm::atan(dir.y, dir.x), 0)), 0.001f);



		float fac = 1;
		if (len != 0)
		{
			fac = 1 - len;
		}
		position += inc;
	}
	bike->getComponent<vrlib::tien::components::AnimatedModelRenderer>()->animationSpeed = speed / 20.0f;
}


void Route::addNode(const glm::vec2 &position, const glm::vec2 &direction)
{
	nodes.push_back(std::make_tuple(position, direction, 0.0f));
}

void Route::finish()
{
	length = 0;
	for (size_t i = 0; i < nodes.size(); i++)
	{
		int i1 = i;
		int i2 = (i + 1) % nodes.size();
		vrlib::math::HermiteCurve hermiteCurve(std::get<0>(nodes[i1]), std::get<1>(nodes[i1]), std::get<0>(nodes[i2]), std::get<1>(nodes[i2]));
		std::get<2>(nodes[i1]) = hermiteCurve.getLength();
		length += std::get<2>(nodes[i1]);
	}

}

glm::vec2 Route::getPosition(float index)
{
	while (index > length)
		index -= length;

	int i = 0;
	float t = 0;
	while (index > t + std::get<2>(nodes[i]))
	{
		t += std::get<2>(nodes[i]);
		i++;
	}
	index -= t;


	int i1 = (int)glm::floor(i);
	int i2 = (i1 + 1) % nodes.size();

	float fract = index / std::get<2>(nodes[i1]);



	vrlib::math::HermiteCurve hermiteCurve(std::get<0>(nodes[i1]), std::get<1>(nodes[i1]), std::get<0>(nodes[i2]), std::get<1>(nodes[i2]));
	return hermiteCurve.getPoint(fract);


}




GrassComponent::GrassComponent(vrlib::tien::Terrain &terrain) : terrain(terrain)
{
	renderContext = GrassRenderContext::getInstance();
	
	grassTexture = vrlib::Texture::loadCached("data/TienTest/biker/textures/texture_grass.jpg");

	std::vector<vrlib::gl::VertexP3> vertices;

	int gridSize = 4;

	for (int x = 0; x < terrain.getWidth() - gridSize; x+= gridSize)
	{
		for (int y = 0; y < terrain.getHeight() - gridSize; y+= gridSize)
		{
			vrlib::gl::VertexP3 v;
			vrlib::gl::setP3(v, glm::vec3(x, terrain[x][y], y));
			vertices.push_back(v);

			vrlib::gl::setP3(v, glm::vec3(x, terrain[x][y + gridSize], y + gridSize));
			vertices.push_back(v);

			vrlib::gl::setP3(v, glm::vec3(x + gridSize, terrain[x + gridSize][y + gridSize], y + gridSize));
			vertices.push_back(v);

			vrlib::gl::setP3(v, glm::vec3(x + gridSize, terrain[x + gridSize][y], y));
			vertices.push_back(v);
		}
	}


	vbo.bind();
	vbo.setData(vertices.size(), &vertices[0], GL_STATIC_DRAW);
	vao = new vrlib::gl::VAO(&vbo);
}


void GrassComponent::update(float elapsedTime, vrlib::tien::Scene& scene)
{
	time += elapsedTime;
}


void GrassComponent::draw()
{
	glDisable(GL_CULL_FACE);
	vrlib::tien::components::Transform* t = node->getComponent<vrlib::tien::components::Transform>();

	GrassRenderContext* context = dynamic_cast<GrassRenderContext*>(renderContext);
	context->renderShader->use();
	context->renderShader->setUniform(GrassRenderContext::RenderUniform::modelMatrix, t->globalTransform);
	context->renderShader->setUniform(GrassRenderContext::RenderUniform::normalMatrix, glm::transpose(glm::inverse(glm::mat3(t->globalTransform))));
	context->renderShader->setUniform(GrassRenderContext::RenderUniform::modelViewProjectionMatrix, context->viewprojection * t->globalTransform);
	context->renderShader->setUniform(GrassRenderContext::RenderUniform::time, time);
	grassTexture->bind();
	vao->bind();
	glDrawArrays(GL_QUADS, 0, vbo.getLength());
	vao->unBind();

	glEnable(GL_CULL_FACE);

}

void GrassComponent::drawShadowMap()
{
}

vrlib::json::Value GrassComponent::toJson() const
{
	vrlib::json::Value ret;
	ret["type"] = "grass";
	return ret;
}


void GrassComponent::GrassRenderContext::frameSetup(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix)
{
	renderShader->use();
	renderShader->setUniform(RenderUniform::projectionMatrix, projectionMatrix);
	renderShader->setUniform(RenderUniform::viewMatrix, viewMatrix);
	viewprojection = projectionMatrix * viewMatrix;
}


void GrassComponent::GrassRenderContext::init()
{
	renderShader = new vrlib::gl::Shader<RenderUniform>("data/TienTest/biker/shaders/grass.vert", "data/TienTest/biker/shaders/grass.frag", "data/TienTest/biker/shaders/grass.geom");
	renderShader->bindAttributeLocation("a_position", 0);
	renderShader->bindAttributeLocation("a_normal", 1);
	renderShader->bindAttributeLocation("a_bitangent", 2);
	renderShader->bindAttributeLocation("a_tangent", 3);
	renderShader->bindAttributeLocation("a_texture", 4);
	renderShader->link();
	renderShader->bindFragLocation("fragColor", 0);
	renderShader->bindFragLocation("fragNormal", 1);
	//shader->bindFragLocation("fragColor", 0);
	renderShader->registerUniform(RenderUniform::modelMatrix, "modelMatrix");
	renderShader->registerUniform(RenderUniform::projectionMatrix, "projectionMatrix");
	renderShader->registerUniform(RenderUniform::viewMatrix, "viewMatrix");
	renderShader->registerUniform(RenderUniform::normalMatrix, "normalMatrix");
	renderShader->registerUniform(RenderUniform::s_texture, "s_texture");
	renderShader->registerUniform(RenderUniform::modelViewProjectionMatrix, "modelViewProjectionMatrix");
	renderShader->registerUniform(RenderUniform::time, "time");
	renderShader->use();
	renderShader->setUniform(RenderUniform::s_texture, 0);
}