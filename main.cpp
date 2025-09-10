#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
// region Global Include (Must be the first!)
#define STB_IMAGE_IMPLEMENTATION
#include "GlobalDefines.h"
// endregion Global Include

#include "Camera.h"
#include "ECS/Components/MeshRenderer.h"
#include "ECS/Components/PlayerController.h"
#include "ECS/Components/Transform.h"
#include "ECS/Registry.h"
#include "ECS/SystemManager.h"
#include "ECS/Systems/PlayerControlSystem.h"
#include "ECS/Systems/RenderSystem.h"
#include "Input/Keyboard.h"
#include "Lights/PointLight.h"
#include "Model.h"
#include "Resources/ResourceManager.h"
#include "Shader.h"
#include "Skybox.h"
#include "StorageBufferDynamicArray.h"
#include "Window.h"
#include "imgui.h"

#include <csignal>
#include <iostream>

#define RGBCOLOR(r, g, b) glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f)

float deltaTime, lastTime;
float red = 0.0f;
bool enableCursorEvent = true;
bool fullscreenEvent;
bool enableCursor = true;
bool enableGrid = true;
bool gridMode = false;

float fpsCounter = 0;
float fpsCount = 0;
float fps = 0;

Input::Keyboard &keyboard = *Input::Keyboard::GetInstance();
Input::Mouse &mouse = *Input::Mouse::GetInstance();
Resources::ResourceManager &resources = *Resources::ResourceManager::GetInstance();

GLuint particleVAO;

struct Uniforms
{
    GLint model = 0;
};

Uniforms uniforms{};

// TODO : Do this in a better way... (Grid exclusive class)
struct SingleMesh
{
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ibo = 0;
    GLint indexCount = 0;

    void Render() const
    {
        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};

SingleMesh plane;

void CreateSimpleMeshes()
{
    // clang-format off
	 constexpr float planeVertices[] = {
		-1.0, 0.0, -1.0,
		1.0, 0.0, -1.0,
		1.0, 0.0, 1.0,
		-1.0, 0.0, 1.0,
	};

	const unsigned int planeIndices[] = {0, 2, 1, 2, 0, 3};
    // clang-format on

    glGenVertexArrays(1, &plane.vao);
    glBindVertexArray(plane.vao);

    glGenBuffers(1, &plane.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, plane.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &plane.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices), &planeIndices, GL_STATIC_DRAW);

    plane.indexCount = 6;

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, static_cast<void *>(nullptr));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ConfigureKeys(Window &window)
{
    keyboard
        .AddCallback(GLFW_KEY_ESCAPE, [&window]() -> void
                     {
                         window.SetShouldClose(true);
                     })
        .AddCallback(GLFW_KEY_T, [&window]() -> void
                     {
                         if (enableCursorEvent) return;
                         enableCursorEvent = true;
                         enableCursor = !enableCursor;
                         mouse.ToggleMouse(enableCursor);
                         window.SetMouseStatus(enableCursor);
                     })
        .AddCallback(GLFW_KEY_F11, [&window]() -> void
                     {
                         if (fullscreenEvent) return;
                         fullscreenEvent = true;
                         window.ToggleFullscreen();
                     });
}

int main()
{
    Window window(1280, 720, "OpenGL Playground");

    if (!window.Init())
    {
        std::cerr << "Cannot initialize window.\n";
        return 1;
    }

    ECS::Registry registry;
    ECS::SystemManager systemManager;

    registry.RegisterComponent<ECS::Components::Transform>();
    registry.RegisterComponent<ECS::Components::MeshRenderer>();
    registry.RegisterComponent<ECS::Components::PlayerController>();
    systemManager.RegisterSystem<ECS::Systems::RenderSystem>();
    systemManager.RegisterSystem<ECS::Systems::PlayerControlSystem>();

    resources.ScanResources();
    Resources::ResourceManager::InitDefaultResources();

    auto particleTexture = resources.GetTexture("particle.png");

    Model turret("./assets/models/turret/source/turret_model.fbx");
    // Model pod("./assets/models/pod/source/pod.fbx");
    Model twob("./assets/models/2b/2b.fbx");

    // clang-format off
	Skybox skybox({
	    "./assets/textures/skybox/px.png",
	    "./assets/textures/skybox/nx.png",
	    "./assets/textures/skybox/py.png",
	    "./assets/textures/skybox/ny.png",
	    "./assets/textures/skybox/pz.png",
	    "./assets/textures/skybox/nz.png"
	});
    // clang-format on
    skybox.Load();

    Shader shader = *resources.GetShader("base");
    Shader skyboxShader = *resources.GetShader("skybox_shader");
    Shader blurShader = *resources.GetShader("blur");
    Shader sharpShader = *resources.GetShader("sharp");
    Shader grayscaleShader = *resources.GetShader("grayscale");
    Shader gridShader = *resources.GetShader("infinite_grid");

    Framebuffer blurFramebuffer(blurShader, window.GetWidth(), window.GetHeight());
    Framebuffer sharpFramebuffer(sharpShader, window.GetWidth(), window.GetHeight());
    Framebuffer grayscaleFramebuffer(grayscaleShader, window.GetWidth(), window.GetHeight());

    window.AddFramebuffer(&blurFramebuffer);
    window.AddFramebuffer(&sharpFramebuffer);
    window.AddFramebuffer(&grayscaleFramebuffer);

    // region Framebuffers
    enum Framebuffers
    {
        BLUR = 0,
        SHARP = 1,
        GRAYSCALE = 2
    };

    std::unordered_map<int, Framebuffer *> framebuffers;
    framebuffers[Framebuffers::BLUR] = &blurFramebuffer;
    framebuffers[Framebuffers::SHARP] = &sharpFramebuffer;
    framebuffers[Framebuffers::GRAYSCALE] = &grayscaleFramebuffer;

    const char *fbItems[]{"Blur", "Sharp", "Grayscale"};
    int fbSelectedItem = 0;
    bool enableEffects = false;
    Framebuffer *selectedEffect = framebuffers[Framebuffers::BLUR];
    // endregion

    Camera camera({2.0f, 2.0f, 2.0f}, {0.0f, 1.0f, 0.0f});
    camera.SetInput(Input::Keyboard::GetInstance(), Input::Mouse::GetInstance());
    // camera.SetMoveSpeed(1.5f);
    // camera.SetTurnSpeed(1.0f);

    float cameraMoveSpeed = 150.0f;
    float cameraTurnSpeed = 75.0f;

    camera.SetMoveSpeed(cameraMoveSpeed);
    camera.SetTurnSpeed(cameraTurnSpeed);

    mouse.ToggleMouse(enableCursor);
    window.SetMouseStatus(enableCursor);

    StorageBufferDynamicArray<Lights::PointLight> pointLights(3);
    pointLights.Add({.position = {2.0f, 2.0f, 2.0f, 0.0f},
                     .ambient = {0.1f, 0.1f, 0.1f, 0.0f},
                     .diffuse = {1.0f, 0.0f, 0.0f, 0.0f},
                     .specular = {1.0f, 1.0f, 1.0f, 0.0f},
                     .constant = 1.0f,
                     .linear = 0.09f,
                     .quadratic = 0.032f,
                     .isTurnedOn = true});

    pointLights.Add({.position = {-2.0f, 2.0f, -2.0f, 0.0f},
                     .ambient = {0.1f, 0.1f, 0.1f, 0.0f},
                     .diffuse = {0.0f, 1.0f, 0.0f, 0.0f},
                     .specular = {1.0f, 1.0f, 1.0f, 0.0f},
                     .constant = 1.0f,
                     .linear = 0.09f,
                     .quadratic = 0.032f,
                     .isTurnedOn = true});

    pointLights.Add({.position = {-2.0f, 2.0f, 2.0f, 0.0f},
                     .ambient = {0.1f, 0.1f, 0.1f, 0.0f},
                     .diffuse = {0.0f, 0.0f, 1.0f, 0.0f},
                     .specular = {1.0f, 1.0f, 1.0f, 0.0f},
                     .constant = 1.0f,
                     .linear = 0.09f,
                     .quadratic = 0.032f,
                     .isTurnedOn = true});

    ConfigureKeys(window);
    CreateSimpleMeshes();

    // region Entity creation

    ECS::Entity turretEntity = registry.CreateEntity();
    registry.AddComponent(turretEntity, ECS::Components::Transform{});
    registry.AddComponent(turretEntity, ECS::Components::PlayerController{
                                            .moveSpeed = 1.0f,
                                            .turnSpeed = 80.0f,
                                            .enabled = true});
    registry.AddComponent(turretEntity, ECS::Components::MeshRenderer{
                                            .model = std::make_shared<Model>(turret),
                                            .shader = std::make_shared<Shader>(shader)});

    registry.GetComponent<ECS::Components::Transform>(turretEntity).scale = {0.02f, 0.02f, 0.02f};

    // endregion Entity creation

    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 model;

    while (!window.ShouldClose())
    {
        auto now = static_cast<float>(glfwGetTime());
        deltaTime = now - lastTime;
        lastTime = now;

        if (fpsCounter <= 1)
        {
            fpsCounter += deltaTime;
            fpsCount++;
        }
        else
        {
            fps = fpsCount;
            fpsCounter = 0;
            fpsCount = 0;
        }

        if (enableEffects)
        {
            selectedEffect->BeginRender();
        }
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
        }

        glPolygonMode(GL_FRONT_AND_BACK, gridMode ? GL_LINE : GL_FILL);

        window.StartGui();

        camera.Move(deltaTime);

        view = camera.GetLookAt();
        projection = glm::perspective(glm::radians(45.0f), window.GetAspect(), 0.1f, 100.0f);

        // skybox
        //     .BeginRender(skyboxShader)
        //     .SetProjection(projection)
        //     .SetView(view)
        //     .Render();

        shader.Use();
        uniforms.model = shader.GetUniformLocation("model");

        shader.Set<4, 4>("view", view);
        shader.Set<4, 4>("projection", projection);
        shader.Set<3>("ambientLightColor", glm::vec3{1.0f, 1.0f, 1.0f});

        // Systems update
        systemManager.UpdateAll(registry, deltaTime);

        /*
         * TODO : Fix render order in the RenderSystem.
         */
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        model = glm::mat4(1.0f);
        model = glm::translate(model, {0.0f, 0.0f, 1.0f});
        model = glm::rotate(model, static_cast<float>(glfwGetTime()), {0.0f, 1.0f, 0.0f});
        model = glm::scale(model, {0.8f, 0.8f, 0.8f});
        shader.Set<4, 4>(uniforms.model, model);
        twob.Render(shader);

        if (enableGrid)
        {
            gridShader.Use();
            gridShader.Set<4, 4>("uVP", projection * view);
            gridShader.Set<3>("cameraPosition", camera.GetPosition());
            plane.Render();
            shader.Use();
        }

        shader.Set("pointLightsSize", static_cast<int>(pointLights.Size()));

        glDisable(GL_BLEND);

        // region gui
        ImGui::Begin("Camera info");
        auto camPos = camera.GetPosition();
        auto camDir = camera.GetDirection();
        ImGui::Text("Position: x=%f y=%f z=%f", static_cast<double>(camPos.x), static_cast<double>(camPos.y),
                    static_cast<double>(camPos.z));
        ImGui::Text("Direction: x=%f y=%f z=%f", static_cast<double>(camDir.x), static_cast<double>(camDir.y),
                    static_cast<double>(camDir.z));
        ImGui::Text("Yaw = %f | Pitch = %f", static_cast<double>(camera.GetYaw()),
                    static_cast<double>(camera.GetPitch()));
        ImGui::End();

        ImGui::Begin("Engine Info and Settings");
        ImGui::Text("Delta time = %f", static_cast<double>(deltaTime));
        ImGui::Text("FPS = %f", static_cast<double>(fps));
        ImGui::Checkbox("Enable Grid", &enableGrid);
        ImGui::Checkbox("Enable effects", &enableEffects);
        if (ImGui::Combo("Shader", &fbSelectedItem, fbItems, IM_ARRAYSIZE(fbItems)))
        {
            selectedEffect = framebuffers[fbSelectedItem];
        }

        ImGui::SeparatorText("Shader reload");

        if (ImGui::Button("Reload base shader"))
        {
            shader.ReloadShader();
            std::cout << "Shader reloaded";
        }

        if (ImGui::Button("Effect shader") && enableEffects)
        {
            selectedEffect->ReloadShader();
            std::cout << "Effect shader reloaded\n";
        }

        if (ImGui::Button("Grid shader"))
        {
            gridShader.ReloadShader();
            std::cout << "Grid shader reloaded!\n";
        }

        if (ImGui::Checkbox("Set grid mode", &gridMode))
            std::cout << std::format("Grid mode: {}\n", gridMode ? "enabled" : "disabled");

        if (ImGui::DragFloat("Camera move speed", &cameraMoveSpeed))
            camera.SetMoveSpeed(cameraMoveSpeed);

        if (ImGui::DragFloat("Camera turn speed", &cameraTurnSpeed))
            camera.SetTurnSpeed(cameraTurnSpeed);

        ImGui::End();

        ImGui::Begin("Lights control");

        if (ImGui::Button("Add light"))
        {
            pointLights.Add({.position = {0.0f, 0.0f, 2.0f, 0.0f},
                             .ambient = {0.1f, 0.1f, 0.1f, 0.0f},
                             .diffuse = {1.0f, 1.0f, 1.0f, 0.0f},
                             .specular = {1.0f, 1.0f, 1.0f, 0.0f},
                             .constant = 1.0f,
                             .linear = 0.09f,
                             .quadratic = 0.032f,
                             .isTurnedOn = true});
        }

        for (size_t i = 0; i < pointLights.Size(); ++i)
        {
            Lights::PointLight &pLight = pointLights[i];
            ImGui::PushID(std::format("PL{}", i).c_str());
            ImGui::Columns(3, "Position");
            ImGui::DragFloat("X", &pLight.position.x);
            ImGui::NextColumn();
            ImGui::DragFloat("Y", &pLight.position.y);
            ImGui::NextColumn();
            ImGui::DragFloat("Z", &pLight.position.z);
            ImGui::Columns(1);
            ImGui::SeparatorText(std::format("Pointlight {}", i).c_str());
            ImGui::SliderFloat("Constant", &pLight.constant, 0.0, 1.0);
            ImGui::SliderFloat("Linear", &pLight.linear, 0.0, 1.0);
            ImGui::SliderFloat("Quadratic", &pLight.quadratic, 0.0, 1.0);
            ImGui::Checkbox("Is turned on", reinterpret_cast<bool *>(&pLight.isTurnedOn));
            ImGui::ColorEdit4("Color", reinterpret_cast<float *>(&pLight.diffuse), ImGuiColorEditFlags_Float);
            pointLights.UpdateIndex(i);

            if (ImGui::Button(std::format("Delete", i).c_str()))
                pointLights.Remove(i);

            ImGui::PopID();
        }
        ImGui::End();
        // endregion

        if (enableEffects)
        {
            Framebuffer::EnableMainFramebuffer();
            selectedEffect->RenderQuad();
        }

        keyboard.HandleKeyLoop();

        // region Special keys handle
        if (!keyboard.GetKeyPress(GLFW_KEY_T)) enableCursorEvent = false;
        if (!keyboard.GetKeyPress(GLFW_KEY_F11)) fullscreenEvent = false;
        // endregion

        window.EndGui();
        window.EndRenderPass();
    }

    return 0;
}

#pragma clang diagnostic pop
