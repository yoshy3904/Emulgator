#include "ui.hpp"

#include <fstream>

#if defined(_WIN32)
void createOpenFileDialog(sf::String& p_file_path_output, const sf::String& p_filter)
{
    LPWSTR filebuff = new wchar_t[256];
    OPENFILENAMEW open = { 0 };
    open.lStructSize = sizeof(OPENFILENAMEW);
    open.hwndOwner = nullptr; //Handle to the parent window
    open.lpstrFilter = p_filter.toWideString().c_str();//L"Image Files(.jpg|.png|.bmp|.jpeg)\0*.jpg;*.png;*.bmp;*.jpeg\0\0";
    open.lpstrCustomFilter = NULL;
    open.lpstrFile = filebuff;
    open.lpstrFile[0] = '\0';
    open.nMaxFile = 256;
    open.nFilterIndex = 1;
    open.lpstrInitialDir = NULL;
    open.lpstrTitle = L"Datei zum Öffnen auswählen\0";
    open.nMaxFileTitle = strlen("Datei zum Öffnen auswählen\0");
    open.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_HIDEREADONLY;
    open.lpstrDefExt = L"txt";

    if (GetOpenFileNameW(&open))
    {
        p_file_path_output = filebuff;
    }
    delete[] filebuff;
}
#endif

Display::Display(Emulator& p_emulator, ui::Canvas& p_canvas)
    : Widget::Widget(p_canvas), emulator(p_emulator)
{
    
}

void Display::setBuffer(const std::vector<uint8_t>* p_buffer, const sf::Vector2f& p_dimensions)
{
    if(buffer != nullptr) return;
    if(p_buffer->size() != p_dimensions.x * p_dimensions.y)
    {
        std::cout << "Buffer of size: " << p_buffer->size() << " doesn't match dimensions given: " << p_dimensions.x << ", " << p_dimensions.y << std::endl;
        return;
    }

    buffer = p_buffer;
    image.create(p_dimensions.x, p_dimensions.y, sf::Color::White);
    texture.loadFromImage(image);
    sprite.setTexture(texture);
}

void Display::setSize(const sf::Vector2f& p_size)
{
    Widget::setSize(p_size);
    sprite.setScale(p_size.x/(float)image.getSize().x, p_size.y/(float)image.getSize().y);
}

void Display::draw(sf::RenderTarget& p_target, sf::RenderStates p_states) const
{
    // Clipping.
    std::vector<sf::Glsl::Mat4> inverse_transforms;
    std::vector<sf::Glsl::Vec2> sizes;

    const Widget* current_mask = getMask();
    while (current_mask != nullptr)
    {
        inverse_transforms.push_back(current_mask->getWorldTransform().getInverse());
        sizes.push_back(current_mask->getSize());
        current_mask = current_mask->getMask();
    }

    canvas.getClippingShader().setUniform("array_size", (int)sizes.size());
    canvas.getClippingShader().setUniformArray("sizes", sizes.data(), sizes.size());
    canvas.getClippingShader().setUniformArray("inverse_transforms", inverse_transforms.data(), inverse_transforms.size());
    canvas.getClippingShader().setUniform("window_size", sf::Glsl::Vec2(window.getSize()));
    canvas.getClippingShader().setUniform("texture", sf::Shader::CurrentTexture);
    canvas.getClippingShader().setUniform("use_texture", true);

    // Drawing.
    p_target.draw(sprite, p_states);
}

void Display::updateLogic()
{
    if(buffer == nullptr) return;

    for (int i = 0; i < buffer->size(); i++)
    {
        if((*buffer)[i] >= 4) 
        {
            std::cout << "Buffer contains invalid data: " << (int)(*buffer)[i] << std::endl;
            continue;
        }
        image.setPixel(i % image.getSize().x, (int)(i / image.getSize().x), sf::Color(emulator.getPPU().getColorPalette()[(*buffer)[i]]));
    }
    texture.update(image);
    sprite.setTexture(texture);
}

TabMenu::TabMenu(ui::Canvas& p_canvas, sf::Font& p_font)
    : Widget::Widget(p_canvas), background(p_canvas), font(p_font)
{
    addChild(&background);

    background.setFillColor(sf::Color(240, 240, 240));
    background.setOutlineThickness(0.f);
    setSize(sf::Vector2f(400.f, 100.f));
}

TabMenu::~TabMenu()
{
    widgets.clear();
    for (int i = 0; i < buttons.size(); i++)
    {
        delete buttons[i];
    }
    buttons.clear();
}

void TabMenu::addTab(Widget* p_widget, const sf::String& p_tab_name)
{
    ui::Button* button = new ui::Button(canvas, font);
    button->text.setString(p_tab_name);
    button->setOnClickEvent([this, p_widget, button]()
    {
        // Hide all widgets that are associated to the buttons.
        for (int i = 0; i < widgets.size(); i++)
        {
            if(widgets[i] == nullptr) continue;
            widgets[i]->setVisible(false);
        }
        // Show the widget associated to the button that was clicked.
        if(p_widget != nullptr)
        {
            p_widget->setVisible(true);
        }
        // Highlight buttons.
        for (int i = 0; i < buttons.size(); i++)
        {
            buttons[i]->setStandardColor(sf::Color(225, 225, 225));
        }
        button->setStandardColor(sf::Color(255, 255, 255));
    });

    addChild(button);
    buttons.push_back(button);
    widgets.push_back(p_widget);
    if(p_widget != nullptr)
    {
        p_widget->setVisible(false);
    }

    setSize(getSize());
}

void TabMenu::setSize(const sf::Vector2f& p_size)
{
    Widget::setSize(p_size);
    background.setSize(p_size);

    float y = 0.f;
    float x = 0.f;
    for (int i = 0; i < buttons.size(); i++)
    {
        buttons[i]->setSize(sf::Vector2f(145.f, 20.f));
        buttons[i]->setPosition(x * 145.f, y * 20.f);
        buttons[i]->text.setHorizontalAlignment(ui::TextField::Middle);
        x++;
        if((buttons[i]->getPosition().x + buttons[i]->getSize().x + 145.f) > p_size.x)
        {
            x = 0;
            y++;
        }
    }
}

MemoryViewer::MemoryViewer(Emulator& p_emulator, ui::Canvas& p_canvas, sf::Font& p_font)
    : Widget::Widget(p_canvas),
    emulator(p_emulator),
    background(p_canvas),
    text(p_canvas, p_font)
{
    addChild(&background);
    addChild(&text);
    background.setFillColor(sf::Color(15, 15, 15));
    text.setTextColor(sf::Color::White);
    text.setCharacterSize(15);
    text.setParagraphSpacing(1.f);
    text.setVerticalAlignment(ui::TextField::Middle);
    text.enableWrapping(false);
    text.setMask(&background);
}

void MemoryViewer::setSize(const sf::Vector2f& p_size)
{
    Widget::setSize(p_size);
    background.setSize(p_size);

    text.setPosition(10.f, 0.f);
    text.setSize(sf::Vector2f(p_size.x - 20.f, p_size.y));
}

void MemoryViewer::setVisible(bool p_state)
{
    Widget::setVisible(p_state);
    background.setVisible(p_state);
    text.setVisible(p_state);
}

void MemoryViewer::updateEvents(sf::Event& p_event)
{
    if(p_event.type == sf::Event::MouseWheelScrolled)
    {
        int change_amount = 0x0010;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
        {
            change_amount = 0x0400;
        }
        if(p_event.mouseWheelScroll.delta > 0.f)
        {
            current_start_address -= change_amount;
        }
        if(p_event.mouseWheelScroll.delta < 0.f)
        {
            current_start_address += change_amount;
        }
    }
}

void MemoryViewer::updateLogic()
{
    showMemory();
}

void MemoryViewer::showMemory()
{
    int rows = getSize().y/17;
    int columns = 16;
    int length = columns * rows;

    int start_address = current_start_address;
    if(start_address < 0x0000) 
    {
        start_address = 0x0000;
    }
    else if(start_address > 0x10000 - length) 
    {
        start_address = 0x10000 - length;
    }
    current_start_address = start_address;

    sf::String string = ui::toHexString(start_address, true, 4) + " - ";

    for (int i = 0; i < length; i++)
    {
        if(i % columns == 0 && i != 0)
        {
            string += "\n" + ui::toHexString(start_address + i, true, 4) + " - ";
        }

        string += ui::toHexString(emulator.getMemory().read(start_address + i)) + " ";
    }

    text.setString(string);
}

TileViewer::TileViewer(Emulator& p_emulator, ui::Canvas& p_canvas, sf::Font& p_font, const sf::Vector2f& p_dimensions)
    : Widget::Widget(p_canvas),
    text_field(p_canvas, p_font),
    save_image_button(p_canvas, p_font),
    save_2bpp_button(p_canvas, p_font),
    emulator(p_emulator)
{
    addChild(&text_field);
    addChild(&save_image_button);
    addChild(&save_2bpp_button);

    image.create(p_dimensions.x, p_dimensions.y, sf::Color::White);
    texture.loadFromImage(image);
    sprite.setTexture(texture);

    text_field.setTextColor(sf::Color::Black);
    text_field.setString("");
    text_field.setParagraphSpacing(1.f);

    save_image_button.text.setString("Save image");
    save_image_button.text.setHorizontalAlignment(ui::TextField::Middle);
    save_image_button.setOnClickEvent([this]()
    {
        image.saveToFile("../Screenshot-" + emulator.getMemory().getCartridge().title + "-" + ui::toHexString(start_address, false, 6) + ".png");
    });

    save_2bpp_button.text.setString("Save 2bpp text file");
    save_2bpp_button.text.setHorizontalAlignment(ui::TextField::Middle);
    save_2bpp_button.setOnClickEvent([this]()
    {
        save2bppTextFile("../2bpp-" + emulator.getMemory().getCartridge().title + "-" + ui::toHexString(start_address, false, 6) + ".asm");
    });
}

void TileViewer::setSize(const sf::Vector2f& p_size)
{
    Widget::setSize(p_size);

    text_field.setSize(sf::Vector2f(p_size.x, 50.f));
    save_image_button.setSize(sf::Vector2f(p_size.x, 20.f));
    save_2bpp_button.setSize(sf::Vector2f(p_size.x, 20.f));

    text_field.setPosition(0.f, p_size.y - text_field.getSize().y - save_image_button.getSize().y);
    save_image_button.setPosition(0.f, text_field.getPosition().y + text_field.getSize().y);
    save_2bpp_button.setPosition(0.f, save_image_button.getPosition().y + save_image_button.getSize().y);

    sprite.setScale(sf::Vector2f(p_size.x / texture.getSize().x, (p_size.y - text_field.getSize().y - save_image_button.getSize().y) / texture.getSize().y));
}

void TileViewer::setVisible(bool p_state)
{
    Widget::setVisible(p_state);
    text_field.setVisible(p_state);
    save_image_button.setVisible(p_state);
    save_2bpp_button.setVisible(p_state);
}

void TileViewer::setMemoryType(MemoryType p_memory_type)
{
    memory_type = p_memory_type;
}

void TileViewer::setStartAddress(int p_start_address)
{
    start_address = p_start_address;
}

void TileViewer::draw(sf::RenderTarget& p_target, sf::RenderStates p_states) const
{
    // Clipping.
    std::vector<sf::Glsl::Mat4> inverse_transforms;
    std::vector<sf::Glsl::Vec2> sizes;

    const Widget* current_mask = getMask();
    while (current_mask != nullptr)
    {
        inverse_transforms.push_back(current_mask->getWorldTransform().getInverse());
        sizes.push_back(current_mask->getSize());
        current_mask = current_mask->getMask();
    }

    canvas.getClippingShader().setUniform("array_size", (int)sizes.size());
    canvas.getClippingShader().setUniformArray("sizes", sizes.data(), sizes.size());
    canvas.getClippingShader().setUniformArray("inverse_transforms", inverse_transforms.data(), inverse_transforms.size());
    canvas.getClippingShader().setUniform("window_size", sf::Glsl::Vec2(window.getSize()));
    canvas.getClippingShader().setUniform("texture", sf::Shader::CurrentTexture);
    canvas.getClippingShader().setUniform("use_texture", true);

    // Drawing.
    p_target.draw(sprite, p_states);
}

void TileViewer::updateEvents(sf::Event& p_event)
{
    if(p_event.type == sf::Event::MouseWheelScrolled)
    {
        int change_amount = 1;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
        {
            change_amount = 0x4000;
        }
        if(p_event.mouseWheelScroll.delta > 0.f)
        {
            start_address -= change_amount;
        }
        if(p_event.mouseWheelScroll.delta < 0.f)
        {
            start_address += change_amount;
        }
    }
}

void TileViewer::updateLogic()
{
    display();
}

void TileViewer::display()
{
    int rows = image.getSize().y/8;
    int columns = image.getSize().x/8;
    int tile_amount = (image.getSize().x/8) * (image.getSize().y/8);
    int x = 0, y = 0;

    // Get memory size.
    int memory_size = 0;
    if(memory_type == MemoryType::Internal)
    {
        memory_size = 0x10000;
    }
    else if (memory_type == MemoryType::MBC1_ROM)
    {
        memory_size = emulator.getMemory().getCartridge().mbc1.getRom().size();
    }

    // Out of bounds check.
    if(tile_amount * 16 >= memory_size)
    {
        return;
    }
    if(start_address + tile_amount * 16 >= memory_size)
    {
        start_address = memory_size - tile_amount * 16;
    }
    if(start_address < 0)
    {
        start_address = 0;
    }

    text_field.setString(
        L"Start Address: " + ui::toHexString(start_address, false, 6) + L"\n" + L"Length: " + 
        ui::toHexString(tile_amount * 16, false, 4) + " (" + ui::toString(tile_amount * 16) + L")\n");

    // Interpret memory as 2bpp image data.
    for (int i = 0; i < tile_amount; i++)
    {
        if(i % columns == 0 && i != 0)
        {
            y += 8;
            x = 0;
        }

        // Interpret a single tile of 16 bytes of memory.
        int k = 0;
        for (int j = 0; j < 16; j+=2)
        {
            // Get two bytes containing data for line.
            uint8_t current_byte = 0;
            uint8_t next_byte = 0;
            if(memory_type == MemoryType::Internal)
            {
                current_byte = emulator.getMemory().read(start_address + i * 16 + j, false);
                next_byte = emulator.getMemory().read(start_address + i * 16 + j + 1, false);
            }
            else if (memory_type == MemoryType::MBC1_ROM)
            {
                current_byte = emulator.getMemory().getCartridge().mbc1.getRom().at(start_address + i * 16 + j);
                next_byte = emulator.getMemory().getCartridge().mbc1.getRom().at(start_address + i * 16 + j + 1);
            }

            // Loop over each bit, extract pixels and write them to image.
            for(int m = 7; m >= 0; m--)
            {
                uint8_t msbit = (next_byte >> m) & 1;
                uint8_t lsbit = (current_byte >> m) & 1;
                uint8_t pixel = 0;

                if(msbit)
                    pixel = pixel | 0b10;
                if(lsbit)
                    pixel = pixel | 0b01;
                
                image.setPixel(x + (7 - m), y + k, (sf::Color)emulator.getPPU().getColorPalette().at(pixel));
            }
            k++;
        }

        x += 8;
    }

    texture.update(image);
    sprite.setTexture(texture);
}

void TileViewer::save2bppTextFile(const sf::String& p_file_path)
{
    std::ofstream file(p_file_path);
    if(file.is_open())
    {
        file << "; Tiles generated from VRAM snapshot from the game " << emulator.getMemory().getCartridge().title << "\n\n";

        int rows = image.getSize().y/8;
        int columns = image.getSize().x/8;
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {
                file << "; Tile_" << i << "_" << j << "\n";

                // Read tile.
                for (int y = 0; y < 8; y++)
                {
                    file << "\tdw `"; 
                    for (int x = 0; x < 8; x++)
                    {
                        sf::Color pixel = image.getPixel(j * 8 + x, i * 8 + y);
                        if(pixel == (sf::Color)emulator.getPPU().getColorPalette()[0])
                        {
                            file << 0;
                        }
                        else if(pixel == (sf::Color)emulator.getPPU().getColorPalette()[1])
                        {
                            file << 1;
                        }
                        else if(pixel == (sf::Color)emulator.getPPU().getColorPalette()[2])
                        {
                            file << 2;
                        }
                        else if(pixel == (sf::Color)emulator.getPPU().getColorPalette()[3])
                        {
                            file << 3;
                        }
                    }
                    file << "\n";
                }                
            }
        }
        
        file << "; Current tile map at 0x9800" << "\n";
        for (int i = 0; i < 32; i++)
        {
            file << "\tdb ";
            for (int j = 0; j < 32; j++)
            {
                file << "$" << ui::toHexString(emulator.getMemory().read(0x9800 + (i * 32) + j, false), false, 2).toAnsiString() << ", ";
            }
            file << "\n";
        }

        file << "; Current tile map at 0x9c00" << "\n";
    }
    file.close();
}

OAMViewer::OAMViewer(Emulator& p_emulator, ui::Canvas& p_canvas, sf::Font& p_font)
    : emulator(p_emulator), Widget::Widget(p_canvas), font(p_font), background(p_canvas)
{
    addChild(&background);

    background.setFillColor(sf::Color::Red);
}

void OAMViewer::setSize(const sf::Vector2f& p_size)
{
    Widget::setSize(p_size);
    background.setSize(p_size);
}

MemoryDebugger::MemoryDebugger(Emulator& p_emulator, ui::Canvas& p_canvas, sf::Font& p_font)
    : Widget::Widget(p_canvas),
    memory_viewer(p_emulator, p_canvas, p_font),
    register_info(p_canvas, p_font),
    background(p_canvas),
    emulator(p_emulator)
{
    addChild(&register_info);
    addChild(&memory_viewer);
    addChild(&background);

    register_info.setTextColor(sf::Color::White);
    register_info.setParagraphSpacing(1.f);

    background.setFillColor(sf::Color(15, 15, 15));
}

void MemoryDebugger::setSize(const sf::Vector2f& p_size)
{
    Widget::setSize(p_size);
    background.setSize(p_size);
    memory_viewer.setSize(sf::Vector2f(p_size.x * 3.f/4.f, p_size.y));
    register_info.setPosition((sf::Vector2f)sf::Vector2i(memory_viewer.getPosition().x + memory_viewer.getSize().x + 5.f, memory_viewer.getPosition().y));
    register_info.setSize(sf::Vector2f(p_size.x * 1.f/4.f - 5.f, p_size.y));
}

void MemoryDebugger::setVisible(bool p_state)
{
    Widget::setVisible(p_state);
    background.setVisible(p_state);
    memory_viewer.setVisible(p_state);
    register_info.setVisible(p_state);
}

void MemoryDebugger::updateLogic()
{
    // Cartridge Information.
    sf::String cartridge_information;
    int cartridge_type_code = emulator.getMemory().getCartridge().type_code;
    if(cartridge_type_code == 0)
    {
        cartridge_information = L"No MBC/ROM only (32KB directly mapped to internal memory 0000-7fff)";
    }
    else if(cartridge_type_code == 1 || cartridge_type_code == 2 || cartridge_type_code == 3)
    {
        cartridge_information = 
        emulator.getMemory().getCartridge().type_string + "\n" +
        "RAM enable: " + (emulator.getMemory().getCartridge().mbc1.getRamEnableRegister() ? "True" : "False") + "\n" +
        "BANK1 register: " + ui::toBinaryString(emulator.getMemory().getCartridge().mbc1.getBank1Register(), 5) + "\n" +
        "BANK2 register: " + ui::toBinaryString(emulator.getMemory().getCartridge().mbc1.getBank2Register(), 2) + "\n" +
        "MODE register: " + ui::toBinaryString(emulator.getMemory().getCartridge().mbc1.getModeRegister(), 1) + "\n";
    }
    else
    {
        cartridge_information = L"Cannot display debug information.";
    }

    // Display.
    register_info.setString(
        L"Registers (+ Interrupts)\nAF: " + ui::toHexString((emulator.getCPU().a << 8) | emulator.getCPU().flags, true, 4) + L"\n" +
        L"BC: " + ui::toHexString((emulator.getCPU().b << 8) | emulator.getCPU().c, true, 4) + L"\n" +
        L"DE: " + ui::toHexString((emulator.getCPU().d << 8) | emulator.getCPU().e, true, 4) + L"\n" +
        L"HL: " + ui::toHexString((emulator.getCPU().h << 8) | emulator.getCPU().l, true, 4) + L"\n" + 
        L"PC: " + ui::toHexString(emulator.getCPU().pc, true, 4) + L"\n" + 
        L"SP: " + ui::toHexString(emulator.getCPU().sp, true, 4) + L"\n" + 
        L"F: " + ui::toBinaryString(emulator.getCPU().flags) + L"\n" + 
        L"   0bZNHC0000\n\n" + 
        L"IF: " + ui::toBinaryString(emulator.getMemory().read(0xff0f)) + L"\n" + 
        L"IE: " + ui::toBinaryString(emulator.getMemory().read(0xffff)) + L"\n" + 
        L"IME: " + ui::toString(emulator.getCPU().interrupt_master_enable) + L"\n\n\n" +
        cartridge_information + "\n\n\n"
    );
}

PPUDebugger::PPUDebugger(Emulator& p_emulator, ui::Canvas& p_canvas, sf::Font& p_font)
    : Widget::Widget(p_canvas),
    background(p_canvas),
    vram_viewer(p_emulator, p_canvas, p_font, sf::Vector2f(128, 192)),
    background_display(p_emulator, p_canvas),
    window_display(p_emulator, p_canvas),
    info_text(p_canvas, p_font),
    emulator(p_emulator),
    oam_viewer(p_emulator, p_canvas, p_font)
{
    addChild(&background);
    addChild(&vram_viewer);
    addChild(&background_display);
    addChild(&window_display);
    addChild(&info_text);
    addChild(&oam_viewer);

    vram_viewer.setMemoryType(TileViewer::Internal);
    vram_viewer.setStartAddress(0x8000);

    background_display.setBuffer(&emulator.getPPU().background_buffer, sf::Vector2f(emulator.getPPU().actual_screen_width, emulator.getPPU().actual_screen_height));
    window_display.setBuffer(&emulator.getPPU().window_buffer, sf::Vector2f(emulator.getPPU().actual_screen_width, emulator.getPPU().actual_screen_height));

    background.setFillColor(sf::Color(255, 255, 255));
    background.setOutlineColor(sf::Color(217, 217, 217));
    background.setOutlineThickness(1.f);

    info_text.setTextColor(sf::Color::Black);
}

void PPUDebugger::setSize(const sf::Vector2f& p_size)
{
    Widget::setSize(p_size);
    background.setSize(p_size);
    vram_viewer.setPosition(410.f, 5.f);
    vram_viewer.setSize(sf::Vector2f(400.f, 675.f));

    background_display.setPosition(5.f, 5.f);
    background_display.setSize(sf::Vector2f(400.f, 400.f));
    window_display.setPosition(5.f, 410.f);
    window_display.setSize(sf::Vector2f(400.f, 400.f));
    
    oam_viewer.setPosition(window_display.getPosition().x, window_display.getPosition().y + window_display.getSize().y + 10.f);
    oam_viewer.setSize(sf::Vector2f(400.f, 100.f));

    info_text.setSize(sf::Vector2f(300.f, p_size.y - 5.f));
    info_text.setPosition(getSize().x - info_text.getSize().x, 5.f);
}

void PPUDebugger::setVisible(bool p_state)
{
    Widget::setVisible(p_state);
    background.setVisible(p_state);
    vram_viewer.setVisible(p_state);

    background_display.setVisible(p_state);
    window_display.setVisible(p_state);

    info_text.setVisible(p_state);
}

void PPUDebugger::updateLogic()
{
    // Mode string.
    sf::String mode_name;
    switch(emulator.getPPU().getLCDMode())
    {
        case 0:
        mode_name = "HORIZONTAL BLANK";
        break;
        case 1:
        mode_name = "VERTICAL BLANK";
        break;
        case 2:
        mode_name = "OAM SCAN";
        break;
        case 3:
        mode_name = "DRAWING PIXELS";
        break;
    }

    info_text.setString(
        "Cycles: " + ui::toString(emulator.getPPU().getCycleCount()) + "\n" +
        "0xFF44 - LY: " + ui::toString((int)emulator.getMemory().read(0xff44)) + "\n" +
        "0xFF45 - LYC: " + ui::toString((int)emulator.getMemory().read(0xff45)) + "\n" +
        "0xFF40 - LCDC: " + "\n" +
        "   " + "LCD_AND_PPU_ENABLE: " + (emulator.getPPU().getLCDCBit(PPU::LCD_AND_PPU_ENABLE) ? "On" : "Off") + "\n" +
        "   " + "WINDOW_TILE_MAP: " + (emulator.getPPU().getLCDCBit(PPU::WINDOW_TILE_MAP) ? "9C00-9FFF" : "9800-9BFF") + "\n" +
        "   " + "WINDOW_ENABLE: " + (emulator.getPPU().getLCDCBit(PPU::WINDOW_ENABLE) ? "On" : "Off") + "\n" +
        "   " + "BG_AND_WIN_TILE_DATA: " + (emulator.getPPU().getLCDCBit(PPU::BG_AND_WIN_TILE_DATA) ? "8000-8FFF" : "8800-97FF") + "\n" +
        "   " + "BG_TILE_MAP: " + (emulator.getPPU().getLCDCBit(PPU::BG_TILE_MAP) ? "9C00-9FFF" : "9BFF-9800") + "\n" +
        "   " + "SPRITE_SIZE: " + (emulator.getPPU().getLCDCBit(PPU::SPRITE_SIZE) ? "8x16" : "8x8") + "\n" +
        "   " + "SPRITE_ENABLE: " + (emulator.getPPU().getLCDCBit(PPU::SPRITE_ENABLE) ? "On" : "Off") + "\n" +
        "   " + "BG_AND_WINDOW_ENABLE: " + (emulator.getPPU().getLCDCBit(PPU::BG_AND_WINDOW_ENABLE) ? "On" : "Off") + "\n" +
        "0xFF41 - STAT: " + "\n" +
        "   " + "LYC_EQUALS_LY_INTERRUPT: " + (emulator.getPPU().getSTATBit(PPU::LYC_EQUALS_LY_INTERRUPT) ? "Enabled" : "Disabled") + "\n" +
        "   " + "OAM_STAT_INTERRUPT: " + (emulator.getPPU().getSTATBit(PPU::OAM_STAT_INTERRUPT) ? "Enabled" : "Disabled") + "\n" +
        "   " + "VBLANK_STAT_INTERRUPT: " + (emulator.getPPU().getSTATBit(PPU::VBLANK_STAT_INTERRUPT) ? "Enabled" : "Disabled") + "\n" +
        "   " + "HBLANK_STAT_INTERRUPT: " + (emulator.getPPU().getSTATBit(PPU::HBLANK_STAT_INTERRUPT) ? "Enabled" : "Disabled") + "\n" +
        "   " + "LY_EQUALS_LYC: " + (emulator.getPPU().getSTATBit(PPU::LY_EQUALS_LYC) ? "Enabled" : "Disabled") + "\n" +
        "   " + "Mode: " + ui::toString((int)emulator.getPPU().getLCDMode()) + " (" + mode_name + ")"
    );
}

Disassembler::Disassembler(Emulator& p_emulator, ui::Canvas& p_canvas, sf::Font& p_font)
    : Widget::Widget(p_canvas), emulator(p_emulator), background(p_canvas), text_field(p_canvas, p_font)
{
    addChild(&background);
    addChild(&text_field);

    background.setFillColor(sf::Color(15, 15, 15));
    text_field.setTextColor(sf::Color::White);
}

void Disassembler::setSize(const sf::Vector2f& p_size)
{
    Widget::setSize(p_size);
    background.setSize(p_size);
    text_field.setSize(p_size);
}

void Disassembler::setVisible(bool p_state)
{
    Widget::setVisible(p_state);
    background.setVisible(p_state);
    text_field.setVisible(p_state);
}

sf::String Disassembler::disassembleAddress(uint16_t p_address)
{
    uint8_t opcode = emulator.getMemory().read(p_address);
    uint8_t operand8 = emulator.getMemory().read(p_address + 1);
    uint16_t operand16 = (emulator.getMemory().read(p_address + 2) << 8) | operand8;

    sf::String disassemble = ui::toHexString(p_address, false, 4) + ": " + 
    emulator.getCPU().instruction_table[opcode].name;

    if(emulator.getCPU().instruction_table[opcode].length == 2)
    {
        disassemble.replace("r8", ui::toHexString(operand8));
        disassemble.replace("a8", ui::toHexString(operand8));
        disassemble.replace("d8", ui::toHexString(operand8));
    }
    if(emulator.getCPU().instruction_table[opcode].length == 3) 
    {
        disassemble.replace("d16", ui::toHexString(operand16));
        disassemble.replace("a16", ui::toHexString(operand16));
    }

    return disassemble;
}

int Disassembler::getInstructionLength(uint16_t p_address)
{
    uint8_t opcode = emulator.getMemory().read(p_address);
    return emulator.getCPU().instruction_table[opcode].length;
}

void Disassembler::updateLogic()
{
    sf::String disassemble;
    uint16_t address = emulator.getCPU().pc;
    for (int i = 0; i < 10; i++)
    {
        disassemble += disassembleAddress(address) + "\n";
        address += getInstructionLength(address);
    }
    
    text_field.setString(disassemble);
}

Debugger::Debugger(Emulator& p_emulator, ui::Canvas& p_canvas, sf::Font& p_font)
    : Widget::Widget(p_canvas),
    tab_menu(p_canvas, p_font),
    enable_button(p_canvas, p_font),
    step_button(p_canvas, p_font),
    open_button(p_canvas, p_font),
    cartridge_info(p_canvas, p_font),
    display(p_emulator, p_canvas),
    memory_debugger(p_emulator, p_canvas, p_font),
    ppu_debugger(p_emulator, p_canvas, p_font),
    disassembler(p_emulator, p_canvas, p_font),
    emulator(p_emulator),
    joypad_info(p_canvas, p_font)
{
    addChild(&tab_menu);
    addChild(&cartridge_info);
    addChild(&joypad_info);
    addChild(&enable_button);
    addChild(&step_button);
    addChild(&open_button);
    addChild(&display);
    addChild(&memory_debugger);
    addChild(&ppu_debugger);
    addChild(&disassembler);

    cartridge_info.setTextColor(sf::Color::Black);
    cartridge_info.setParagraphSpacing(1.f);

    joypad_info.setTextColor(sf::Color::Black);
    joypad_info.setParagraphSpacing(1.f);

    enable_button.text.setHorizontalAlignment(ui::TextField::Middle);
    enable_button.text.setString("Play");
    enable_button.setOnClickEvent([this]()
    {
        if(emulator.isEnabled())
        {
            emulator.setEnabled(false);
            enable_button.text.setString("Play");
        }
        else
        {
            emulator.setEnabled(true);
            enable_button.text.setString("Stop");
        }
    });
    open_button.text.setHorizontalAlignment(ui::TextField::Middle);
    open_button.text.setString("Open .gb");
    open_button.setOnClickEvent([this]()
    {
        emulator.setEnabled(false);
        enable_button.text.setString("Play");

        sf::String file_path;
        createOpenFileDialog(file_path, ".gb Files\0*.gb\0\0");
        emulator.loadCartridge(file_path.toAnsiString());
    });
    step_button.text.setHorizontalAlignment(ui::TextField::Middle);
    step_button.text.setString("Step");
    step_button.setOnClickEvent([this]()
    {
        emulator.setEnabled(true, Emulator::Step);
    });

    tab_menu.addTab(&memory_debugger, "Memory");
    tab_menu.addTab(&ppu_debugger, "PPU");
    tab_menu.addTab(&disassembler, "Disassembler");

    display.setBuffer(&emulator.getPPU().screen_buffer, sf::Vector2f(emulator.getPPU().screen_width, emulator.getPPU().screen_height));

    setSize((sf::Vector2f)window.getSize());
}

void Debugger::setSize(const sf::Vector2f& p_size)
{
    Widget::setSize(p_size);
    display.setSize(sf::Vector2f(emulator.getPPU().screen_width * 2.f, emulator.getPPU().screen_height * 2.f));
    tab_menu.setPosition(display.getPosition().x + display.getSize().x, 0.f);
    tab_menu.setSize(sf::Vector2f(p_size.x - tab_menu.getPosition().x, p_size.y));

    enable_button.setPosition(display.getPosition().x, display.getSize().y);
    enable_button.setSize(sf::Vector2f(display.getSize().x, 20.f));
    open_button.setPosition(display.getPosition().x , display.getSize().y + enable_button.getSize().y - 1.f);
    open_button.setSize(sf::Vector2f(display.getSize().x, 20.f));
    step_button.setPosition(display.getPosition().x, display.getSize().y + enable_button.getSize().y + open_button.getSize().y - 2.f);
    step_button.setSize(sf::Vector2f(display.getSize().x, 20.f));

    memory_debugger.setPosition(tab_menu.getPosition().x, 20.f);
    memory_debugger.setSize(sf::Vector2f(tab_menu.getSize().x, p_size.y - 20.f));

    ppu_debugger.setPosition(tab_menu.getPosition().x, 20.f);
    ppu_debugger.setSize(sf::Vector2f(tab_menu.getSize().x, p_size.y - 20.f));

    cartridge_info.setPosition(5.f, step_button.getPosition().y + step_button.getSize().y + 5.f);
    cartridge_info.setSize(sf::Vector2f(display.getSize().x, 100.f));

    joypad_info.setPosition(5.f, cartridge_info.getPosition().y + cartridge_info.getSize().y + 5.f);
    joypad_info.setSize(sf::Vector2f(display.getSize().x, 100.f));

    disassembler.setPosition(tab_menu.getPosition().x, 20.f);
    disassembler.setSize(sf::Vector2f(tab_menu.getSize().x, p_size.y - 20.f));
}

void Debugger::updateEvents(sf::Event& p_event)
{
    if(p_event.type == sf::Event::Resized)
    {
        setSize((sf::Vector2f)window.getSize());
    }

    // Joypad input.
    if(p_event.type == sf::Event::KeyPressed)
    {
        if(p_event.key.code == sf::Keyboard::Down)
            emulator.markKeyPressed(Input::Keys::Down);
        if(p_event.key.code == sf::Keyboard::Up)
            emulator.markKeyPressed(Input::Keys::Up);
        if(p_event.key.code == sf::Keyboard::Left)
            emulator.markKeyPressed(Input::Keys::Left);
        if(p_event.key.code == sf::Keyboard::Right)
            emulator.markKeyPressed(Input::Keys::Right);
        if(p_event.key.code == sf::Keyboard::Enter)
            emulator.markKeyPressed(Input::Keys::Start);
        if(p_event.key.code == sf::Keyboard::Space)
            emulator.markKeyPressed(Input::Keys::Select);
        if(p_event.key.code == sf::Keyboard::S)
            emulator.markKeyPressed(Input::Keys::B);
        if(p_event.key.code == sf::Keyboard::A)
            emulator.markKeyPressed(Input::Keys::A); 
    }
    if(p_event.type == sf::Event::KeyReleased)
    {
        if(p_event.key.code == sf::Keyboard::Down)
            emulator.markKeyReleased(Input::Keys::Down);
        if(p_event.key.code == sf::Keyboard::Up)
            emulator.markKeyReleased(Input::Keys::Up);
        if(p_event.key.code == sf::Keyboard::Left)
            emulator.markKeyReleased(Input::Keys::Left);
        if(p_event.key.code == sf::Keyboard::Right)
            emulator.markKeyReleased(Input::Keys::Right);
        if(p_event.key.code == sf::Keyboard::Enter)
            emulator.markKeyReleased(Input::Keys::Start);
        if(p_event.key.code == sf::Keyboard::Space)
            emulator.markKeyReleased(Input::Keys::Select);
        if(p_event.key.code == sf::Keyboard::S)
            emulator.markKeyReleased(Input::Keys::B);
        if(p_event.key.code == sf::Keyboard::A)
            emulator.markKeyReleased(Input::Keys::A);
    }
}

void Debugger::updateLogic()
{
    cartridge_info.setString(
        "Game Title: " + emulator.getMemory().getCartridge().title + "\n" +
        "Cartridge Type: " + emulator.getMemory().getCartridge().type_string + " (" + ui::toHexString(emulator.getMemory().getCartridge().type_code) + ")" + "\n" +
        "ROM Size: " + emulator.getMemory().getCartridge().rom_size_string + " (" + ui::toHexString(emulator.getMemory().getCartridge().rom_size_code) + ")" + "\n" +
        "RAM Size: " + emulator.getMemory().getCartridge().ram_size_string + " (" + ui::toHexString(emulator.getMemory().getCartridge().ram_size_code) + ")" + "\n"
    );

    joypad_info.setString(
        "0xFF00 - Joypad (" + ui::toBinaryString(emulator.getMemory().read(0xff00, false)) + ")\n" +
        "Bit 7 - No used\n" + 
        "Bit 6 - No used\n" + 
        "Bit 5 - Action Buttons (" + (emulator.getMemory().read(0xff00, false) & 0b100000 ? "Not selected" : "Selected") + ")\n" +
        "Bit 4 - Dir Buttons (" + (emulator.getMemory().read(0xff00, false) & 0b10000 ? "Not selected" : "Selected") + ")\n" +
        "Bit 3 - Down OR Start (" + (emulator.getMemory().read(0xff00, false) & 0b1000 ? "Not pressed" : "Pressed") + ")\n" +
        "Bit 2 - Up OR Select (" + (emulator.getMemory().read(0xff00, false) & 0b100 ? "Not pressed" : "Pressed") + ")\n" +
        "Bit 1 - Left OR B (" + (emulator.getMemory().read(0xff00, false) & 0b10 ? "Not pressed" : "Pressed") + ")\n" +
        "Bit 0 - Right OR A (" + (emulator.getMemory().read(0xff00, false) & 0b1 ? "Not pressed" : "Pressed") + ")\n" +
        "Controls: A, S, Arrow Keys, Enter, Space"
    );
}
