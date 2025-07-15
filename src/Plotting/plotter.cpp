#include "Plotting/plotter.h"

Eigen::Matrix3f ortho(int width, int height)
{
    return Eigen::Matrix3f{
        {2.0f / width, 0, -1.0f},
        {0, 2.0f / height, -1.0f},
        {0.0f, 0.0f, 1.0f}};
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
        RenderData data = line.loadDataToBuffers(xMin, xMax, yMin, yMax, GL_LINE_STRIP);

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

    plotAxes();

    plotAxesTicks();

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
            glDrawArrays(renderBuffers.mode, 0, renderBuffers.bufferSize);
        }

        glBindVertexArray(0);

        // renderText("This is a sample text 0123456789", 50.0f, 50.0f, 1.0f, Colour(0.5f, 0.8f, 0.2f));

        renderTickLabels();

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
    float xZeroPos = (float)scalePoint(0.0, xMin, xMax);
    float yZeroPos = (float)scalePoint(0.0, yMin, yMax);

    float axes[8];

    axes[0] = 0.6f;
    axes[1] = yZeroPos;
    axes[2] = -0.6;
    axes[3] = yZeroPos;
    axes[4] = xZeroPos - TICK_OFFSET;
    axes[5] = -0.6f;
    axes[6] = xZeroPos - TICK_OFFSET;
    axes[7] = 0.6f;

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    updateBuffers(VAO, 4, Eigen::Vector3f{1.0f, 1.0f, 1.0f}, GL_LINES);
}

void Plotter::plotAxesTicks()
{
    float xZeroPos = (float)scalePoint(0.0, xMin, xMax);
    float yZeroPos = (float)scalePoint(0.0, yMin, yMax);

    float sizeXAxis = 1.2f; // based on vertices defined in PlotAxes()
    float sizeYAxis = 1.2f; // based on vertices defined in PlotAxes()

    float xDelta = sizeXAxis / NUM_X_TICKS;
    float yDelta = sizeYAxis / NUM_Y_TICKS;

    float xInit = -0.6;
    float yInit = (float)scalePoint(0.0, yMin, yMax);

    float xOffset = TICK_OFFSET;
    float yOffset = TICK_OFFSET;

    float tickSize = 0.01f;

    float ticks[NUM_X_TICKS * 4 + NUM_Y_TICKS * 4];

    int k = 0;
    // vertices of all ticks on x-axis
    for (int i = 0; i < NUM_X_TICKS * 4; i += 4)
    {
        ticks[i] = xInit + k * xDelta + xOffset;
        ticks[i + 1] = yInit;
        xTickPositions.push_back(Vertex{ticks[i], ticks[i + 1]});
        ticks[i + 2] = xInit + k * xDelta + xOffset;
        ticks[i + 3] = yInit - tickSize;
        k++;
        std::cout << ticks[i] << ", " << ticks[i + 1] << std::endl;
    }

    k = 0;

    xInit = (float)scalePoint(0.0, xMin, xMax);
    yInit = -0.6;
    std::cout << "---------------_" << std::endl;

    // vertices of all ticks on y axis.
    for (int j = NUM_X_TICKS * 4; j < NUM_X_TICKS * 4 + NUM_Y_TICKS * 4; j += 4)
    {
        ticks[j] = xInit - xOffset;
        ticks[j + 1] = yInit + k * yDelta;
        if (k == 0)
            std::cout << ticks[j] << ", " << ticks[j + 1] << std::endl;
        yTickPositions.push_back(Vertex{ticks[j], ticks[j + 1]});
        ticks[j + 2] = xInit - tickSize - xOffset;
        ticks[j + 3] = yInit + k * yDelta;
        k++;
        std::cout << ticks[j] << ", " << ticks[j + 1] << std::endl;
    }

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ticks), ticks, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    updateBuffers(VAO, NUM_X_TICKS * 2 + NUM_Y_TICKS * 2, Eigen::Vector3f{1.0f, 1.0f, 1.0f}, GL_LINES);
}

void Plotter::renderTickLabels()
{
    float textScale = 0.15f;
    int nDecimals = 3;
    int k = 0;

    for (Vertex xTickPosition : xTickPositions)
    {
        // convert from [-0.5, 0.5] to [0, width] or [0, height] (pixel)
        float xPos = scaleBackPoint(xTickPosition.x - TICK_OFFSET, 0.6, SCR_WIDTH * 0.2, SCR_WIDTH * 0.8);
        float yPos = scaleBackPoint(xTickPosition.y, 0.6, SCR_HEIGHT * 0.2, SCR_HEIGHT * 0.8);

        float xValue = scaleBackPoint(xTickPosition.x, 0.5, xMin, xMax);

        // if (k == 0)
        std::cout << xTickPosition.x << ", " << xTickPosition.y << ", " << xPos << ", " << yPos << ", " << xMin << ", " << xMax << ", " << xValue << std::endl;
        k++;

        std::string formattedXValue = std::to_string(xValue);
        formattedXValue = formattedXValue.substr(0, formattedXValue.size() - (6 - nDecimals));

        renderText("X", xTickPosition.x, xTickPosition.y, textScale, Colour{"WHITE"});
    }
    std::cout << "------x------" << std::endl;
    k = 0;
    for (Vertex yTickPosition : yTickPositions)
    {
        // convert from [-0.5, 0.5] to [0, width] or [0, height] (pixel)
        float xPos = scaleBackPoint(yTickPosition.x, 0.6, SCR_WIDTH * 0.2, SCR_WIDTH * 0.8);
        float yPos = scaleBackPoint(yTickPosition.y, 0.6, SCR_HEIGHT * 0.2, SCR_HEIGHT * 0.8);

        float yValue = scaleBackPoint(yTickPosition.y, 0.5, yMin, yMax);

        // if (k == 0)
        std::cout << yTickPosition.x << ", " << yTickPosition.y << ", " << xPos << ", " << yPos << ", " << yMin << ", " << yMax << ", " << yValue << std::endl;
        k++;

        std::string formattedYValue = std::to_string(yValue);
        formattedYValue = formattedYValue.substr(0, formattedYValue.size() - (6 - nDecimals));

        renderText("Y", (float)scalePoint(0.0, xMin, xMax) + 0.2, yTickPosition.y, textScale, Colour{"WHITE"});
    }
    std::cout << "------y------" << std::endl;
}

void Plotter::updateBuffers(unsigned int buffer, int numVertices, Eigen::Vector3f colour, GLenum mode)
{
    RenderData data{buffer, numVertices, colour, mode};

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

    initFont();
}

void Plotter::initFont()
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    FT_Face face;
    if (FT_New_Face(ft, "../resources/fonts/Lora-Regular.ttf", 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load 128 ASCII character set
    for (unsigned int c = 0; c < 128; c++)
    {
        // load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer);
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character{
            texture,
            Eigen::Vector2i(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            Eigen::Vector2i(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x};
        characters[c] = character;
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Shader textShader = ResourceManager::LoadShader("../resources/shaders/text.vs", "../resources/shaders/text.fs", nullptr, "text");

    Eigen::Matrix3f orthoProjection = ortho(800, 600);

    textShader.Use();
    int projectionLoc = glGetUniformLocation(textShader.ID, "projection");
    glUniformMatrix3fv(projectionLoc, 1, GL_FALSE, orthoProjection.data());
}

void Plotter::renderText(std::string text, float x, float y, float scale, Colour colour)
{
    Shader textShader = ResourceManager::GetShader("text");
    // activate necessary shader + texture + buffer
    textShader.Use();

    glUniform3f(glGetUniformLocation(textShader.ID, "textColor"), colour.colour[0], colour.colour[1], colour.colour[2]);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    // iterate through characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = characters[*c];

        float xPos = x + ch.Bearing[0] * scale;
        float yPos = y - (ch.Size[1] - ch.Bearing[1]) * scale;

        float w = ch.Size[0] * scale / SCR_WIDTH;
        float h = ch.Size[1] * scale / SCR_HEIGHT;

        // update VBO for each character
        float vertices[6][4] = {
            {xPos, yPos + h, 0.0f, 0.0f},
            {xPos, yPos, 0.0f, 1.0f},
            {xPos + w, yPos, 1.0f, 1.0f},

            {xPos, yPos + h, 0.0f, 0.0f},
            {xPos + w, yPos, 1.0f, 1.0f},
            {xPos + w, yPos + h, 1.0f, 0.0f}};

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        // update VBO content
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // advance cursors for next glyph (advance is 1/64, hence bitshift of 2^6)
        x += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
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
