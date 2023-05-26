//! @file EmuUI/MainWindow.cpp
//! @brief The definition of an object representing the main application window
//! of the Mighty Oak emulator.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <glm/glm.hpp>

#include "Ag/ObjectGL/SDL2Driver.hpp"
#include "EmuUI/MainWindow.hpp"

namespace Mo {
namespace UI {

////////////////////////////////////////////////////////////////////////////////
// MainWindow Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Ensures the window and its related render context are properly
//! disposed of.
MainWindow::~MainWindow()
{
    cleanup();
}

//! @brief Creates an instance of the main window ready to render OpenGL.
//! @throw Ag::Exception In one of various failure conditions.
std::shared_ptr<MainWindow> MainWindow::create()
{
    Ag::GL::SDL2Driver driver;
    Ag::GL::DisplayFormat displayFormat;
    displayFormat.enableHardwareAcceleration(true);
    displayFormat.enableDoubleBuffering(true);
    displayFormat.setRedBitCount(8);
    displayFormat.setGreenBitCount(8);
    displayFormat.setBlueBitCount(8);

    Ag::GL::ContextOptions renderSettings;
#ifdef _DEBUG
    renderSettings.enableDebugging(true);
#endif
    renderSettings.enableCoreProfile(true);

    // TODO: On Windows, this changes some state which means that the
    // later call to driver.crateDisplayDevice() can only create a legacy
    // OpenGL 1.1 context. I have no idea why. So we'll skip it for now.
    //Ag::Version maxVersion = driver.getMaxSupportedVersion(renderSettings,
    //                                                       displayFormat);

    //if (maxVersion < Ag::Version(4, 2))
    //{
    //    throw Ag::NotSupportedException("Running with a version of OpenGL less than 4.2 Core");
    //}

    renderSettings.setPreferredVersion(4, 2);

    Ag::GL::DisplayContext display = driver.createDisplayDevice(displayFormat);

    Ag::SDL::WindowUPtr window(
        SDL_CreateWindow("Mighty Oak",
                         SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED,
                         640, 480,
                         SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE));

    if (!window)
    {
        throw Ag::SDL::ApiException("SDL_CreateWindow(SDL_WINDOW_OPENGL)");
    }

    Ag::GL::RenderContext renderer =
        display.createRenderer(reinterpret_cast<uintptr_t>(window.get()),
                               renderSettings);

    // Ensure the context is selected before using it.
    if (renderer.getAPI().getAPIVersion() < Ag::Version(4, 2))
    {
        throw Ag::OperationException("Failed to create a proper OpenGL context.");
    }

    // NOTE: Can't use std::make_unique<>()  because the default constructor
    // is private, however, there are no construction parameters, so no need.
    std::shared_ptr<MainWindow> instance = std::make_shared<MainWindow>();

    // Initialise the object now that we know everything is fine.
    instance->_sdlWindow = std::move(window);
    instance->_display = display;
    instance->_renderer = renderer;

    instance->initialise();

    return instance;
}

//! @brief Registers with an InputManager in order to process input events.
//! @param[in] inputManager The object which processes events for the window.
void MainWindow::registerHandlers(Ag::SDL::InputManager &inputManager)
{
    inputManager.registerWindowEventHandler(_sdlWindow.get(), shared_from_this());
    inputManager.registerKeyboardEventHandler(_sdlWindow.get(), shared_from_this());
}

//! @brief Initialises OpenGL rendering resources.
void MainWindow::initialise()
{
    if (!_renderer.isBound())
        return;

    Ag::GL::RenderScope scope(_renderer);

    _scene = std::make_unique<Scene>(_renderer);
}

//! @brief Renders the contents of the window.
void MainWindow::redraw()
{
    if (!_renderer.isBound() || !_scene)
        return;

    Ag::GL::RenderScope scope(_renderer);

    int width, height;
    SDL_GL_GetDrawableSize(_sdlWindow.get(), &width, &height);

    _scene->draw(_renderer, width, height);
    _renderer.swapBuffers();
}

//! @brief Disposes of OpenGL rendering resources.
void MainWindow::cleanup()
{
    if (!_renderer.isBound() || !_scene)
        return;

    Ag::GL::RenderScope scope(_renderer);

    _scene.reset();
    _renderer.flushResources();
}

// Inherited from Ag::SDL::IDispatcherTarget.
void MainWindow::processEvent(Ag::SDL::InputManager &manager,
                              const SDL_Event &nextEvent)
{
    switch (nextEvent.type)
    {
    case SDL_WINDOWEVENT:
        if (nextEvent.window.event == SDL_WINDOWEVENT_CLOSE)
        {
            SDL_HideWindow(_sdlWindow.get());
            manager.requestExit(0);
        }
        break;

    case SDL_KEYDOWN:
        if (nextEvent.key.keysym.sym == SDLK_F5)
        {
            redraw();
        }
        break;
    }
}

//! @brief Constructs an object which can render to the window surface
//! using OpenGL
//! @param[in] context An object representing the OpenGL render context
//! which should be selected on the current thread.
MainWindow::Scene::Scene(Ag::GL::RenderContext &context)
{
    using namespace Ag::GL;

    ProgramBuilder progBuilder(context);
    progBuilder.addShader(gl::ShaderType::VertexShader,
                          "#version 420\n\n"
                          "in vec2 pos;\n\n"
                          "void main(void) {\n"
                          "    gl_Position = vec4(pos, 1.0, 1.0);"
                          "}");
    progBuilder.addShader(gl::ShaderType::FragmentShader,
                          "#version 420 core\n\n"
                          "out vec4 colour;\n\n"
                          "void main(void) {\n"
                          "    colour = vec4(0.0, 0.8, 1.0, 1.0);\n"
                          "}");

    ShaderProg = progBuilder.create();

    VertexSchemaBuilder vertexBuilder;
    vertexBuilder.setVertexSize(sizeof(glm::vec2));
    vertexBuilder.add2DPosition("pos", 0);

    VertexSchema schema = vertexBuilder.createSchema();
    VertexAttribMapping attribMapping = ShaderProg.createAttribMapping(schema);

    Vao = context.createVertexArray();
    Vao.select();
    Vertices = context.createVertexBuffer();
    Vertices.setSchema(schema);

    const glm::vec2 points[]{
        {  0.5f,  0.5f },
        { -0.5f,  0.5f },
        { -0.5f, -0.5f }
    };

    Vertices.fill(points, sizeof(points), gl::BufferUsage::StaticDraw);
    Vertices.define(attribMapping);
}

//! @brief Redraws the scene.
//! @param[in] context An object representing currently selected
//! OpenGL context.
//! @param[in] width The width of the display area in pixels.
//! @param[in] height The height of the display area in pixels.
void MainWindow::Scene::draw(Ag::GL::RenderContext &context,
                             int width, int height)
{
    const auto &gl = context.getAPI();

    // Define which bit of the window to draw in.
    gl.viewport(0, 0, width, height);

    glm::vec4 backgroundColour(0, 0, 0, 1);

    gl.clearBufferFV(gl::BufferEnum::Color, 0,
                     reinterpret_cast<float *>(&backgroundColour));

    Vao.select();
    ShaderProg.select();
    
    gl.drawArrays(gl::PrimitiveType::Triangles, 0, 3);
}

}} // namespace Mo::UI
////////////////////////////////////////////////////////////////////////////////

