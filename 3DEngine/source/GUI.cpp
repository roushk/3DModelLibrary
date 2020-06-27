#include <pch.h>
#include "GUI.h"



GUI::GUI() : reader(pattern::get<ObjectReader>()), lighting(pattern::get<SceneLighting>()),
render(pattern::get<Render>())
{
  if (render.gWindow == nullptr)
    std::cout << "test" << std::endl;
}

void GUI::SetStyle()
{
  ImGuiStyle& Style = ImGui::GetStyle();

  Style.WindowBorderSize = 1.f;
  Style.WindowPadding = ImVec2(0, 0);
  Style.WindowRounding = 0.0f;
  Style.FramePadding = ImVec2(5, 0);
  Style.FrameRounding = 0.0f;
  Style.ItemSpacing = ImVec2(12, 8);
  Style.ItemInnerSpacing = ImVec2(8, 6);
  Style.IndentSpacing = 25.0f;
  Style.ScrollbarSize = 15.0f;
  Style.ScrollbarRounding = 0.0f;
  Style.GrabMinSize = 5.0f;
  Style.GrabRounding = 3.0f;
  Style.DisplaySafeAreaPadding = ImVec2(5, 5);
  const float alpha = 0.66f;
  Style.Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.0f);
  Style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, alpha);
  Style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, alpha);
  Style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, alpha);
  Style.Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, alpha);
  Style.Colors[ImGuiCol_Border] = ImVec4(0.30f, 0.30f, 0.33f, 0.88f);
  Style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
  Style.Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, alpha);
  Style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, alpha);
  Style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, alpha);
  Style.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, alpha);
  Style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
  Style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, alpha);
  Style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, alpha);
  Style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, alpha);
  Style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
  Style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, alpha);
  Style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, alpha);
  Style.Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
  Style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
  Style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, alpha);
  Style.Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, alpha);
  Style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, alpha);
  Style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, alpha);
  Style.Colors[ImGuiCol_Header] = ImVec4(0.20f, 0.19f, 0.22f, alpha);
  Style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, alpha);
  Style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, alpha);
  Style.Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, alpha);
  Style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, alpha);
  Style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, alpha);
  Style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  Style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, alpha);
  Style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, alpha);
  //Style.Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
  //Style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
  //Style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
  Style.Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
  Style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
  Style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
  Style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
  Style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
  Style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
}

void GUI::BindImGUI(SDL_Window* window, SDL_GLContext* context)
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls


  io.IniFilename = NULL; //disable imgui.ini

  ImGui_ImplSDL2_InitForOpenGL(window, context);
  ImGui_ImplOpenGL3_Init("#version 430");

  // Setup style
  //SetStyle();
  ImGui::StyleColorsDark();
  //ImFont* font0 = io.Fonts->AddFontDefault();
  //ImGui::PushFont(font0);
}

void GUI::RenderFrame()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(pattern::get<Render>().gWindow);
  ImGui::NewFrame();
  //***************************************************************//
  //ImGui::ShowDemoWindow();
  /*
    Select specific lights to be used in the scene by changing the number of active lights.
    Select the light types. Default light type should be a point light.
    Toggle to pause/start the light ‘rotation’.
    Toggle between the three scenarios
   */

  {
    //test window please ignore
    ImGui::Begin("Light Settings", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    //| ImGuiWindowFlags_NoCollapse);
    ImGui::Spacing();
    ImGui::SetWindowPos({0, 0}); //top left
    ImGui::SetWindowSize({350, static_cast<float>(render.height)});
    std::vector<const char*> names = lighting.GetLightNames();
    std::vector<const char*> types = lighting.GetLightTypes();
    std::vector<const char*> models = lighting.GetLightingTypes();
    static int currentLight = 0;
    static int currentLightType = 0;
    static int currentLightingModel = ssReflectionMap;
    static int currentScenario = 0;
    static int currentUVModel = 0;
    /*
    ImGui::Text("Highlight Tightness");
    ImGui::DragFloat("HighlightTightness", &render.highlightTightness, 0.025, 0, 1000);
    ImGui::Text("Transmission Coefficient");
    ImGui::DragFloat("TransmissionCoefficient", &render.transmissionCoefficient, 0.001, 0, 1);
    
    //Lighting Model Selection
    ImGui::Text("Lighting Model Type");
    ImGui::PushItemWidth(-1);
    ImGui::ListBox(" Models", &currentLightingModel, models.data(), models.size(), 3);
    //ImGui::PushItemWidth(70);
    //ImGui::InputInt("Max Lights", &lighting.maxLights, 1);
    //ImGui::PopItemWidth();
    */
    //Scenario Selection
    const char* listbox_items[] = {
      "Same Parameters (Point)", "Same Parameters (Directional)",
      "Same Parameters (Spotlight)", "Different Params / Same Type (Point)",
      "Different Params / Same Type (Directional)", "Different Params / Same Type (Spotlight)",
      "Mixed Params / Types (0)", "Mixed Params / Types (1)",
      "Mixed Params / Types (2)"
    };
    /*
    ImGui::Text("Scenario Selection");
    ImGui::PushItemWidth(-1);
    ImGui::ListBox(" Scenarions", &currentScenario, listbox_items, IM_ARRAYSIZE(listbox_items), 3);
    ImGui::PopItemWidth();
    */
    const char* listbox_items2[] = {
      "Axis Aligned Bounding Box",
      "Bounding Sphere Centroid",
      "Bounding Sphere Ritter",
      "Bounding Sphere Larsson",
      "Bounding Sphere PCA",
      "Bounding Ellipsoid PCA",
      "Oriented Bounding Box PCA",
      "AABB Bounding Volume Heiarchy",
      "BS Bounding Volume Heiarchy"
    };





    ImGui::Checkbox("Overwrite Params", &overwriteParams);

    if (overwriteParams)
    {
      float inner = 0.5f;
      float outer = 0.1f;
      float falloff = 0.1f;
      switch (currentScenario) //same parameters
      {
      default:
        //nuffin

        break;
      case 0:

        //spot lights
        for (auto& light : lighting.lights)
        {
          light.SetPointLight({0.1f, 0.1f, 0.1f}, {10.0f, 10.0f, 10.0f}, {2.0f, 2.0f, 2.0f});
        }

        break;
      case 1: //same params directional

        for (auto& light : lighting.lights)
        {
          light.SetDirectionalLight({0.1f, 0.1f, 0.1f}, {0.5f, 1.0f, 0.2f}, {1.0f, 0.3f, 0.2f});
        }

        break;
      case 2: //same params spot

        for (auto& light : lighting.lights)
        {
          light.SetSpotlight(inner, outer, falloff, {0.1f, 0.1f, 0.1f}, {0.8f, 1.0f, 0.3f}, {0.9f, 1.0f, 0.1f}
          );
        }

        break;
      case 3: //different colors spot
        //spot lights
        //amb,                 diff,              spec
        lighting.lights[0].SetPointLight({0.3f, 0.1f, 0.1f}, {0.1f, 0.4f, 0.1f}, {1.0f, 0.8f, 0.0f});
        lighting.lights[1].SetPointLight({0.1f, 0.4f, 0.1f}, {1.0f, 0.5f, 1.0f}, {0.5f, 0.2f, 1.0f});
        lighting.lights[2].SetPointLight({0.2f, 0.1f, 0.9f}, {1.0f, 0.6f, 1.0f}, {1.0f, 0.7f, 1.0f});
        lighting.lights[3].SetPointLight({0.1f, 0.7f, 0.1f}, {1.0f, 0.8f, 1.0f}, {1.0f, 1.0f, 0.3f});
        lighting.lights[4].SetPointLight({0.5f, 0.1f, 0.1f}, {1.0f, 1.0f, 1.0f}, {0.7f, 0.0f, 1.0f});
        lighting.lights[5].SetPointLight({0.7f, 0.1f, 0.1f}, {0.1f, 0.7f, 0.1f}, {0.1f, 0.4f, 0.1f});
        lighting.lights[6].SetPointLight({0.8f, 0.5f, 0.1f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.2f, 0.7f});
        lighting.lights[7].SetPointLight({0.1f, 0.1f, 1.0f}, {1.0f, 0.7f, 1.0f}, {0.0f, 1.0f, 1.0f});

        break;
      case 4: //different colors spot
        //amb,                 diff,              spec
        lighting.lights[0].SetDirectionalLight({0.3f, 0.1f, 0.1f}, {0.1f, 0.4f, 0.1f}, {1.0f, 0.8f, 0.0f});
        lighting.lights[1].SetDirectionalLight({0.1f, 0.4f, 0.1f}, {1.0f, 0.5f, 1.0f}, {0.5f, 0.2f, 1.0f});
        lighting.lights[2].SetDirectionalLight({0.2f, 0.1f, 0.9f}, {1.0f, 0.6f, 1.0f}, {1.0f, 0.7f, 1.0f});
        lighting.lights[3].SetDirectionalLight({0.1f, 0.7f, 0.1f}, {1.0f, 0.8f, 1.0f}, {1.0f, 1.0f, 0.3f});
        lighting.lights[4].SetDirectionalLight({0.5f, 0.1f, 0.1f}, {1.0f, 1.0f, 1.0f}, {0.7f, 0.0f, 1.0f});
        lighting.lights[5].SetDirectionalLight({0.7f, 0.1f, 0.1f}, {0.1f, 0.7f, 0.1f}, {0.1f, 0.4f, 0.1f});
        lighting.lights[6].SetDirectionalLight({0.8f, 0.5f, 0.1f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.2f, 0.7f});
        lighting.lights[7].SetDirectionalLight({0.1f, 0.1f, 1.0f}, {1.0f, 0.7f, 1.0f}, {0.0f, 1.0f, 1.0f});
        break;
      case 5: //different colors spot
        lighting.lights[0].SetSpotlight(inner, outer, falloff, {0.3f, 0.1f, 0.1f}, {0.1f, 0.4f, 0.1f},
                                        {1.0f, 0.8f, 0.0f});
        lighting.lights[1].SetSpotlight(inner, outer, falloff, {0.1f, 0.4f, 0.1f}, {1.0f, 0.5f, 1.0f},
                                        {0.5f, 0.2f, 1.0f});
        lighting.lights[2].SetSpotlight(inner, outer, falloff, {0.2f, 0.1f, 0.9f}, {1.0f, 0.6f, 1.0f},
                                        {1.0f, 0.7f, 1.0f});
        lighting.lights[3].SetSpotlight(inner, outer, falloff, {0.1f, 0.7f, 0.1f}, {1.0f, 0.8f, 1.0f},
                                        {1.0f, 1.0f, 0.3f});
        lighting.lights[4].SetSpotlight(inner, outer, falloff, {0.5f, 0.1f, 0.1f}, {1.0f, 1.0f, 1.0f},
                                        {0.7f, 0.0f, 1.0f});
        lighting.lights[5].SetSpotlight(inner, outer, falloff, {0.7f, 0.1f, 0.1f}, {0.1f, 0.7f, 0.1f},
                                        {0.1f, 0.4f, 0.1f});
        lighting.lights[6].SetSpotlight(inner, outer, falloff, {0.8f, 0.5f, 0.1f}, {1.0f, 1.0f, 1.0f},
                                        {1.0f, 0.2f, 0.7f});
        lighting.lights[7].SetSpotlight(inner, outer, falloff, {0.1f, 0.1f, 1.0f}, {1.0f, 0.7f, 1.0f},
                                        {0.0f, 1.0f, 1.0f});
        break;
      case 6: //mixed 1
        lighting.lights[0].SetPointLight({0.3f, 0.1f, 0.1f}, {0.1f, 0.4f, 0.1f}, {1.0f, 0.8f, 0.0f});
        lighting.lights[1].SetDirectionalLight({0.1f, 0.4f, 0.1f}, {1.0f, 0.5f, 1.0f}, {0.5f, 0.2f, 1.0f});
        lighting.lights[2].SetSpotlight(inner, outer, falloff, {0.2f, 0.1f, 0.9f}, {1.0f, 0.6f, 1.0f},
                                        {1.0f, 0.7f, 1.0f});
        lighting.lights[3].SetPointLight({0.1f, 0.7f, 0.1f}, {1.0f, 0.8f, 1.0f}, {1.0f, 1.0f, 0.3f});
        lighting.lights[4].SetPointLight({0.5f, 0.1f, 0.1f}, {1.0f, 1.0f, 1.0f}, {0.7f, 0.0f, 1.0f});
        lighting.lights[5].SetDirectionalLight({0.7f, 0.1f, 0.1f}, {0.1f, 0.7f, 0.1f}, {0.1f, 0.4f, 0.1f});
        lighting.lights[6].SetSpotlight(inner, outer, falloff, {0.8f, 0.5f, 0.1f}, {1.0f, 1.0f, 1.0f},
                                        {1.0f, 0.2f, 0.7f});
        lighting.lights[7].SetSpotlight(inner, outer, falloff, {0.1f, 0.1f, 1.0f}, {1.0f, 0.7f, 1.0f},
                                        {0.0f, 1.0f, 1.0f});

        break;
      case 7: //mixed 2
        lighting.lights[0].SetSpotlight(inner, outer, falloff, {0.3f, 0.1f, 0.1f}, {0.1f, 0.4f, 0.1f},
                                        {1.0f, 0.8f, 0.0f});
        lighting.lights[1].SetSpotlight(inner, outer, falloff, {0.1f, 0.4f, 0.1f}, {1.0f, 0.5f, 1.0f},
                                        {0.5f, 0.2f, 1.0f});
        lighting.lights[3].SetDirectionalLight({0.1f, 0.7f, 0.1f}, {1.0f, 0.8f, 1.0f}, {1.0f, 1.0f, 0.3f});
        lighting.lights[4].SetPointLight({0.2f, 0.1f, 0.9f}, {1.0f, 0.6f, 1.0f}, {1.0f, 0.7f, 1.0f});
        lighting.lights[5].SetDirectionalLight({0.5f, 0.1f, 0.1f}, {1.0f, 1.0f, 1.0f}, {0.7f, 0.0f, 1.0f});
        lighting.lights[6].SetPointLight({0.8f, 0.5f, 0.1f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.2f, 0.7f});
        lighting.lights[7].SetSpotlight(inner, outer, falloff, {0.2f, 0.1f, 0.9f}, {1.0f, 0.6f, 1.0f},
                                        {1.0f, 0.7f, 1.0f});


        break;
      case 8: //mixed 3
        lighting.lights[0].SetSpotlight(inner, outer, falloff, {0.8f, 0.5f, 0.1f}, {1.0f, 1.0f, 1.0f},
                                        {1.0f, 0.2f, 0.7f});
        lighting.lights[1].SetSpotlight(inner, outer, falloff, {0.1f, 0.1f, 1.0f}, {1.0f, 0.7f, 1.0f},
                                        {0.0f, 1.0f, 1.0f});
        lighting.lights[2].SetDirectionalLight({0.5f, 0.1f, 0.1f}, {1.0f, 1.0f, 1.0f}, {0.7f, 0.0f, 1.0f});
        lighting.lights[3].SetDirectionalLight({0.1f, 0.4f, 0.1f}, {1.0f, 0.5f, 1.0f}, {0.5f, 0.2f, 1.0f});
        lighting.lights[4].SetPointLight({0.7f, 0.1f, 0.1f}, {0.1f, 0.7f, 0.1f}, {0.1f, 0.4f, 0.1f});
        lighting.lights[5].SetPointLight({0.8f, 0.5f, 0.1f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.2f, 0.7f});
        lighting.lights[6].SetPointLight({0.1f, 0.1f, 1.0f}, {1.0f, 0.7f, 1.0f}, {0.0f, 1.0f, 1.0f});
        lighting.lights[7].SetDirectionalLight({0.1f, 0.4f, 0.1f}, {1.0f, 0.5f, 1.0f}, {0.5f, 0.2f, 1.0f});
        break;
      }
    }


    /*
    render.SetObjectShader(currentLightingModel);
    const char* uvitems[] = {"Cylindrical", "Spherical", "6 Planar"};
    ImGui::Text("UV Mapping Selection");
    ImGui::PushItemWidth(-1);
    ImGui::ListBox(" CoordTypes", &render.currentUVModel, uvitems, IM_ARRAYSIZE(uvitems), 4);
    ImGui::PopItemWidth();
    */
    //I and K global

    int oldcurrentTextureMaps = currentTextureMaps;
    ImGui::SliderInt("Current Texture", &currentTextureMaps, 0, pattern::get<Render>().textureMaps.size() - 1);

    if(currentTextureMaps != oldcurrentTextureMaps)
    {
      pattern::get<Render>().LoadNormalAndHeight();
      pattern::get<Render>().LoadMaterial();
    }

    ImGui::Checkbox("Debug Draw Mode Toggle", &debugDrawMode);
    ImGui::Checkbox("BRDF + IBL / Phong", &BRDF_IBL);
    ImGui::Checkbox("SSAO", &SSAO);

    ImGui::Checkbox("Automatic Camera", &autoCameraRotation);
    if(autoCameraRotation)
    {
      ImGui::Indent(10.0f);
      ImGui::Checkbox("Camera Rotation", &rotateCamera);
      ImGui::Unindent(10.0f);

    }

    ImGui::Checkbox("Normal Mapping", &NormalMapping);
    if(NormalMapping)
    {
      ImGui::Indent(10.0f);
      ImGui::Checkbox("Parallax Mapping Instead", &ParallaxMapping);
      ImGui::Unindent(10.0f);

    }
    ImGui::Checkbox("Enable Local Lights", &EnableLocalLights);
    if(EnableLocalLights)
    {
      ImGui::Indent(10.0f);
      ImGui::Checkbox("Display Light Spheres Diffuse", &showLightSpheres);
      ImGui::Unindent(10.0f);
    }
    ImGui::Checkbox("Start/Stop Light Rotation", &rotateLights);
    ImGui::PushItemWidth(200);
    //ImGui::DragFloat("Scalar Level", &render.scalarLevel, 0.05f, 0.0f, 200.0f);
    ImGui::DragFloat("Parallax Scale", &ParallaxScale, 0.000001f, 0, 0.8f,"%.6f");
    ImGui::DragFloat("Max Depth", &render.max_depth, 0.05f, 0.0f, 200.0f);
    ImGui::DragFloat("Exposure", &render.exposure, 0.05f, 0.0f, 10000.0f);
    ImGui::DragFloat("Contrast (0 is bright)", &render.contrast, 0.02f, 0.0f, 5.0f);
    ImGui::DragFloat("Material Alpha", &render.materialRoughness, 0.005f, 0.0f, 500.0f);
    ImGui::DragFloat("SSAO Contrast", &render.SSAOcontrast, 0.001f, 0.0f, 5.0f);
    ImGui::DragFloat("SSAO Scale", &render.SSAOscale, 0.001f, 0.0f, 5.0f);
    ImGui::DragFloat("SSAO Range", &render.SSAOrange, 0.001f, 0.0f, 5.0f);
    ImGui::DragFloat("SSAO Blur Scalar", &render.SSAOBlurScalar, 0.001f, 0.0f, 5.0f);

    
    ImGui::PopItemWidth();
    /*
    ImGui::BeginChild("Global Color", {390, 90});
    ImGui::Text("Global Color");
    ImGui::DragFloat3("Iglobal", glm::value_ptr(lighting.global.Iglobal), 0.005f, 0.0f, 1.0f);
    ImGui::DragFloat("Kglobal", &lighting.global.Kglobal, 0.005f, 0.0f, 1.0f);
    ImGui::DragFloat3("FogColor", glm::value_ptr(lighting.global.FogColor), 0.005f, 0.0f, 1.0f);
    ImGui::EndChild();
    */
    //Attenuation
    ImGui::BeginChild("Attenumation paramaters", {390, 40});
    ImGui::Text("Attenumation paramaters");
    ImGui::DragFloat3("C1, C2, C3", glm::value_ptr(lighting.global.AttenParam), 0.005f, 0.0f, 1.0f);
    ImGui::EndChild();

    //Ambient and emissive light settings
    ImGui::BeginChild("Ambient/Emissive Material Settings", {390, 80});
    ImGui::Text("Ambient/Emissive Material Settings");
    ImGui::DragFloat3("Ambient", glm::value_ptr(lighting.global.KMaterialambient), 0.005f, 0.0f, 1.0f);
    ImGui::DragFloat3("Emissive", glm::value_ptr(lighting.global.KMaterialemissive), 0.005f, 0.0f, 1.0f);
    ImGui::PushItemWidth(50);
    /**ImGui::DragFloat("Near Plane", &lighting.global.near, 0.01f, 0.0f, lighting.global.far);
    ImGui::SameLine();

    ImGui::DragFloat("Far Plane", &lighting.global.far,0.01f, lighting.global.near);*/
    ImGui::PopItemWidth();
    ImGui::EndChild();
    
    if (ImGui::CollapsingHeader("Light Settings"))
    {
      //Light selection
      //*********//
      Light& light = lighting.lights.at(currentLight);
      std::string name;
      float lightHeight = 0;
      if (light.type == ltDirectional)
      {
        name = "Directional Light";
        lightHeight = 110;
      }
      else if (light.type == ltPoint)
      {
        name = "Point Light";
        lightHeight = 110;
      }
      else if (light.type == ltSpotlight) //spotlight
      {
        name = "Spot Light";
        lightHeight = 180;
      }
      else
        lightHeight = 0;


      ImGui::BeginChild("LightSettings", { 390, lightHeight + 90 });
      ImGui::Text("Light Settings");
      ImGui::PushItemWidth(60);
      ImGui::InputInt("Active Lights", &lighting.activeLights, 1);
      if (lighting.maxLights < lighting.activeLights)
        lighting.activeLights = lighting.maxLights;
      if (lighting.activeLights < 0)
        lighting.activeLights = 0;
      ImGui::SameLine();
      ImGui::PushItemWidth(100);
      ImGui::Combo("Lights", &currentLight, names.data(), lighting.activeLights);
      ImGui::PushItemWidth(150);
      currentLightType = light.type;

      ImGui::Combo("Type", &currentLightType, types.data(), types.size());
      light.type = currentLightType;

      ImGui::PopItemWidth();

      if (light.type != ltNone)
      {
        ImGui::BeginChild("Lights", { 390, lightHeight });
        ImGui::Text(name.c_str());
        ImGui::DragFloat3("Ambient", glm::value_ptr(light.ambient), 0.005f, 0.0f, 10.0f);
        ImGui::DragFloat3("Diffuse", glm::value_ptr(light.diffuse), 0.005f, 0.0f, 10.0f);
        ImGui::DragFloat3("Specular", glm::value_ptr(light.specular), 0.005f, 0.0f, 10.0f);
      }
      if (light.type == ltSpotlight || light.type == ltDirectional)
        ImGui::DragFloat3("Direction", glm::value_ptr(light.direction), 0.005f, -10.0f, 10.0f);
      if (light.type == ltSpotlight || light.type == ltPoint)
        ImGui::DragFloat3("Position", glm::value_ptr(light.position), 0.01f, -10.0f, 10.0f);
      if (light.type == ltSpotlight)
      {
        ImGui::PushItemWidth(50);
        ImGui::DragFloat("Outer Radius", &light.outerRadius, 0.005f, 0.0f, PI);
        ImGui::SameLine();
        ImGui::DragFloat("Inner Radius", &light.innerRadius, 0.005f, 0.0f, PI);
        ImGui::DragFloat("Falloff Value", &light.falloffValue, 0.005f, 0.0f, 1.0f);
        if (light.outerRadius > light.innerRadius)
        {
          light.innerRadius = light.outerRadius;
        }

        ImGui::PopItemWidth();
      }


      static unsigned i = 0;
      //Toggle Lights on/off
      if (light.type != ltNone)
        ImGui::EndChild();


      ImGui::EndChild();
    }
    //*********//
    /*
    buttonLeftDown = ImGui::ArrowButton("Left", ImGuiDir_Left);


    ImGui::SameLine();
    ImGui::Text("Change Model");
    ImGui::SameLine();

    buttonRightDown = ImGui::ArrowButton("Right", ImGuiDir_Right);
    //ImGui::Text("Position Of Model");
    //ImGui::DragFloat3("Position Of Model", glm::value_ptr(position), 0.005f, -3.0f, 3.0f);
    */
    ImGui::Text("Camera");
    //ImGui::DragFloat3("Eye point", glm::value_ptr(position), 0.001f, -5.0f, 5.0f);
    //ImGui::DragFloat3("Look at vec", glm::value_ptr(position), 0.001f, -5.0f, 5.0f);


    ImGui::Text("Current GBuffer To Display");
    ImGui::Text("0 = View Pos, 1 = Normal, 2 = Diffuse");
    ImGui::Text("3 = Specular,  4 = Pre-Blur Shadow (RGB = z)");
    ImGui::Text("5 = Post-Horizontal-Blur Shadow (RGB = z)");
    ImGui::Text("6 = Post-Both-Blur Shadow (RGB = z)");
    ImGui::Text("7 = SSAO Map (RGB = z)");
    ImGui::Text("8 = Post-Horizontal-Blur SSAO (RGB = z)");
    ImGui::Text("9 = Post-Both-Blur SSAO (RGB = z)");
    ImGui::Text("10 = Tangent Map");


    ImGui::SliderInt("Current GBuffer Texture", &currentCam, 0, 10);
    //ImGui::Text("FBO to Render");
    //ImGui::SliderInt("Current FBO", &currentFBO, 0, 5);
    //toggle between 3 scenarios
    /*
     All lights have the same color parameters and type (position/directional/spot).
    § All lights have different color parameters and same type
    (position/directional/spot).
    § Implement a third scenario that mixes the lights and their types. For
    example, you can have half of your active lights positional and the other
    half spotlights. Arrange them in a creative combination to get cool
    lighting effects.
     */

    ImGui::End();
  }

  //render stuff

  //all data
  //format it

  //***************************************************************//
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  ImGui::EndFrame();
}

void GUI::Update(float dt)
{
}

GUI::~GUI()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
}
