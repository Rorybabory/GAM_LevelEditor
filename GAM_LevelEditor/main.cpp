
#include <iostream>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl2.h"

#include <nfd.h>

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

unsigned int loadTexture(const char* filename) {
    unsigned int texture;

    int width, height, nrChannels;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }    
    stbi_image_free(data);

    return texture;
}

void renderRect() {
    glLoadIdentity();
    glScalef(720.0f/1280.0f, 1, 1);
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-1.0f, 1.0f);

    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(1.0f, 1.0f);

    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(1.0f, -1.0f);

    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(-1.0f, -1.0f);

    glEnd();
}

void importDialog(unsigned int * texture) {
    nfdchar_t* outPath = NULL;
    nfdresult_t result = NFD_OpenDialog(NULL, NULL, &outPath);

    if (result == NFD_OKAY) {
        printf("Found Image: %s\n", outPath);
        glDeleteTextures(1, texture);
        loadTexture(outPath);
        free(outPath);
    }
}

int main()
{
    glfwInit();
    glfwSetErrorCallback(glfw_error_callback);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Level Editor", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    //Setup IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();

    int button_counter = 0;
    float color[4] = { 0.3f,0.5f,0.7f,1 };

    unsigned int texture = loadTexture("Assets/map2.png");
    int display_w = 0;
    int display_h = 0;

    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_ALPHA);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        {
            bool active = true;
            ImGui::Begin("Level Editor", &active, ImGuiWindowFlags_MenuBar);
            ImGui::SetWindowSize(ImVec2(250, (float)display_h));
            ImGui::SetWindowPos(ImVec2(0, 0));

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Open..")) { importDialog(&texture); }
                    if (ImGui::MenuItem("Save")) {}
                    if (ImGui::MenuItem("Close")) {}
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            ImGui::ColorPicker4("##picker", (float*)&color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
            ImGui::Image((ImTextureID)(texture), ImVec2(180, 180), ImVec2(0, 0), ImVec2(1, 1));

            //ImGui::Text("Button Presses: %i", button_counter);
            //if (ImGui::Button("Button")) {
            //    button_counter++;
            //}

            ImGui::End();
        }

        ImGui::Render();
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(1,1,1,1);
        glClear(GL_COLOR_BUFFER_BIT);

        glDisable(GL_TEXTURE_2D);
        glColor3f(color[0], color[1], color[2]);
        renderRect();
        glColor3f(1, 1, 1);

        glBindTexture(GL_TEXTURE_2D, texture);
        glEnable(GL_TEXTURE_2D);
        renderRect();
        glDisable(GL_TEXTURE_2D);

        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}
