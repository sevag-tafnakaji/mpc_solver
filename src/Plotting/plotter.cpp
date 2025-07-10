#include "Plotting/plotter.h"

bool comp_x(const Vertex &a, const Vertex &b)
{
    return a.x < b.x;
}

bool comp_y(const Vertex &a, const Vertex &b)
{
    return a.y < b.y;
}

Plotter::Plotter()
{
    this->init();
}

Plotter::~Plotter()
{
    for (RenderData data : VAOs)
    {
        glDeleteVertexArrays(1, &data.VAO);
    }
    ResourceManager::Clear();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
}

void Plotter::plot(std::vector<double> x, std::vector<double> y)
{
    if (x.size() != y.size())
        throw std::invalid_argument("X & Y are of different sizes.");

    // numLines iterates over the valid colours defined in line.h
    int numLines = plotLines.size() % NUM_COLOURS;

    Line line(numLines);

    line.setData(x, y);

    plotLines.push_back(line);
}

void Plotter::plot(std::vector<double> x, std::vector<double> y, std::string colour)
{
    if (x.size() != y.size())
        throw std::invalid_argument("X & Y are of different sizes.");

    Line line(x, y, colour);

    plotLines.push_back(line);
}

void Plotter::plot(std::vector<double> x, std::vector<double> y, Eigen::Vector3f colour)
{
    if (x.size() != y.size())
        throw std::invalid_argument("X & Y are of different sizes.");

    Line line(x, y, colour);

    plotLines.push_back(line);
}

void Plotter::extractMinMaxValues()
{
    for (Line l : plotLines)
    {
        const auto [xMinValue, xMaxValue] = std::minmax_element(l.lineData.begin(), l.lineData.end(), comp_x);
        const auto [yMinValue, yMaxValue] = std::minmax_element(l.lineData.begin(), l.lineData.end(), comp_y);

        if (xMinValue->x < xMin)
            xMin = xMinValue->x;

        if (xMaxValue->x > xMax)
            xMax = xMaxValue->x;

        if (yMinValue->y < yMin)
            yMin = yMinValue->y;

        if (yMaxValue->y > yMax)
            yMax = yMaxValue->y;
    }
}

void Plotter::loadDataToBuffers()
{
    for (Line line : plotLines)
    {
        RenderData data = line.loadDataToBuffers(xMin, xMax, yMin, yMax);

        updateBuffers(data);
    }
}

/**
 * TODO: Add the following:
 * Grid
 * Colour for each line (automatic + manual)
 * axis ticks and values (automatic + manual)
 * legend/label (must be able to throw error if no label was given)
 */
void Plotter::render()
{

    // Extract what the maximum x & y values are
    extractMinMaxValues();

    // use the max x & y values to scale data of each line to [-0.5, 0.5], then store as RenderData struct
    loadDataToBuffers();

    auto start = std::chrono::high_resolution_clock::now();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Activate the current shader
        Shader shader = ResourceManager::GetShader("plotter");

        shader.Use();

        // plot all of the active buffers
        for (RenderData renderBuffers : activeVAOs)
        {
            shader.SetVector3f("colour", renderBuffers.colour, false);
            glBindVertexArray(renderBuffers.VAO);
            glDrawArrays(GL_LINE_STRIP, 0, renderBuffers.bufferSize);
        }

        glBindVertexArray(0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();

        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        // std::cout << duration.count() << "ms (" << 1000.0 / duration.count() << " FPS)." << std::endl;

        start = end;
    }
}

void Plotter::plotAxes()
{
    float axes[6];

    axes[0] = 0.6f;
    axes[1] = -0.6f;
    axes[2] = -0.6f;
    axes[3] = -0.6f;
    axes[4] = -0.6f;
    axes[5] = 0.6f;

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    updateBuffers(VAO, 3, Eigen::Vector3f{1.0f, 1.0f, 1.0f});
}

void Plotter::updateBuffers(unsigned int buffer, int numVertices, Eigen::Vector3f colour)
{
    RenderData data{buffer, numVertices, colour};

    VAOs.push_back(data);
    activeVAOs.push_back(data);
}

void Plotter::updateBuffers(RenderData data)
{
    VAOs.push_back(data);
    activeVAOs.push_back(data);
}

void Plotter::init()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        // TODO: Raise some custom failed init error
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        // TODO: Raise some custom failed init error
    }

    ResourceManager::LoadShader("../resources/shaders/shader.vs", "../resources/shaders/shader.fs", nullptr, "plotter");

    plotAxes();
}

// // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// // ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
