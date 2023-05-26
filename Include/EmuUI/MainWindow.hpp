//! @file EmuUI/MainWindow.hpp
//! @brief The declaration of an object representing the main application window
//! of the Mighty Oak emulator.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __EMU_UI_MAIN_WINDOW_HPP__
#define __EMU_UI_MAIN_WINDOW_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "Ag/SdlTools.hpp"
#include "Ag/ObjectGL.hpp"

namespace Mo {
namespace UI {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object representing the main application window of the Mighty Oak
//! emulator.
class MainWindow : public Ag::SDL::IDispatcherTarget
{
public:
    // Construction/Destruction
    MainWindow() = default;
    ~MainWindow();
    static std::shared_ptr<MainWindow> create();

    // Operations
    void registerHandlers(Ag::SDL::InputManager &inputManager);

    void initialise();
    void redraw();
    void cleanup();

    // Overrides
    void processEvent(Ag::SDL::InputManager &manager,
                      const SDL_Event &nextEvent) override;
private:
    // Internal Types
    struct Scene
    {
        Ag::GL::VertexArrayObject Vao;
        Ag::GL::VertexBuffer Vertices;
        Ag::GL::Program ShaderProg;

        Scene(Ag::GL::RenderContext &context);

        void draw(Ag::GL::RenderContext &context, int width, int height);
    };

    // Internal Fields
    Ag::SDL::WindowUPtr _sdlWindow;
    Ag::GL::DisplayContext _display;
    Ag::GL::RenderContext _renderer;
    std::unique_ptr<Scene> _scene;
};

using MainWindowSPtr = std::shared_ptr<MainWindow>;

}} // namespace Mo::UI

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////
