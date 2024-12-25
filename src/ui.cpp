#include "ui.hpp"
#include "emulator.hpp"

#ifdef _WIN32
#include <Windows.h>

void OpenOpenFileDialog(std::wstring& fileStr)
{
    LPWSTR filebuff = new wchar_t[256];
    OPENFILENAMEW open = { 0 };
    open.lStructSize = sizeof(OPENFILENAMEW);
    open.hwndOwner = nullptr; //Handle to the parent window
    open.lpstrFilter = L"GB Files\0*.gb\0\0";//L"Image Files(.jpg|.png|.bmp|.jpeg)\0*.jpg;*.png;*.bmp;*.jpeg\0\0";
    open.lpstrCustomFilter = NULL;
    open.lpstrFile = filebuff;
    open.lpstrFile[0] = '\0';
    open.nMaxFile = 256;
    open.nFilterIndex = 1;
    open.lpstrInitialDir = NULL;
    open.lpstrTitle = L"Choose file to open\0";
    open.nMaxFileTitle = strlen("Choose file to open\0");
    open.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_HIDEREADONLY;
    open.lpstrDefExt = L"txt";

    if (GetOpenFileNameW(&open))
    {
        fileStr = filebuff;
    }
    delete[] filebuff;
}
#endif

PixelGraphic::PixelGraphic(ui::Canvas& p_canvas)
    : Graphic::Graphic(p_canvas)
{

}

void PixelGraphic::create(int p_width, int p_heigth, const sf::Color& p_color)
{
    //image.create(p_width, p_heigth, p_color);
}

void PixelGraphic::setPixel(int p_x, int p_y, const sf::Color& p_color)
{
    // if(image.getSize().x == 0 || image.getSize().y == 0)
    // {
    //     std::cout << "Image was not created!" << std::endl;
    //     return;
    // }
    // if(p_x < 0 || p_x >= image.getSize().x || p_y < 0 || p_y >= image.getSize().y)
    // {
    //     std::cout << "setPixel() out of bounds!" << std::endl;
    //     return;
    // }

    // image.setPixel(p_x, p_y, p_color);
    // texture.loadFromImage(image);
}

FunctionGraph::FunctionGraph(ui::Canvas& p_canvas)
    : Widget::Widget(p_canvas), triangles(sf::LinesStrip, 0)
{
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineColor(sf::Color::Black);
    rect.setOutlineThickness(1.f);
}

void FunctionGraph::draw(sf::RenderTarget& p_target, sf::RenderStates p_states) const 
{
    // Clipping.
    std::vector<sf::Glsl::Mat4> inverse_transforms;
    std::vector<sf::Glsl::Vec2> sizes;

    const Widget* current_parent = getMask();
    while (current_parent != nullptr)
    {
        inverse_transforms.push_back(current_parent->getWorldTransform().getInverse());
        sizes.push_back(current_parent->getSize());
        current_parent = current_parent->getMask();
    }

    canvas.getClippingShader().setUniform("array_size", (int)sizes.size());
    canvas.getClippingShader().setUniformArray("sizes", sizes.data(), sizes.size());
    canvas.getClippingShader().setUniformArray("inverse_transforms", inverse_transforms.data(), inverse_transforms.size());
    canvas.getClippingShader().setUniform("window_size", sf::Glsl::Vec2(window.getSize()));
    canvas.getClippingShader().setUniform("texture", sf::Shader::CurrentTexture);
    canvas.getClippingShader().setUniform("use_texture", false);

    window.draw(triangles, p_states);
    window.draw(rect, p_states);
}

void FunctionGraph::setSize(const sf::Vector2f& p_size)
{
    Widget::setSize(p_size);
    rect.setSize(p_size);
}

void FunctionGraph::setNext(float y)
{
    points.push_back(sf::Vector2f(current_x, y * getSize().y));
    current_x++;
    if(current_x > getSize().x)
    {
        current_x = getSize().x;
        points.erase(points.begin());
        for (int i = 0; i < points.size(); i++)
        {
            points[i].x--;
        }
    }
    refresh();
}

void FunctionGraph::refresh()
{
    triangles.resize(points.size());
    for (int i = 0; i < triangles.getVertexCount(); i++)
    {
        triangles[i].position = points[i];
        triangles[i].color = sf::Color::Green;
    }
    
}

Debugger::Debugger(ui::Canvas& p_canvas, Emulator& p_emulator, std::mutex& p_mutex) 
    : mutex(p_mutex),
    dockspace(p_canvas), 
    emulator(p_emulator),
    canvas(p_canvas)
{
    init();
}

void Debugger::init()
{
    // Display.
    display_panel = dockspace.root;
    display_panel->setTitle("Display");
    display_graphic = display_panel->createGraphic();
    display_graphic->setScale(sf::Vector2f(4.f, 4.f));

    // Background.
    background_panel = dockspace.createPanel("Background");
    background_graphic = background_panel->createGraphic();
    background_graphic->setScale(sf::Vector2f(4.f, 4.f));
    dockspace.insertPanel(background_panel, display_panel, 4);

    // Cartridge.
    cartridge_panel = dockspace.createPanel("Cartridge");
    play_button = cartridge_panel->createButton("Start", [this]()
    {
        emulator.setEnabled(!emulator.isEnabled());
        play_button->text.setString(emulator.isEnabled() ? "Stop" : "Start");
    });
    cartridge_panel->createButton("Step", [this]()
    {
        emulator.step();
    });
    cartridge_panel->createButton("Load Cartridge", [this]()
    {
        #ifdef _WIN32
        std::wstring file_string;
        OpenOpenFileDialog(file_string);
        emulator.loadCartridge(sf::String(file_string));
        #endif

        cartridge_text->setString(
            std::string("Title: ") + emulator.getMemory().getCartridge().title + "\n"
            "Type: " + emulator.getMemory().getCartridge().type_string + " (" + ui::toHexString(emulator.getMemory().getCartridge().type_code, true, 2, "0x") + ")\n"
            "ROM Size: " + emulator.getMemory().getCartridge().rom_size_string + " (" + ui::toHexString(emulator.getMemory().getCartridge().rom_size_code, true, 2, "0x") + ")\n"
            "RAM Size: " + emulator.getMemory().getCartridge().ram_size_string + " (" + ui::toHexString(emulator.getMemory().getCartridge().ram_size_code, true, 2, "0x") + ")\n");
    });
    cartridge_panel->breakLine();
    cartridge_panel->createButton("Reset", [this]()
    {
        emulator.reset();
    });
    cartridge_panel->breakLine();

    cartridge_panel->createButton("<", [this](){ if(emulator.getSpeed() <= 0) return; emulator.setSpeed(emulator.getSpeed() - 5); speed_text->setString(ui::toString(emulator.getSpeed()) + "%"); });
    speed_text = cartridge_panel->createText(ui::toString(emulator.getSpeed()) + "%");
    speed_text->setSize(sf::Vector2f(35.f, 20.f));
    cartridge_panel->createButton(">", [this](){ if(emulator.getSpeed() >= 1000) return; emulator.setSpeed(emulator.getSpeed() + 5); speed_text->setString(ui::toString(emulator.getSpeed()) + "%"); });
    cartridge_panel->breakLine();
    
    metrics_text = cartridge_panel->createText("");
    metrics_text->setSize(sf::Vector2f(500.f, 100.f));
    cartridge_panel->breakLine();
    cartridge_text = cartridge_panel->createText("");
    cartridge_text->setSize(sf::Vector2f(500.f, 100.f));
    cartridge_panel->breakLine();

    dockspace.insertPanel(cartridge_panel, display_panel, 1);

    // PPU.
    ppu_panel = dockspace.createPanel("PPU");
    ppu_info = ppu_panel->createText("");
    dockspace.insertPanel(ppu_panel, cartridge_panel, 2);

    // CPU.
    cpu_panel = dockspace.createPanel("CPU/Registers");
    cpu_text = cpu_panel->createText("");
    dockspace.insertPanel(cpu_panel, cartridge_panel, 1);

    // Window.
    window_panel = dockspace.createPanel("Window");
    dockspace.insertPanel(window_panel, display_panel, 4);
    window_graphic = window_panel->createGraphic();
    window_graphic->setScale(sf::Vector2f(4.f, 4.f));

    // VRAM viewer.
    vram_panel = dockspace.createPanel("VRAM Viewer");
    dockspace.insertPanel(vram_panel, background_panel, 2);
    vram_tile_data_header = vram_panel->createText("VRAM Tile Data (384 Tiles, 8x8 pixels, 0x8000-0x97FF)");
    vram_panel->breakLine();
    vram_graphic = vram_panel->createGraphic();
    vram_panel->breakLine();
    vram_tile_graphic = vram_panel->createGraphic();
    vram_tile_info = vram_panel->createText("");

    // OAM viewer.
    oam_panel = dockspace.createPanel("OAM Viewer");
    for (int i = 0; i < 40; i++)
    {
        oam_entries[i].graphic = oam_panel->createGraphic();
        oam_entries[i].graphic->setSize(sf::Vector2f(32.f, 32.f));
        oam_entries[i].text_field = oam_panel->createText("");
        oam_entries[i].text_field->enableWrapping(false);
        oam_entries[i].text_field->setSize(sf::Vector2f(40.f, 50.f));
        if((i + 1) % 8 == 0) oam_panel->breakLine();
    }
    oam_info = oam_panel->createText("");
    oam_info->enableWrapping(false);
    oam_info->setSize(sf::Vector2f(800.f, 80.f));
    dockspace.insertPanel(oam_panel, vram_panel, 4);

    // APU.
    apu_panel = dockspace.createPanel("APU");

    apu_general = apu_panel->createText("General");
    apu_general->setSize(sf::Vector2f(400.f, 90.f));
    apu_panel->breakLine();

    apu_panel->createButton("<", [this](){ emulator.setVolume(emulator.getVolume() - 5.f); volume_text->setString(ui::toString(emulator.getAPU().getVolume(), 0) + "%"); });
    volume_text = apu_panel->createText(ui::toString(emulator.getAPU().getVolume(), 0) + "%");
    volume_text->setSize(sf::Vector2f(35.f, 20.f));
    apu_panel->createButton(">", [this](){ emulator.setVolume(emulator.getVolume() + 5.f); volume_text->setString(ui::toString(emulator.getAPU().getVolume(), 0) + "%"); });
    apu_panel->breakLine();

    apu_ch1 = apu_panel->createText("Channel 1");
    apu_ch1->setSize(sf::Vector2f(400.f, 90.f));
    apu_panel->breakLine();

    ch1_graph = apu_panel->create<FunctionGraph>();
    ch1_graph->setSize(sf::Vector2f(sf::Vector2f(400.f, 40.f)));
    apu_panel->breakLine();

    apu_ch2 = apu_panel->createText("Channel 2");
    apu_ch2->setSize(sf::Vector2f(400.f, 70.f));
    apu_panel->breakLine();
    
    ch2_graph = apu_panel->create<FunctionGraph>();
    ch2_graph->setSize(sf::Vector2f(sf::Vector2f(400.f, 40.f)));
    apu_panel->breakLine();
    
    apu_ch3 = apu_panel->createText("Channel 3");
    apu_ch3->setSize(sf::Vector2f(400.f, 70.f));
    apu_panel->breakLine();

    ch3_graph = apu_panel->create<FunctionGraph>();
    ch3_graph->setSize(sf::Vector2f(sf::Vector2f(400.f, 40.f)));
    apu_panel->breakLine();
    
    apu_ch4 = apu_panel->createText("Channel 4");
    apu_ch4->setSize(sf::Vector2f(400.f, 70.f));
    apu_panel->breakLine();

    ch4_graph = apu_panel->create<FunctionGraph>();
    ch4_graph->setSize(sf::Vector2f(sf::Vector2f(400.f, 40.f)));
    apu_panel->breakLine();
    
    dockspace.insertPanel(apu_panel, ppu_panel, 4);
}

void Debugger::updateEvents(sf::Event& p_event)
{
    mutex.lock();
    if(p_event.type == sf::Event::KeyPressed)
    {
        if(p_event.key.code == sf::Keyboard::A) emulator.markKeyPressed(Input::Keys::A);
        if(p_event.key.code == sf::Keyboard::S) emulator.markKeyPressed(Input::Keys::B);
        if(p_event.key.code == sf::Keyboard::Down) emulator.markKeyPressed(Input::Keys::Down);
        if(p_event.key.code == sf::Keyboard::Up) emulator.markKeyPressed(Input::Keys::Up);
        if(p_event.key.code == sf::Keyboard::Left) emulator.markKeyPressed(Input::Keys::Left);
        if(p_event.key.code == sf::Keyboard::Right) emulator.markKeyPressed(Input::Keys::Right);
        if(p_event.key.code == sf::Keyboard::Space) emulator.markKeyPressed(Input::Keys::Start);
        if(p_event.key.code == sf::Keyboard::Enter) emulator.markKeyPressed(Input::Keys::Select);
    }
    if(p_event.type == sf::Event::KeyReleased)
    {
        if(p_event.key.code == sf::Keyboard::A) emulator.markKeyReleased(Input::Keys::A);
        if(p_event.key.code == sf::Keyboard::S) emulator.markKeyReleased(Input::Keys::B);
        if(p_event.key.code == sf::Keyboard::Down) emulator.markKeyReleased(Input::Keys::Down);
        if(p_event.key.code == sf::Keyboard::Up) emulator.markKeyReleased(Input::Keys::Up);
        if(p_event.key.code == sf::Keyboard::Left) emulator.markKeyReleased(Input::Keys::Left);
        if(p_event.key.code == sf::Keyboard::Right) emulator.markKeyReleased(Input::Keys::Right);
        if(p_event.key.code == sf::Keyboard::Space) emulator.markKeyReleased(Input::Keys::Start);
        if(p_event.key.code == sf::Keyboard::Enter) emulator.markKeyReleased(Input::Keys::Select);
    }
    mutex.unlock();
}

void Debugger::updateLogic()
{
    mutex.lock();

    // APU.
    for (int i = 0; i < emulator.getAPU().ch1_sample_list.size(); i+=8)
    {
        ch1_graph->setNext(emulator.getAPU().ch1_sample_list[i]/20000.f);
    }
    for (int i = 0; i < emulator.getAPU().ch2_sample_list.size(); i+=8)
    {
        ch2_graph->setNext(emulator.getAPU().ch2_sample_list[i]/20000.f);
    }
    for (int i = 0; i < emulator.getAPU().ch3_sample_list.size(); i+=8)
    {
        ch3_graph->setNext(emulator.getAPU().ch3_sample_list[i]/20000.f);
    }
    for (int i = 0; i < emulator.getAPU().ch4_sample_list.size(); i+=8)
    {
        ch4_graph->setNext(emulator.getAPU().ch4_sample_list[i]/20000.f);
    }

    std::array<sf::String, 4> duty_cycle_string = { "12,5%", "25%", "50%", "75%" };
    std::array<sf::String, 3> audio_status = { "Stopped", "Paused", "Playing" };

    apu_general->setString(sf::String("General\n") +
    "Audio enabled: " + audio_status[emulator.getAPU().getStatus()] + "\n"
    "NR52 (0xff26): " + ui::toBinaryString(emulator.getMemory().read(0xff26)) + "\n"
    "div_apu: " + ui::toHexString(emulator.getAPU().div_apu, true, 2) + "\n" +
    "dots/seconds: " + ui::toString(emulator.getAPU().cycle_count_per_second) + "\n");

    apu_ch1->setString(sf::String("Channel 1: ") + (emulator.getAPU().ch1_active ? "Active" : "Inactive") + "\n"
    "Volume: " + ui::toHexString(emulator.getAPU().ch1_volume, true, 2) + "\n"
    "Duration: " + ui::toHexString((emulator.getMemory().read(0xff14) & 0b111) << 8 | emulator.getMemory().read(0xff13)) + " "
    "Duty cycle: " + duty_cycle_string[emulator.getMemory().read(0xff11) >> 6] + "\n"
    "Duration sweep pace: " + ui::toHexString(emulator.getAPU().ch1_duration_sweep_pace, true, 2) + "\n"
    "Length timer: " + ui::toHexString(emulator.getAPU().ch1_length_timer, true, 2) + "\n");

    apu_ch2->setString(sf::String("Channel 2: ") + (emulator.getAPU().ch2_active ? "Active" : "Inactive") + "\n"
    "Volume: " + ui::toHexString(emulator.getAPU().ch2_volume, true, 2) + "\n"
    "Duration: " + ui::toHexString((emulator.getMemory().read(0xff19) & 0b111) << 8 | emulator.getMemory().read(0xff18)) + ""
    "Duty cycle: " + duty_cycle_string[emulator.getMemory().read(0xff16) >> 6] + "\n"
    "Length timer: " + ui::toHexString(emulator.getAPU().ch2_length_timer, true, 2) + "\n");

    sf::String wave_ram;
    for (int i = 0; i < 16; i++)
    {
        wave_ram += ui::toHexString(emulator.getMemory().read(0xff30 + i), true, 2) + " ";
    }

    apu_ch3->setString(sf::String("Channel 3: ") + (emulator.getAPU().ch3_active ? "Active" : "Inactive") + "\n"
    "Wave RAM: " + wave_ram + "\n");

    apu_ch4->setString(sf::String("Channel 4: ") + (emulator.getAPU().ch4_active ? "Active" : "Inactive") + "\n"
    "Volume: " + ui::toHexString(emulator.getAPU().ch4_volume, true, 2) + "\n"
    "Clock shift: " + ui::toHexString(emulator.getMemory().read(0xff22) >> 4, true, 2) + " "
    "Clock divider: " + ui::toHexString(emulator.getMemory().read(0xff22) & 0b111, true, 2) + "\n"
    "Length timer: " + ui::toHexString(emulator.getAPU().ch2_length_timer, true, 2) + "\n");

    // Display.
    sf::Image background_image;
    sf::Image window_image;
    background_image.create(emulator.getPPU().actual_screen_width, emulator.getPPU().actual_screen_height);
    window_image.create(emulator.getPPU().actual_screen_width, emulator.getPPU().actual_screen_height);
    for (int i = 0; i < emulator.getPPU().actual_screen_width; i++)
    {
        for (int j = 0; j < emulator.getPPU().actual_screen_height; j++)
        {
            int color_index = emulator.getPPU().background_buffer[j * emulator.getPPU().actual_screen_width + i];
            background_image.setPixel(i, j, (sf::Color)emulator.getPPU().getColorPalette()[color_index]);   

            color_index = emulator.getPPU().window_buffer[j * emulator.getPPU().actual_screen_width + i];
            window_image.setPixel(i, j, (sf::Color)emulator.getPPU().getColorPalette()[color_index]);
        }
    }

    background_graphic_texture.loadFromImage(background_image);
    background_graphic->setTexture(&background_graphic_texture);

    window_graphic_texture.loadFromImage(window_image);
    window_graphic->setTexture(&window_graphic_texture);

    sf::Image display_image;
    display_image.create(emulator.getPPU().screen_width, emulator.getPPU().screen_height);
    for (int i = 0; i < emulator.getPPU().screen_width; i++)
    {
        for (int j = 0; j < emulator.getPPU().screen_height; j++)
        {
            int color_index = emulator.getPPU().screen_buffer[j * emulator.getPPU().screen_width + i];
            display_image.setPixel(i, j, (sf::Color)emulator.getPPU().getColorPalette()[color_index]);    
        }
    }
    
    display_graphic_texture.loadFromImage(display_image);
    display_graphic->setTexture(&display_graphic_texture);

    // Cartridge.
    metrics_text->setString("dots/sec: " + ui::toString(emulator.cycle_count_per_second) + "\n"
    "Speed: " + ui::toString((emulator.cycle_count_per_second / 4194304.f) * 100.f, 1) + "%\n"
    "Cycle budget: " + ui::toString(emulator.cycle_budget, 1) + "\n");

    // CPU.
    cpu_text->setString(
        "af: " + ui::toHexString(emulator.getCPU().a << 4 | emulator.getCPU().flags, true, 4, "0x") + "\n"
        "bc: " + ui::toHexString(emulator.getCPU().b << 4 | emulator.getCPU().c, true, 4, "0x") + "\n"
        "de: " + ui::toHexString(emulator.getCPU().d << 4 | emulator.getCPU().e, true, 4, "0x") + "\n"
        "de: " + ui::toHexString(emulator.getCPU().h << 4 | emulator.getCPU().l, true, 4, "0x") + "\n"
        "sp: " + ui::toHexString(emulator.getCPU().sp, true, 4, "0x") + "\n"
        "pc: " + ui::toHexString(emulator.getCPU().pc, true, 4, "0x") + "\n"
        "flags: " + ui::toBinaryString(emulator.getCPU().flags, 8, "0b") + "\n"
            + "   zero z        = " + ((emulator.getCPU().flags & 0b10000000) ? "true" : "false") + "\n"
            + "   subtraction n = " + ((emulator.getCPU().flags & 0b01000000) ? "true" : "false") + "\n"
            + "   half carry h  = " + ((emulator.getCPU().flags & 0b00100000) ? "true" : "false") + "\n"
            + "   carry c       = " + ((emulator.getCPU().flags & 0b00010000) ? "true" : "false") + "\n"
        "JOYP (0xff00):" + ui::toHexString(emulator.getMemory().read(0xff00), true, 2, "0x") + " - " + ui::toBinaryString(emulator.getMemory().read(0xff00), 8, "0b"));
    cpu_text->setSize(cpu_panel->getSize());

    // VRAM (0x8000-0x97ff -> There are 384 tiles, each 8x8 pixels)
    sf::Image vram_image;
    int tiles_in_column = 16;
    int tiles_in_row = 384/tiles_in_column;
    int vram_image_width = tiles_in_row * 8;
    int vram_image_heigth = tiles_in_column * 8;
    vram_image.create(vram_image_width, vram_image_heigth);

    for (int i = 0; i < tiles_in_row; i++)
    {
        for (int j = 0; j < tiles_in_column; j++)
        {
            std::vector<uint8_t> pixels(64, 0x00);
            emulator.getPPU().getTile(0x8000 + (i * 16 + j * tiles_in_row * 16), pixels);
            for (int k = 0; k < 64; k++)
            {
                vram_image.setPixel(i * 8 + k % 8, j * 8 + (int)(k / 8), (sf::Color)emulator.getPPU().getColorPalette()[pixels[k]]);
            }
        }
    }

    // Highlight individual tiles.
    sf::Image vram_tile_image;
    vram_tile_image.create(8, 8);

    sf::Vector2f mouse_position = ui::getMousePosition(canvas.window, &canvas.getView());
    sf::Vector2f relative_mouse_position = (mouse_position - vram_graphic->getWorldTransform().transformPoint(sf::Vector2f()))/2.f;
    if(relative_mouse_position.x >= 0 && relative_mouse_position.x < vram_image_width && relative_mouse_position.y >= 0 && relative_mouse_position.y < vram_image_heigth)
    {
        sf::Vector2i tile_position = sf::Vector2i(relative_mouse_position.x / 8, relative_mouse_position.y / 8);
        for (int k = 0; k < 64; k++)
        {
            sf::Color pixel = vram_image.getPixel(tile_position.x * 8 + k % 8, tile_position.y * 8 + (int)(k / 8));
            vram_image.setPixel(tile_position.x * 8 + k % 8, tile_position.y * 8 + (int)(k / 8), sf::Color(pixel.r, pixel.g, pixel.b, 100));
            vram_tile_image.setPixel(k % 8, k / 8, pixel);
        }
        vram_tile_info->setString("Tile at " + ui::toHexString(0x8000 + (tile_position.x * 16 + tile_position.y * tiles_in_row * 16), true) + "\n"
        + "x=" + ui::toString(tile_position.x) + ", y=" + ui::toString(tile_position.y));
    }

    vram_tile_texture.loadFromImage(vram_tile_image);
    vram_tile_graphic->setTexture(&vram_tile_texture);
    vram_tile_graphic->setSize(sf::Vector2f(64, 64));
    vram_tile_info->setSize(sf::Vector2f(128, 64));

    vram_texture.loadFromImage(vram_image);
    vram_graphic->setTexture(&vram_texture);
    vram_graphic->setPreserveAspect(false);
    vram_graphic->setSize(sf::Vector2f(vram_image_width * 2.f, vram_image_heigth * 2.f));

    vram_tile_data_header->setSize(sf::Vector2f(vram_panel->getSize().x, 20.f));

    // OAM entries. 
    for (int i = 0; i < 40; i++)
    {
        oam_entries[i].image.create(8, 8);
        
        // Retrieve oam tiles. They are located at 0xFE00, each entry is 4 bytes long.
        std::vector<uint8_t> tile;
        emulator.getPPU().getTile(0x8000 + emulator.getMemory().read(0xFE02 + i * 4, false) * 16, tile);
        for (int j = 0; j < 64; j++)
        {
            oam_entries[i].image.setPixel(j % 8, j / 8, (sf::Color)emulator.getPPU().getColorPalette()[tile[j]]);
        }
        oam_entries[i].text_field->setString(ui::toHexString(emulator.getMemory().read(0xFE02 + i * 4, false), true, 2) + "\n"
        + ui::toHexString(emulator.getMemory().read(0xFE01 + i * 4, false), true, 2) + "\n"
        + ui::toHexString(emulator.getMemory().read(0xFE00 + i * 4, false), true, 2) + "\n");
        
        oam_entries[i].texture.loadFromImage(oam_entries[i].image);
        oam_entries[i].graphic->setTexture(&oam_entries[i].texture);

        if(oam_entries[i].graphic->isHovered())
        {
            uint8_t info_byte = emulator.getMemory().read(0xFE03 + i * 4, false);
            oam_info->setString("Entry " + ui::toString(i) + ": " + "BG/Window over OBJ = " + (info_byte >> 7 & 1 ? "Yes" : "No") + ", "
            "Y flip = " + (info_byte >> 6 & 1 ? "Yes" : "No") + ", "
            "X flip = " + (info_byte >> 5 & 1 ? "Yes" : "No") + ", "
            "Palette number = " + (info_byte >> 4 & 1 ? "OBP1" : "OBP0"));
        }
    }

    // PPU.
    std::array<sf::String, 4> ppu_mode_strings = { "HBLANK", "VBLANK", "OAMSCAN", "DRAWING PIXELS" };
    sf::String ppu_info_string = "Cycles: " + ui::toString(emulator.getPPU().getCycleCount()) + "\n"
    "Current PPU Mode: " + (ppu_mode_strings[emulator.getPPU().getLCDMode()]) + "\n"
    "LY (0xff44): " + ui::toHexString(emulator.getMemory().read(0xff44), true, 2, "0x") + "\n"
    "LYC (0xff45): " + ui::toHexString(emulator.getMemory().read(0xff45), true, 2, "0x") + "\n"
    "Scroll: x=" + ui::toHexString(emulator.getPPU().scroll_x, true, 2) + " y=" + ui::toHexString(emulator.getPPU().scroll_y, true, 2) + "\n"
    "Fetcher: x=" + ui::toString(emulator.getPPU().fetcher_x) + " y=" + ui::toString(emulator.getPPU().fetcher_y) + "\n\n";
    ppu_info_string += "BG_AND_WINDOW_ENABLE: " + ui::toHexString(emulator.getPPU().getLCDCBit(PPU::BG_AND_WINDOW_ENABLE), true, 2) + "\n";
    ppu_info_string += "IE: " + ui::toBinaryString(emulator.getMemory().read(0xffff, false), 8) + "\n";
    ppu_info_string += "IF: " + ui::toBinaryString(emulator.getMemory().read(0xff0f, false), 8) + "\n";
    ppu_info_string += "Window: x=" + ui::toHexString(emulator.getPPU().window_x, true, 2) + " y=" + ui::toHexString(emulator.getPPU().window_y, true, 2) + "\n";
    ppu_info_string += sf::String("Window triggered: ") + (emulator.getPPU().window_triggered ? "True" : "False") + "\n";
    ppu_info_string += sf::String("Window internal counter: ") + ui::toHexString(emulator.getPPU().window_internal_counter, true, 2) + "\n";
    ppu_info_string += "Pixels to erase: " + ui::toHexString(emulator.getPPU().scroll_x % 8, true, 2) + "\n";
    
    ppu_info_string += "Objects on scanline: " + ui::toString(emulator.getPPU().objects_on_scanline.size()) + "\n";
    for (int i = 0; i < emulator.getPPU().objects_on_scanline.size(); i++)
    {
        ppu_info_string += ui::toString(i) + ". x=" + std::to_string(emulator.getPPU().objects_on_scanline[i].x) + " y=" + std::to_string(emulator.getPPU().objects_on_scanline[i].y) + "\n";
    }
    ppu_info_string += "FIFO size: " + ui::toString(emulator.getPPU().fifo.size()) + "\n";
    for (int i = 0; i < emulator.getPPU().fifo.size(); i++)
    {
        ppu_info_string += ui::toString(i) + ". color=" + ui::toString(emulator.getPPU().fifo[i].color) + "\n";
    }
    ppu_info_string += emulator.getPPU().info_text;
    
    
    ppu_info->setString(ppu_info_string);
    ppu_info->setSize(sf::Vector2f(ppu_panel->getSize()));

    mutex.unlock();
}