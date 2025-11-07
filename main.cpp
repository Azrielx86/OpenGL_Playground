#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
// region Global Include (Must be the first!)
#define STB_IMAGE_IMPLEMENTATION
#include "GlobalDefines.h"
// endregion Global Include

#include "Camera.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/MeshRenderer.h"
#include "ECS/Components/PlayerController.h"
#include "ECS/Components/Transform.h"
#include "ECS/Registry.h"
#include "ECS/SystemManager.h"
#include "ECS/Systems/CollisionSystem.h"
#include "ECS/Systems/PlayerControlSystem.h"
#include "ECS/Systems/RenderSystem.h"
#include "Input/Keyboard.h"
#include "Lights/PointLight.h"
#include "Model.h"
#include "Primitives/AbstractPrimitive.h"
#include "Primitives/Cube.h"
#include "Primitives/Plane.h"
#include "Resources/ResourceManager.h"
#include "Shader.h"
#include "SkinnedAnimation.h"
#include "SkinnedAnimator.h"
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
bool enableSkybox = false;
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

Primitives::Plane plane;
Primitives::Cube cube;

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
    registry.RegisterComponent<ECS::Components::AABBCollider>();
    registry.RegisterComponent<ECS::Components::SBBCollider>();
    registry.RegisterComponent<ECS::Components::OBBCollider>();
    systemManager.RegisterSystem<ECS::Systems::RenderSystem>();
    systemManager.RegisterSystem<ECS::Systems::PlayerControlSystem>();
    systemManager.RegisterSystem<ECS::Systems::CollisionSystem>();

    resources.ScanResources();
    Resources::ResourceManager::InitDefaultResources();

    auto particleTexture = resources.GetTexture("particle.png");

    Model turret("./assets/models/turret/source/turret_model.fbx");
    // Model pod("./assets/models/pod/source/pod.fbx");
    Model twob("./assets/models/2b/2b_rig.fbx");

    SkinnedAnimation *animation = twob.GetAnimation(1);

    SkinnedAnimator animator;

    if (animation)
    {
        animator.PlayAnimation(animation);
    }

    // clang-format off
	Skybox skybox({
	    "./assets/textures/skybox/sky_cubemap/px.png",
	    "./assets/textures/skybox/sky_cubemap/nx.png",
	    "./assets/textures/skybox/sky_cubemap/py.png",
	    "./assets/textures/skybox/sky_cubemap/ny.png",
	    "./assets/textures/skybox/sky_cubemap/pz.png",
	    "./assets/textures/skybox/sky_cubemap/nz.png"
	});
    // clang-format on
    skybox.Load();

    Shader shader = *resources.GetShader("base");
    Shader skyboxShader = *resources.GetShader("skybox_shader");
    Shader blurShader = *resources.GetShader("blur");
    Shader sharpShader = *resources.GetShader("sharp");
    Shader grayscaleShader = *resources.GetShader("grayscale");
    Shader gridShader = *resources.GetShader("infinite_grid");
    Shader debugShader = *resources.GetShader("debug");

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

    plane.Init();
    cube.Init();

    // region Entity creation

    ECS::Entity turretEntity = registry.CreateEntity();
    registry.AddComponent(turretEntity, ECS::Components::Transform{});
    registry.AddComponent(turretEntity, ECS::Components::PlayerController{
                                            .moveSpeed = 1.0f,
                                            .turnSpeed = 80.0f,
                                            .enabled = true});
    registry.AddComponent(turretEntity, ECS::Components::MeshRenderer{
                                            .model = &turret,
                                            .shader = &shader});
    // registry.AddComponent(turretEntity, ECS::Components::AABBCollider{
    //                                         .min = {-15.39f, 0.63f, -25.98f},
    //                                         .max = {15.39f, 58.68f, 25.98f}});
    registry.AddComponent(turretEntity, ECS::Components::OBBCollider{
                                            .center = {0.0f, 29.34f, 0.0f},
                                            .rotation = glm::quat{1.0f, 0.0f, 0.0f, 0.0f},
                                            .halfExtents = {15.39f, 29.34f, 25.98f}});

    registry.GetComponent<ECS::Components::Transform>(turretEntity).scale = {0.02f, 0.02f, 0.02f};

    ECS::Entity twoBEntity = registry.CreateEntity();
    registry.AddComponent(twoBEntity, ECS::Components::Transform{
                                          .translation = {0.0f, 0.0f, 1.0f},
                                          .scale = {0.8f, 0.8f, 0.8f}});
    registry.AddComponent(twoBEntity, ECS::Components::MeshRenderer{
                                          .model = &twob,
                                          .shader = &shader});
    // registry.AddComponent(twoBEntity, ECS::Components::AABBCollider{
    //                                       .min = {-0.3f, 0.0f, -0.31f},
    //                                       .max = {0.3f, 2.1f, 0.31f}});
    registry.AddComponent(twoBEntity, ECS::Components::OBBCollider{
                                          .center = {0.0f, 1.05f, 0.0f},
                                          .rotation = {1.0f, 0.0f, 0.0f, 0.0f},
                                          .halfExtents = {0.3f, 1.05f, 0.3f}});

    // endregion Entity creation

    glm::mat4 view;
    glm::mat4 projection;
    // glm::mat4 model;

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

        if (enableSkybox)
        {
            skybox
                .BeginRender(skyboxShader)
                .SetProjection(projection)
                .SetView(view)
                .Render();
        }

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

        // animator.UpdateAnimation(deltaTime);
        // auto finalBones = animator.GetFinalBoneMatrices();

        // model = glm::mat4(1.0f);
        // model = glm::translate(model, {0.0f, 0.0f, 1.0f});
        // model = glm::scale(model, {0.8f, 0.8f, 0.8f});
        // shader.Set<4, 4>(uniforms.model, model);
        // for (unsigned int i = 0; i < finalBones.size(); i++)
        //     shader.Set<4, 4>(std::format("bones[{}]", i).c_str(), finalBones[i]);
        // twob.Render(shader);

        for (unsigned int i = 0; i < MAX_BONES; i++)
            shader.Set<4, 4>(std::format("bones[{}]", i).c_str(), glm::mat4(1.0f));

        if (enableGrid)
        {
            gridShader.Use();
            gridShader.Set<4, 4>("uVP", projection * view);
            gridShader.Set<3>("cameraPosition", camera.GetPosition());
            plane.Render();
            shader.Use();
        }

        glEnable(GL_BLEND);

        glDisable(GL_BLEND);

        shader.Set("pointLightsSize", static_cast<int>(pointLights.Size()));

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        debugShader.Use();
        const GLint debugProjection = debugShader.GetUniformLocation("projection");
        const GLint debugView = debugShader.GetUniformLocation("view");
        const GLint debugModel = debugShader.GetUniformLocation("model");
        const GLint debugColor = debugShader.GetUniformLocation("color");
        debugShader.Set<3>(debugColor, glm::vec3(1.0f, 1.0f, 0.0f));
        debugShader.Set<4, 4>(debugProjection, projection);
        debugShader.Set<4, 4>(debugView, view);

        // AABB Debug draw
        for (const ECS::Entity colliderEntity : registry.View<ECS::Components::AABBCollider, ECS::Components::Transform>())
        {
            const auto &transform = registry.GetComponent<ECS::Components::Transform>(colliderEntity);
            const auto &collider = registry.GetComponent<ECS::Components::AABBCollider>(colliderEntity);
            const auto [min, max] = collider.GetWorldAABB(transform);

            auto collidersModel = glm::mat4(1.0f);
            collidersModel = glm::translate(collidersModel, min + (max - min) * 0.5f);
            collidersModel = glm::scale(collidersModel, max - min);
            debugShader.Set<4, 4>(debugModel, collidersModel);

            cube.Render();
        }

        // OBB Debug draw
        for (const ECS::Entity obbEntity : registry.View<ECS::Components::OBBCollider, ECS::Components::Transform>())
        {
            const auto &transform = registry.GetComponent<ECS::Components::Transform>(obbEntity);
            const auto &collider = registry.GetComponent<ECS::Components::OBBCollider>(obbEntity);
            const auto worldOBB = collider.GetWorldOBB(transform);

            auto collidersModel = glm::mat4(1.0f);
            collidersModel = glm::translate(collidersModel, worldOBB.center);
            collidersModel *= glm::mat4_cast(worldOBB.rotation);
            collidersModel = glm::scale(collidersModel, worldOBB.halfExtents * 2.0f);
            debugShader.Set<4, 4>(debugModel, collidersModel);

            cube.Render();
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
        ImGui::Checkbox("Enable skybox", &enableSkybox);
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
            ImGui::ColorEdit3("Color", reinterpret_cast<float *>(&pLight.diffuse), ImGuiColorEditFlags_Float);
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
