#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>
// #include <windows.h>
#include <iostream>
#include "Game.hpp"
#include "ui/GLHeaders.hpp"

int main(int argc, char* argv[])
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1000, 800, "Chess", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Load OpenGL function pointers with Glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to load OpenGL functions\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // char buffer[MAX_PATH];
    // GetCurrentDirectoryA(MAX_PATH, buffer);
    // std::cout << "Current working directory: " << buffer << std::endl;

    ImGuiIO&             io             = ImGui::GetIO();
    static const ImWchar chess_ranges[] = {0x0020, 0x00FF, 0x2654, 0x265F, 0}; // ASCII + pièces d'échecs
    ImFontConfig         config;
    ImFont*              font = io.Fonts->AddFontFromFileTTF("../../assets/fonts/segoe-ui-symbol.ttf", 32.0f, &config, chess_ranges);
    if (!font)
    {
        std::cerr << "Erreur : Impossible de charger la police segoe-ui-symbol.ttf" << std::endl;
    }
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Style ImGui
    ImGui::StyleColorsDark();

    // Initialisation des backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Initialisation du jeu
    Game game;
    if (!game.initialize(argv[0]))
    {
        std::cerr << "Erreur lors de l'initialisation du jeu" << '\n';
        return -1;
    }

    // Boucle principale
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Démarrer la frame ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Mettre à jour et afficher le jeu
        game.run();

        // Rendu
        ImGui::Render();
        int display_w = 0;
        int display_h = 0;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Nettoyage
    game.shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

// int main()
// {
//     quick_imgui::loop(
//         "Chess",
//         {
//             .init = [&]() {},
//             .loop = [&]() {

//                 Game game;
//                 game.init();
//                 game.render();
//             },
//         }
//     );
// }

/*int main()
{
    float value{0.f};

    quick_imgui::loop(
        "Chess",
        {
            .init = [&]() {},
            .loop =
                [&]() {
                    ImGui::ShowDemoWindow(); // This opens a window which shows tons of examples of what you can do with ImGui. You should check it out! Also, you can use the "Item Picker" in the top menu of that demo window: then click on any widget and it will show you the corresponding code directly in your IDE!

                    ImGui::Begin("Example");

                    ImGui::SliderFloat("My Value", &value, 0.f, 3.f);

                    if (ImGui::Button("1", ImVec2{50.f, 50.f}))
                        std::cout << "Clicked button 1\n";
                    ImGui::SameLine(); // Draw the next ImGui widget on the same line as the previous one. Otherwise it would be below it

                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{1.f, 0.f, 0.f, 1.f}); // Changes the color of all buttons until we call ImGui::PopStyleColor(). There is also ImGuiCol_ButtonActive and ImGuiCol_ButtonHovered

                    ImGui::PushID(2); // When some ImGui items have the same label (for exemple the next two buttons are labeled "Yo") ImGui needs you to specify an ID so that it can distinguish them. It can be an int, a pointer, a string, etc.
                                      // You will definitely run into this when you create a button for each of your chess pieces, so remember to give them an ID!
                    if (ImGui::Button("Yo", ImVec2{50.f, 50.f}))
                        std::cout << "Clicked button 2\n";
                    ImGui::PopID(); // Then pop the id you pushed after you created the widget

                    ImGui::SameLine();
                    ImGui::PushID(3);
                    if (ImGui::Button("Yo", ImVec2{50.f, 50.f}))
                        std::cout << "Clicked button 3\n";
                    ImGui::PopID();

                    ImGui::PopStyleColor();

                    ImGui::End();
                },
        }
    );
}*/