#pragma once
#include "sfguil.hpp"
#include "emulator.hpp"

#if defined(_WIN32)
#include <windows.h>
// https://docs.microsoft.com/en-us/windows/win32/api/commdlg/ns-commdlg-openfilenamea.
void createOpenFileDialog(sf::String& p_file_path_output, const sf::String& p_filter);
#endif

class Display : public ui::Widget
{
private:
    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;

    const std::vector<uint8_t>* buffer = nullptr;
    Emulator& emulator;
public:
    Display(Emulator& p_emulator, ui::Canvas& p_canvas);

    void setBuffer(const std::vector<uint8_t>* p_buffer, const sf::Vector2f& p_dimensions);

    void setSize(const sf::Vector2f& p_size);

    void draw(sf::RenderTarget& p_target, sf::RenderStates p_states) const override;
    void updateLogic() override;
};

class MemoryViewer : public ui::Widget
{
private:
    ui::Graphic background;
    Emulator& emulator;
    ui::TextField text;

    int current_start_address = 0x0000;
public:
    MemoryViewer(Emulator& p_emulator, ui::Canvas& p_canvas, sf::Font& p_font);

    void setSize(const sf::Vector2f& p_size) override;
    void setVisible(bool p_state) override;

    void updateEvents(sf::Event& p_event) override;
    void updateLogic() override;
private:
    void showMemory();
};

class TileViewer : public ui::Widget
{
public:
    enum MemoryType
    {
        Internal, MBC1_ROM
    };
private:
    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;
    ui::TextField text_field;
    ui::Button save_image_button;
    ui::Button save_2bpp_button;

    int start_address = 0x0000;

    MemoryType memory_type = Internal;

    Emulator& emulator;
public:
    TileViewer(Emulator& p_emulator, ui::Canvas& p_canvas, sf::Font& p_font, const sf::Vector2f& p_dimensions);

    void setSize(const sf::Vector2f& p_size) override;
    void setVisible(bool p_state) override;

    void setMemoryType(MemoryType p_memory_type);
    void setStartAddress(int p_start_address);

    void draw(sf::RenderTarget& p_target, sf::RenderStates p_states) const override;
    void updateEvents(sf::Event& p_event) override;
    void updateLogic() override;
private:
    void display();
    void save2bppTextFile(const sf::String& p_file_path);
};

class OAMViewer : public ui::Widget
{
private:
    ui::Graphic background;

    Emulator& emulator;
    sf::Font& font;
public:
    OAMViewer(Emulator& p_emulator, ui::Canvas& p_canvas, sf::Font& p_font);

    void setSize(const sf::Vector2f& p_size);
};

class MemoryDebugger : public ui::Widget
{
private:
    ui::Graphic background;
    MemoryViewer memory_viewer;
    ui::TextField register_info;

    Emulator& emulator;
public:
    MemoryDebugger(Emulator& p_emulator, ui::Canvas& p_canvas, sf::Font& p_font);

    void setSize(const sf::Vector2f& p_size);
    void setVisible(bool p_state) override;

    void updateLogic();
};

class PPUDebugger : public ui::Widget
{
private:
    ui::Graphic background;
    TileViewer vram_viewer;
    Display background_display;
    Display window_display;
    ui::TextField info_text;
    OAMViewer oam_viewer;
    Emulator& emulator;
public:
    PPUDebugger(Emulator& p_emulator, ui::Canvas& p_canvas, sf::Font& p_font);

    void setSize(const sf::Vector2f& p_size);
    void setVisible(bool p_state) override;

    void updateLogic();
};

class Disassembler : public ui::Widget
{
private:
    ui::Graphic background;
    ui::TextField text_field;

    Emulator& emulator;
public:
    Disassembler(Emulator& p_emulator, ui::Canvas& p_canvas, sf::Font& p_font);

    void setSize(const sf::Vector2f& p_size);
    void setVisible(bool p_state) override;

    sf::String disassembleAddress(uint16_t p_address);
    int getInstructionLength(uint16_t p_address);

    void updateLogic();
};

class TabMenu : public ui::Widget
{
private:
    ui::Graphic background;
    std::vector<ui::Button*> buttons;
    std::vector<ui::Widget*> widgets;
    sf::Font& font;
public:
    TabMenu(ui::Canvas& p_canvas, sf::Font& p_font);
    ~TabMenu();

    // Add a tab with a name and a widget that will be shown when the tab is selected. p_widget can be nullptr.
    void addTab(Widget* p_widget, const sf::String& p_tab_name);

    void setSize(const sf::Vector2f& p_size) override;
};

class Debugger : public ui::Widget
{
private:
    TabMenu tab_menu;
    ui::Button enable_button;
    ui::Button step_button;
    ui::Button open_button;
    ui::TextField cartridge_info;
    ui::TextField joypad_info;

    Display display;
    MemoryDebugger memory_debugger;
    PPUDebugger ppu_debugger;
    Disassembler disassembler;

    Emulator& emulator;
public:
    Debugger(Emulator& p_emulator, ui::Canvas& p_canvas, sf::Font& p_font);

    void setSize(const sf::Vector2f& p_size) override;
    void updateEvents(sf::Event& p_event) override;
    void updateLogic() override;
};