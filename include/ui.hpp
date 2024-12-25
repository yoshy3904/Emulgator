#pragma once
#include <mutex>

#include "sfguil.hpp"

class Emulator;

class PixelGraphic : public ui::Graphic
{
private:
    sf::Texture texture;
    sf::Image image;
public:
    PixelGraphic(ui::Canvas& p_canvas);

    void create(int p_width, int p_heigth, const sf::Color& p_color = sf::Color::Black);
    void setPixel(int p_x, int p_y, const sf::Color& p_color);
};

class FunctionGraph : public ui::Widget
{
private:
    sf::VertexArray triangles;
    std::vector<sf::Vector2f> points;
    int current_x = 0;

    sf::RectangleShape rect;
public:
    FunctionGraph(ui::Canvas& p_canvas);

    void draw(sf::RenderTarget& p_target, sf::RenderStates p_states) const override;

    void setSize(const sf::Vector2f& p_size) override;

    void setNext(float y);

    void refresh();
};

class Debugger
{
private:
    ui::DockSpace dockspace;
    ui::Canvas& canvas;
    Emulator& emulator;

    ui::Panel* background_panel;
    ui::Graphic* background_graphic;
    sf::Texture background_graphic_texture;

    ui::Panel* window_panel;
    ui::Graphic* window_graphic;
    sf::Texture window_graphic_texture;

    ui::Panel* display_panel;
    ui::Graphic* display_graphic;
    sf::Texture display_graphic_texture;

    ui::Panel* cpu_panel;
    ui::TextField* cpu_text;

    ui::Panel* cartridge_panel;
    ui::TextField* cartridge_text;
    ui::Button* play_button;
    ui::TextField* speed_text;
    ui::TextField* metrics_text;
    std::array<int, 6> speed_options;

    ui::Panel* vram_panel;
    ui::TextField* vram_tile_data_header;
    ui::Graphic* vram_graphic;
    sf::Texture vram_texture;
    ui::Graphic* vram_tile_graphic;
    sf::Texture vram_tile_texture;
    ui::TextField* vram_tile_info;

    ui::Panel* oam_panel;
    struct OAMEntry
    {
        ui::Graphic* graphic;
        ui::TextField* text_field;
        sf::Texture texture;
        sf::Image image;
    };
    std::array<OAMEntry, 40> oam_entries;
    ui::TextField* oam_info;

    ui::Panel* ppu_panel;
    ui::TextField* ppu_info;

    ui::Panel* apu_panel;
    ui::TextField* apu_ch1;
    ui::TextField* apu_ch2;
    ui::TextField* apu_ch3;
    ui::TextField* apu_ch4;
    ui::TextField* apu_general;
    ui::TextField* volume_text;
    FunctionGraph* ch1_graph;
    FunctionGraph* ch2_graph;
    FunctionGraph* ch3_graph;
    FunctionGraph* ch4_graph;

    std::mutex& mutex;
public:
    Debugger(ui::Canvas& p_canvas, Emulator& p_emulator, std::mutex& p_mutex);

    void updateEvents(sf::Event& p_event);
    void updateLogic();
private:
    void init();
};