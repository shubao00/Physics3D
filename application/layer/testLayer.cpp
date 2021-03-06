#include "core.h"

#include "GL/glew.h"

#include "testLayer.h"
#include "view/screen.h"

#include "../graphics/renderer.h"
#include "../graphics/shader/shader.h"
#include "../graphics/mesh/indexedMesh.h"
#include "../graphics/buffers/frameBuffer.h"
#include "../graphics/buffers/vertexBuffer.h"
#include "../graphics/buffers/bufferLayout.h"

#include "../graphics/resource/textureResource.h"
#include "../engine/resource/meshResource.h"
#include "../util/resource/resourceManager.h"
#include "../application/ecs/light.h"
#include "application.h"
#include "shader/shaders.h"
#include "input/standardInputHandler.h"
#include "worlds.h"
#include "../physics/math/mathUtil.h"

namespace Application {

struct Uniform {
	Mat4f model;
	Vec3f albedo;
	Vec3f mrao;
};

std::vector<Light*> llights;
std::vector<Uniform> uniforms;
MeshResource* mesh;

void TestLayer::onInit() {
	float size = 7.0f;
	float space = 2.5f;
	frepeat(row, size) {
		frepeat(col, size) {
			frepeat(z, 1) {
				uniforms.push_back(
					{
						Mat4f {
							1, 0, 0, (col - size / 2.0f) * space,
							0, 1, 0, 20+(row - size / 2.0f) * space,
							0, 0, 1, 0,
							0, 0, 0, 1,
						},
						Vec3f(0.5, 0, 0),
						Vec3f(row / size, col / size + 0.05f, 1.0f),
					}
				);
			}
		}
	}

	BufferLayout layout = BufferLayout(
		{
			BufferElement("vModelMatrix", BufferDataType::MAT4, true),
			BufferElement("vAlbedo", BufferDataType::FLOAT3, true),
			BufferElement("vMRAo", BufferDataType::FLOAT3, true),
		}
	);
	VertexBuffer* vbo = new VertexBuffer(uniforms.data(), uniforms.size() * sizeof(Uniform));

	//mesh = ResourceManager::add<MeshResource>("plane", "../res/models/plane.obj");
	mesh = ResourceManager::add<MeshResource>("sphere", "../res/models/sphere.obj");
	
	//mesh = ResourceManager::add<MeshResource>("sphere", "../res/models/sphere bot/sphere.obj");

	mesh->getMesh()->addUniformBuffer(vbo, layout);

	ResourceManager::add<TextureResource>("wall_color", "../res/textures/wall/wall_color.jpg");
	ResourceManager::add<TextureResource>("wall_normal", "../res/textures/wall/wall_normal.jpg");
	ResourceManager::add<TextureResource>("canvas_color", "../res/textures/canvas/canvas_color.png");
	ResourceManager::add<TextureResource>("canvas_normal", "../res/textures/canvas/canvas_normal.png");

	llights.push_back(new Light(Vec3f(-10,10,10), Color3(300), 1, { 1, 1, 1 }));
	llights.push_back(new Light(Vec3f(10,10,10), Color3(300), 1, { 1, 1, 1 }));
	llights.push_back(new Light(Vec3f(-10,-10,10), Color3(300), 1, { 1, 1, 1 }));
	llights.push_back(new Light(Vec3f(10,-10,10), Color3(300), 1, { 1, 1, 1 }));
	ApplicationShaders::lightingShader.updateLight(llights);
}

void TestLayer::onUpdate() {

}

void TestLayer::onEvent(Event& event) {
	if (event.getType() == EventType::KeyPress) {
		if (static_cast<KeyPressEvent&>(event).getKey() == Keyboard::TAB.code) {
			llights[0]->position = fromPosition(screen.camera.cframe.position);
			ApplicationShaders::lightingShader.updateLight(llights);
		}
	}
}

void TestLayer::onRender() {
	using namespace Graphics;
	using namespace Graphics::Renderer;

	Screen* screen = static_cast<Screen*>(this->ptr);

	beginScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	ApplicationShaders::lightingShader.bind();

	//ResourceManager::get<TextureResource>("canvas_color")->bind(0);
	//ResourceManager::get<TextureResource>("canvas_normal")->bind(1);

	//ApplicationShaders::lightingShader.updateTexture(false);
	ApplicationShaders::lightingShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix, screen->camera.cframe.position);

	mesh->getMesh()->renderInstanced(uniforms.size(), FILL);

	endScene();
}

void TestLayer::onClose() {

}

};