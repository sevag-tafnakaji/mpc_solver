#include "Plotting/plotter.h"

Plotter::Plotter()
{
    this->init();
}

Plotter::~Plotter()
{
    // TODO: delete all buffers
    // glDeleteVertexArrays(1, &plot.VAO);
    ResourceManager::Clear();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
}

std::vector<double> scaleVector(std::vector<double> vect, double minValue, double maxValue)
{
    // scale vector such that all new values would be [-0.5, 0.5]
    double abs_min_value = fabs(minValue);

    double shifted_max = maxValue + abs_min_value;

    std::vector<double> scaled_vector;

    for (double elem : vect)
    {
        double shifted_elem = (elem + abs_min_value) / shifted_max - 0.5;
        scaled_vector.push_back(shifted_elem);
    }

    return scaled_vector;
}

void Plotter::plot(std::vector<double> x, std::vector<double> y)
{
    if (x.size() != y.size())
        throw std::invalid_argument("X & Y are of different sizes.");

    xData.push_back(x);
    yData.push_back(y);
}

void Plotter::extractMinMaxValues()
{
    for (std::vector<double> x : xData)
    {
        const auto [minValue, maxValue] = std::minmax_element(x.begin(), x.end());

        if (*minValue < xMin)
            xMin = *minValue;

        if (*maxValue > xMax)
            xMax = *maxValue;
    }

    for (std::vector<double> y : yData)
    {
        const auto [minValue, maxValue] = std::minmax_element(y.begin(), y.end());

        if (*minValue < yMin)
            yMin = *minValue;

        if (*maxValue > yMax)
            yMax = *maxValue;
    }
}

void Plotter::loadDataToBuffers()
{
    if (xData.size() != yData.size())
        throw std::invalid_argument("Somehow the stored X & Y data do not match");

    for (int i = 0; i < xData.size(); i++)
    {
        std::vector<double> x = xData[i];
        std::vector<double> y = yData[i];

        float vertices[x.size() * 2];

        // Scale values to range from -0.5 to 0.5 (horizontally and vertically)
        std::vector<double> new_x = scaleVector(x, xMin, xMax);
        std::vector<double> new_y = scaleVector(y, yMin, yMax);

        int j = 0;
        for (int i = 0; i < x.size() * 2; i += 2)
        {
            vertices[i] = new_x[j];
            vertices[i + 1] = new_y[j];
            j++;
        }

        unsigned int VBO, VAO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        updateBuffers(VAO, x.size());
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

    extractMinMaxValues();
    loadDataToBuffers();

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
        ResourceManager::GetShader("plotter").Use();

        // plot all of the active buffers
        for (std::pair<unsigned int, int> BufferWithNumVertices : activeVAOs)
        {
            glBindVertexArray(BufferWithNumVertices.first);
            glDrawArrays(GL_LINE_STRIP, 0, BufferWithNumVertices.second);
        }

        glBindVertexArray(0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
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

    updateBuffers(VAO, 3);
}

void Plotter::updateBuffers(unsigned int buffer, int numVertices)
{
    std::pair<unsigned int, int> result = std::make_pair(buffer, numVertices);

    VAOs.push_back(result);
    activeVAOs.push_back(result);
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
