#pragma once

#include <string>

struct ImFont;

class GUI {
public:
    GUI() noexcept;
    void render() noexcept;
    bool isOpen{ false };
private:
    static void hotkey(int&) noexcept;
    void updateColors() const noexcept;
    void renderMenuBar() noexcept;
    void renderAimbotWindow() noexcept;
    void renderfastshotbotWindow() noexcept;
    void renderbodyoutlineWindow() noexcept;
    void rendercolouredbodyWindow() noexcept;
    void renderespWindow() noexcept;
    void renderVisualsWindow() noexcept;
    void renderSkinChangerWindow() noexcept;
    void renderMiscWindow() noexcept;
    void renderConfigWindow() noexcept;
    void renderGuiStyle2() noexcept;

    struct {
        bool aimhelper{ true };
        bool fastshotbot{ false };
        bool bodyoutline{ false };
        bool colouredbody{ false };
        bool esp{ false };
        bool visuals{ false };
        bool skinChanger{ false };
        bool misc{ false };
        bool config{ false };
    } window;

    struct {
        ImFont* tahoma = nullptr;
        ImFont* segoeui = nullptr;
        ImFont* tabfont = nullptr;
    } fonts;
};

extern GUI gui;
