#include "sfguil.hpp"
#include <vector>
#include <cmath>

namespace ui
{
    Widget::Widget(Canvas& p_canvas)
        : window(p_canvas.window), canvas(p_canvas)
    {
        setPosition(sf::Vector2f(0.f, 0.f));
        setSize(sf::Vector2f(100.f, 100.f));

        p_canvas.add(this);
    }
    
    Widget::~Widget() 
    {
        canvas.remove(this);
    }
    
    void Widget::setSize(const sf::Vector2f& p_size)
    {
        size = p_size;
    }

    const sf::Vector2f& Widget::getSize() const
    {
        return size;
    }

	sf::Transform Widget::getWorldTransform() const
	{
		sf::Transform transform = getTransform();
		const Widget* current_parent = getParent();
		while (current_parent != nullptr)
		{
		 	transform = current_parent->getTransform() * transform;
		    current_parent = current_parent->getParent();
		}
		return transform;
	}

    void Widget::draw(sf::RenderTarget& p_target, sf::RenderStates p_states) const
    {
		
    }
    
    void Widget::updateLogic() 
    {
		
    }
    
    void Widget::updateEvents(sf::Event& p_event) 
    {
		
    }

    void Widget::setVisible(bool p_state)
    {
        is_visible = p_state;
        for (int i = 0; i < getChildCount(); i++)
        {
            getChild(i)->setVisible(p_state);
        }
    }

    bool Widget::isVisible()
    {
        return is_visible;
    }

    void Widget::setUpdated(bool p_state)
    {
        is_updated = p_state;
        for (int i = 0; i < getChildCount(); i++)
        {
            getChild(i)->setUpdated(p_state);
        }
    }

    bool Widget::isUpdated()
    {
        return is_updated;
    }

    void Widget::setSelectable(bool p_state)
    {
        is_selectable = p_state;
        for (int i = 0; i < getChildCount(); i++)
        {
            getChild(i)->setSelectable(p_state);
        }
    }

    bool Widget::isSelectable()
    {
        return is_selectable;
    }

    void Widget::setLayer(int p_layer_index)
    {   
        layer = p_layer_index;
        canvas.moveToLayer(this, layer);

        for (int i = 0; i < getChildCount(); i++)
        {
            getChild(i)->setLayer(p_layer_index);
        }
    }

    int Widget::getLayer()
    {
        return layer;
    }

    void Widget::addChild(Widget* p_widget, int p_index)
    {
        // Check if widget is itself.
        if(p_widget == this)
        {
            std::cout << "Tried setting this widget as its own child." << std::endl;
            return;
        }
        // Check if widget was already added as child.
        for (int i = 0; i < children.size(); i++)
        {
            if(p_widget == children[i])
            {
                return;
            }
        }
        // Add/Insert child.
        if(p_index < 0 || p_index >= children.size())
        {
            children.push_back(p_widget);
        }
        else
        {
            children.insert(children.begin() + p_index, p_widget);
        }
        p_widget->parent = this;
    }

    void Widget::removeChild(Widget* p_widget)
    {
        for (int i = 0; i < children.size(); i++)
        {
            if(p_widget == children[i])
            {
                p_widget->parent = nullptr;
                children.erase(children.begin() + i);
            }
        }
    }

    Widget* Widget::getChild(int p_index) const
    {
        if(children.size() == 0) return nullptr;
        if(p_index < 0) p_index = 0;
        if(p_index >= children.size()) p_index = children.size() - 1;
        return children[p_index];
    }

    Widget* Widget::getParent() const
    {
        return parent;
    }

    int Widget::getChildCount() const
    {
        return children.size();
    }

    sf::String Widget::printSelf(int depth)
    {
        sf::String value;
        for (int i = 0; i < depth; i++)
        {
            value += "-   ";
        }
        sf::String name = "Widget";

        Button* button = dynamic_cast<Button*>(this);
        if(button != nullptr) name = "Button";
        TextField* text = dynamic_cast<TextField*>(this);
        if(text != nullptr) name = "TextField";
        Graphic* graphic = dynamic_cast<Graphic*>(this);
        if(graphic != nullptr) name = "Graphic";
        ScrollBar* scrollbar = dynamic_cast<ScrollBar*>(this);
        if(scrollbar != nullptr) name = "ScrollBar";
        ScrollList* scrolllist = dynamic_cast<ScrollList*>(this);
        if(scrolllist != nullptr) name = "ScrollList";
        DropDownList* dropdownlist = dynamic_cast<DropDownList*>(this);
        if(dropdownlist != nullptr) name = "DropDownList";
        DockNode* docknode = dynamic_cast<DockNode*>(this);
        if(docknode != nullptr) name = "DockNode";
        DockSpace* dockspace = dynamic_cast<DockSpace*>(this);
        if(dockspace != nullptr) name = "DockSpace";
        Panel* panel = dynamic_cast<Panel*>(this);
        if(panel != nullptr) name = "Panel";

        if(isHovered()) name.insert(0, "=>");

        value += name + ": layer=" + toString(layer) + " children=" + toString(children.size()) + "\n";
        for (int i = 0; i < children.size(); i++)
        {
            value += children[i]->printSelf(depth + 1);
        }
        return value;
    }

    bool Widget::isMouseOver()
    {
        sf::Vector2f mouse_position = getWorldTransform().getInverse().transformPoint(getMousePosition(window, &canvas.getView()));
        bool mouse_over = sf::FloatRect(sf::Vector2f(), getSize()).contains(mouse_position); 
        if(getMask() != nullptr) mouse_over = mouse_over && getMask()->isMouseOver();

        return mouse_over;
    }

    bool Widget::isMouseOverChildren()
    {
        for (int i = 0; i < getChildCount(); i++)
        {
            if (children[i]->isMouseOver() || children[i]->isMouseOverChildren())
            {
                return true;
            }
        }
        return false;
    }

    bool Widget::isHovered()
    {
        return canvas.getHovered() == this;
    }

    bool Widget::areChildrenHovered()
    {
        for (int i = 0; i < getChildCount(); i++)
        {
            if(children[i]->isHovered() || children[i]->areChildrenHovered())
            {
                return true;
            }
        }
        return false;
    }

    bool Widget::isSelected()
    {
        return canvas.getSelected() == this;
    }

    bool Widget::areChildrenSelected()
    {
        for (int i = 0; i < getChildCount(); i++)
        {
            if(children[i]->isSelected() || children[i]->areChildrenSelected())
            {
                return true;
            }
        }
        return false;
    }

    void Widget::setMask(Widget* p_mask) 
    {
        if(p_mask == this) 
        {
            std::cout << "Tried setting mask to itself." << std::endl;
            return;
        }
        mask = p_mask;

        for (int i = 0; i < getChildCount(); i++)
        {
            getChild(i)->setMask(p_mask);
        }
    }

    Widget* Widget::getMask() const
    {
        return mask;
    }

    Graphic::Graphic(Canvas& p_canvas) : Widget(p_canvas), preserve_aspect(true)
    {
    }
    
    void Graphic::setOutlineColor(const sf::Color& p_color) 
    {
        background.setOutlineColor(p_color);
    }
    
    void Graphic::setOutlineThickness(float p_thickness)
    {
        background.setOutlineThickness(p_thickness);
    }

    void Graphic::setFillColor(const sf::Color& p_color) 
    {
        background.setFillColor(p_color);
    }

    void Graphic::setTexture(const sf::Texture* p_texture, const sf::IntRect& p_texture_rect)
    {
        background.setTextureRect(p_texture_rect);
        background.setTexture(p_texture);
    }

    void Graphic::setTexture(const sf::Texture* p_texture)
    {
        background.setTexture(p_texture);
    }

    const sf::Texture* Graphic::getTexture()
    {
        return background.getTexture();
    }

    const sf::Vector2f& Graphic::getImagePosition()
    {
        preserveAspect();
        return background.getPosition();
    }

    const sf::Vector2f& Graphic::getImageSize()
    {
        preserveAspect();
        return background.getSize();
    }

    void Graphic::setPreserveAspect(bool p_state)
    {
        preserve_aspect = p_state;
    }

    bool Graphic::isPreservingAspect()
    {
        return preserve_aspect;
    }

    const sf::Color& Graphic::getOutlineColor()
    {
        return background.getOutlineColor();
    }

    float Graphic::getOutlineThickness()
    {
        return background.getOutlineThickness();
    }
    
    const sf::Color& Graphic::getFillColor()
    {
        return background.getFillColor();
    }

    void Graphic::draw(sf::RenderTarget& p_target, sf::RenderStates p_states) const
    {
        // Resizing.
        preserveAspect();

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
        canvas.getClippingShader().setUniform("use_texture", background.getTexture() != nullptr);

        //std::cout << "SHADER INFO" << std::endl;
        //std::cout << "array_size: " << (int)sizes.size() << std::endl;
        //std::cout << "sizes: " << (int)sizes.size() << std::endl;
        //std::cout << "inverse_transforms: " << (int)sizes.size() << std::endl;
        //std::cout << "window_size: " << window.getSize().x << ", " << window.getSize().y << std::endl;
        //std::cout << "texture: " << (int)sizes.size() << std::endl;
        //std::cout << "use_texture: " << (background.getTexture() != nullptr) << std::endl;
        
        // Drawing.
        window.draw(background, p_states);
    }

    void Graphic::preserveAspect() const
    {
        if(preserve_aspect && background.getTexture() != nullptr)
        {
            sf::Vector2f base_size = (sf::Vector2f)background.getTexture()->getSize();
            float aspect_ratio = base_size.x / base_size.y;
            if(getSize().y * aspect_ratio < getSize().x)
            {
                background.setSize(sf::Vector2f(getSize().y * aspect_ratio, getSize().y));
            }
            else
            {
                aspect_ratio = base_size.y / base_size.x;
                background.setSize(sf::Vector2f(getSize().x, getSize().x * aspect_ratio));
            }
            background.setPosition(sf::Vector2f(getSize().x/2.f - background.getSize().x/2.f, getSize().y/2.f - background.getSize().y/2.f));
        }
        else
        {
            background.setPosition(background.getOutlineThickness(), background.getOutlineThickness());
            background.setSize(sf::Vector2f(getSize().x - background.getOutlineThickness() * 2.f, getSize().y - background.getOutlineThickness() * 2.f));
        }
    }

    TextField::TextField(Canvas& p_canvas) 
        : Widget(p_canvas), 
        string(L"New Text"), 
        font(p_canvas.font),
        character_size(12),
        line_spacing_factor(1.f),
        paragraph_spacing_factor(1.25f),
        text_color(sf::Color::Black),
        horizontal_alignment(Left), 
        vertical_alignment(Left), 
        overflow_type(Overflow),
        wrapping(true),
        vertices(sf::Triangles),
        geometry_need_update(true),
        selection_color(sf::Color(0, 0, 255, 100)),
        selection_vertices(sf::Triangles),
        debug_vertices(sf::Lines)
    {

    }

    void TextField::setString(const sf::String& p_string)
    {
        if(string != p_string)
        {
            string = p_string;
            geometry_need_update = true;
        }
    }

    void TextField::setFont(const sf::Font& p_font)
    {
        if(&font != &p_font)
        {
            font = p_font;
            geometry_need_update = true;
        }
    }

    void TextField::setCharacterSize(unsigned int p_size)
    {
        if(character_size != p_size)
        {
            character_size = p_size;
            geometry_need_update = true;
        }
    }

    void TextField::setLineSpacing(float p_spacing_factor)
    {
        if(line_spacing_factor != p_spacing_factor)
        {
            line_spacing_factor = p_spacing_factor;
            geometry_need_update = true;
        }   
    }

    void TextField::setParagraphSpacing(float p_spacing_factor)
    {
        if(paragraph_spacing_factor != p_spacing_factor)
        {
            paragraph_spacing_factor = p_spacing_factor;
            geometry_need_update = true;
        }   
    }

    void TextField::setTextColor(const sf::Color& p_color)
    {
        if(text_color != p_color)
        {
            text_color = p_color;

            // Change vertex colors directly, no need to update whole geometry
            // (if geometry is updated anyway, we can skip this step)
            if (!geometry_need_update)
            {
                for (std::size_t i = 0; i < vertices.getVertexCount(); ++i)
                    vertices[i].color = text_color;
            }
        }
    }

    void TextField::setOverflowType(OverflowType p_type)
    {
        if(overflow_type != p_type)
        {
            overflow_type = p_type;
            geometry_need_update = true;
        }
    }

    void TextField::enableWrapping(bool p_state)
    {
        if(wrapping != p_state)
        {
            wrapping = p_state;
            geometry_need_update = true;
        }
    }

    void TextField::setHorizontalAlignment(Alignment p_type)
    {
        if(horizontal_alignment != p_type)
        {
            horizontal_alignment = p_type;
            geometry_need_update = true;
        }
    }

    void TextField::setVerticalAlignment(Alignment p_type)
    {
        if(vertical_alignment != p_type)
        {
            vertical_alignment = p_type;
            geometry_need_update = true;
        }
    }

    void TextField::setSize(const sf::Vector2f& p_size)
    {
        if(getSize() != p_size)
        {
            Widget::setSize(p_size);
            geometry_need_update = true;
        }
    }

    void TextField::setSelection(int p_start, int p_end)
    {
        if(start_selection != p_start || end_selection != p_end)
        {
            start_selection = p_start;
            end_selection = p_end;
            geometry_need_update = true;
        }
    }

    void TextField::setSelectionColor(const sf::Color& p_color)
    {
        if(selection_color != p_color)
        {
            selection_color = p_color;

            if (!geometry_need_update)
            {
                for (int i = 0; i < selection_vertices.getVertexCount(); i++)
                    selection_vertices[i].color = selection_color;
            }
        }
    }

    const sf::String& TextField::getString() const
    {
        return string;
    }

    const sf::Font& TextField::getFont() const
    {
        return font;
    }

    unsigned int TextField::getCharacterSize() const
    {
        return character_size;
    }

    float TextField::getLineSpacing() const
    {
        return line_spacing_factor;
    }

    float TextField::getParagraphSpacing() const
    {
        return paragraph_spacing_factor;
    }

    const sf::Color& TextField::getTextColor() const
    {
        return text_color;
    }

    TextField::OverflowType TextField::getOverflowType() const
    {
        return overflow_type;
    }

    bool TextField::isWrapping() const
    {
        return wrapping;
    }

    TextField::Alignment TextField::getHorizontalAlignment() const
    {
        return horizontal_alignment;
    }
    TextField::Alignment TextField::getVerticalAlignment() const
    {
        return vertical_alignment;
    }

    const sf::Color& TextField::getSelectionColor() const
    {
        return selection_color;
    }

    void TextField::matchSizeToText()
    {
        setSize(sf::Vector2f(1000.f, 1000.f)); // TODO: Update text bounds checking and tracking.
        ensureGeometryUpdate();
        setSize(sf::Vector2f(text_bounds.width + 5.f, text_bounds.height + 5.f)); // Add some offset to the width so that the text doesn't break into a new line.
    }

    sf::Vector2f TextField::findCharacterPos(int p_index) const
    {
        ensureGeometryUpdate();

        int current_index = 0;

        for (int i = 0; i < text_info.size(); i++)
        {
            for (int j = 0; j < text_info[i].size(); j++)
            {
                if(current_index == p_index) 
                {
                    return getTransform().transformPoint(vertices[text_info[i][j].start_vertex].position.x, text_info[i][j].baseline - character_size);
                }
                current_index++;
            }
        }

        // The text is empty or index is out of bounds, just return the position of the first/last character.
        if(current_index == 0 || p_index < 0) return getTransform().transformPoint(start_of_string.x, start_of_string.y - character_size);
        return getTransform().transformPoint(end_of_string.x, end_of_string.y - character_size);
    }

    int TextField::findIndex(const sf::Vector2f& p_position) const
    {
        ensureGeometryUpdate();

        float smallest_distance = 0.f;
        int smallest_distance_index = 0; 
        int current_index = 0; 
        bool first = true;

        for (int i = 0; i < text_info.size(); i++)
        {
            for (int j = 0; j < text_info[i].size(); j++)
            {
                sf::Vector2f distance_vector = getTransform().transformPoint(vertices[text_info[i][j].start_vertex].position.x, text_info[i][j].baseline) - p_position;
                float square_distance = distance_vector.x * distance_vector.x + distance_vector.y * distance_vector.y; // No need to pull the square root. 

                if(first)
                {
                    smallest_distance = square_distance;
                    smallest_distance_index = current_index;
                    first = false;
                }

                if(square_distance < smallest_distance)
                {
                    smallest_distance = square_distance;
                    smallest_distance_index = current_index;
                }
                current_index++;
            }
        }
        // The text is empty.
        if(current_index == 0) return 0;

        // Since the cursor should appear behind the last character and not in front, we need to also check that position.
        sf::Vector2f distance_vector = getTransform().transformPoint(end_of_string) - p_position;
        float square_distance = distance_vector.x * distance_vector.x + distance_vector.y * distance_vector.y;
        if(square_distance < smallest_distance)
        {
            smallest_distance = square_distance;
            smallest_distance_index = current_index;
        }

        return smallest_distance_index;
    }

    void TextField::ensureGeometryUpdate() const
    {
        // Ensure an update is needed due to a change of the text.
        if(!geometry_need_update) return;
        // Mark as updated.
        geometry_need_update = false;

        // Update geometry.
        applyStringGeometry(string);

        // Update overflow.
        if(overflow_type != Overflow)
        {
            applyStringGeometry(applyOverflowType());
        }

        // Change alignment.
        applyHorizontalAlignment();
        applyVerticalAlignment();

        // Update selection.
        applySelection();

        // Debug geometry.
        //applyDebugGeometry();
    }

    void TextField::draw(sf::RenderTarget& p_target, sf::RenderStates p_states) const 
    {
        ensureGeometryUpdate();

        p_states.texture = &font.getTexture(character_size);

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
        canvas.getClippingShader().setUniform("use_texture", true);
        window.draw(vertices, p_states);
        canvas.getClippingShader().setUniform("use_texture", false);
        window.draw(selection_vertices, p_states);
        window.draw(debug_vertices, p_states);
    }

    void TextField::addGlyphQuad(sf::VertexArray& p_vertices, const sf::Vector2f& p_position, const sf::Color& p_color, const sf::Glyph& p_glyph) const
    {
        // Bounds of each quad.
        float left = p_glyph.bounds.left;
        float top = p_glyph.bounds.top;
        float right = p_glyph.bounds.left + p_glyph.bounds.width;
        float bottom = p_glyph.bounds.top  + p_glyph.bounds.height;

        // UV texture coordinates that match up the texture of the font with the individual quads.
        float u1 = p_glyph.textureRect.left;
        float v1 = p_glyph.textureRect.top;
        float u2 = p_glyph.textureRect.left + p_glyph.textureRect.width;
        float v2 = p_glyph.textureRect.top  + p_glyph.textureRect.height;

        // Every quad is made up from two triangles, therefore 6 vertices are added to the passed vertex array.
        p_vertices.append(sf::Vertex(sf::Vector2f(p_position.x + left, p_position.y + bottom), p_color ,sf::Vector2f(u1, v2)));
        p_vertices.append(sf::Vertex(sf::Vector2f(p_position.x + left, p_position.y + top), p_color    ,sf::Vector2f(u1, v1)));
        p_vertices.append(sf::Vertex(sf::Vector2f(p_position.x + right, p_position.y + top), p_color   ,sf::Vector2f(u2, v1)));
        p_vertices.append(sf::Vertex(sf::Vector2f(p_position.x + left, p_position.y + bottom), p_color ,sf::Vector2f(u1, v2)));
        p_vertices.append(sf::Vertex(sf::Vector2f(p_position.x + right, p_position.y + top), p_color   ,sf::Vector2f(u2, v1)));
        p_vertices.append(sf::Vertex(sf::Vector2f(p_position.x + right, p_position.y + bottom), p_color,sf::Vector2f(u2, v2)));
    }

    void TextField::applyHorizontalAlignment() const
    {
        if(horizontal_alignment == Left) return;

        for (int i = 0; i < text_info.size(); i++)
        {
            // Skip empty lines.
            if(text_info[i].size() == 0) continue;

            sf::Vector2f start_position = vertices[text_info[i][0].start_vertex].position;
            sf::Vector2f end_position = vertices[text_info[i][text_info[i].size() - 1].end_vertex].position;
            float distanceToMove = 0.f;
            if(horizontal_alignment == Right)
            {
                distanceToMove = getSize().x - end_position.x;
            }
            else if(horizontal_alignment == Middle)
            {
                distanceToMove = (getSize().x/2.f) - end_position.x/2.f;
            }

            // Update actual vertices.
            for (int j = text_info[i][0].start_vertex; j < text_info[i][text_info[i].size() - 1].end_vertex + 1; j++)
            {
                vertices[j].position.x += (int)distanceToMove;
            }
            // Update end and start of string.
            if(i == 0) start_of_string += sf::Vector2f(distanceToMove, 0.f);
            if(i == text_info.size() - 1) end_of_string += sf::Vector2f(distanceToMove, 0.f);
        }
    }

    void TextField::applyVerticalAlignment() const
    {
        if(vertical_alignment == Top) return;

        float distanceToMove = 0.f;
        if (vertical_alignment == Middle)
            distanceToMove = (getSize().y/2.f) - (text_bounds.height/2.f);
        else if (vertical_alignment == Bottom)
            distanceToMove = getSize().y - text_bounds.height;
        
        // Update actual vertices.
        for (int i = 0; i < vertices.getVertexCount(); i++)
        {
            vertices[i].position.y += (int)distanceToMove;
        }
        // Update text_info array.
        for (int i = 0; i < text_info.size(); i++)
        {
            for (int j = 0; j < text_info[i].size(); j++)
            {
                text_info[i][j].baseline += (int)distanceToMove;
            }
        }

        // Update end and start of string.
        start_of_string += sf::Vector2f(0.f, distanceToMove);
        end_of_string += sf::Vector2f(0.f, distanceToMove);
    }

    sf::String TextField::applyOverflowType() const
    {
        sf::String modified_string = string;
        int current_index = 0;
        for (int i = 0; i < text_info.size(); i++)
        {
            for (int j = 0; j < text_info[i].size(); j++)
            {
                if(vertices[text_info[i][j].end_vertex].position.x > getSize().x || vertices[text_info[i][j].start_vertex].position.y >= getSize().y)
                {
                    if(overflow_type == Truncate)
                    {
                        return modified_string.substring(0, current_index);
                    }
                    else if (overflow_type == Dotted)
                    {
                        return modified_string.substring(0, current_index - 4 >= 0 ? current_index - 4 : 0) + "...";
                    }
                }
                current_index++;
            }
        }
        return modified_string;
    }

    void TextField::applySelection() const
    {
        selection_vertices.clear();
        int current_index = 0;

        // Find highest distance to the baseline.
        float highest_distance_to_baseline = 0.f;
        for (int i = 0; i < text_info.size(); i++)
        {
            for (int j = 0; j < text_info[i].size(); j++)
            {
                highest_distance_to_baseline = std::max(highest_distance_to_baseline, vertices[text_info[i][j].start_vertex].position.y - text_info[i][j].baseline);
            }
        }

        // Place vertices for selection.
        for (int i = 0; i < text_info.size(); i++)
        {
            for (int j = 0; j < text_info[i].size(); j++)
            {
                if(current_index >= start_selection && current_index < end_selection)
                {
                    sf::Vector2f position = sf::Vector2f(vertices[text_info[i][j].start_vertex].position.x, text_info[i][j].baseline - character_size);
                    sf::Vector2f size = sf::Vector2f(0.f, character_size + highest_distance_to_baseline);
                    
                    if(j + 1 < text_info[i].size()) // Not reached last character in line.
                    {
                        // Take distance between start_vertex of current character to start_vertex of next character as width.
                        size.x = (vertices[text_info[i][j + 1].start_vertex].position.x - vertices[text_info[i][j].start_vertex].position.x);
                    }
                    else // Reached last character in line.
                    {
                        // Since the last character in line has no next character, we just take the end_vertex of the last character as width.
                        size.x = vertices[text_info[i][j].end_vertex].position.x - vertices[text_info[i][j].start_vertex].position.x;
                    }

                    selection_vertices.append(sf::Vertex(sf::Vector2f(position.x, position.y), selection_color));
                    selection_vertices.append(sf::Vertex(sf::Vector2f(position.x + size.x, position.y), selection_color));
                    selection_vertices.append(sf::Vertex(sf::Vector2f(position.x + size.x, position.y + size.y), selection_color));
                    selection_vertices.append(sf::Vertex(sf::Vector2f(position.x, position.y), selection_color));
                    selection_vertices.append(sf::Vertex(sf::Vector2f(position.x + size.x, position.y + size.y), selection_color));
                    selection_vertices.append(sf::Vertex(sf::Vector2f(position.x, position.y + size.y), selection_color));
                }

                current_index++;
            }
        }
    }

    void TextField::applyStringGeometry(const sf::String& p_string) const
    {
        // Reset and compute important values.
        text_bounds = sf::FloatRect();
        vertices.clear();
        text_info.clear();
        std::vector<CharacterInfo> current_line;

        int line_spacing = font.getLineSpacing(character_size) * line_spacing_factor;
        int paragraph_spacing = font.getLineSpacing(character_size) * paragraph_spacing_factor;
        float x = 0.f;
        float y = character_size;
        start_of_string = sf::Vector2f(x, y);

        // Loop through each character in the string and place its vertices.
        for (int i = 0; i < p_string.getSize(); i++)
        {
            // Store character data.
            sf::Uint32 current_char = p_string[i];
            const sf::Glyph& current_glyph = font.getGlyph(current_char, character_size, false);

            // Place vertices.
            if(current_char == '\n')
            {
                current_line.push_back( { y,  (int)vertices.getVertexCount(), (int)vertices.getVertexCount() + 5 });
                addGlyphQuad(vertices, sf::Vector2f(x, y), text_color, font.getGlyph(' ', character_size, false));

                x = 0.f;
                y += paragraph_spacing;
                text_info.push_back(current_line);
                current_line.clear();
            }
            else if(x + current_glyph.advance > getSize().x && wrapping) // Sometimes there will be no space on a line that is too long, so just break the line if it gets too long.
            {
                x = 0.f;
                y += line_spacing;

                text_info.push_back(current_line);
                current_line.clear();

                current_line.push_back( { y, (int)vertices.getVertexCount(), (int)vertices.getVertexCount() + 5 });
                addGlyphQuad(vertices, sf::Vector2f(x, y), text_color, current_glyph);
                x += current_glyph.advance;
            }
            else if(current_char == ' ' && wrapping) // If there is a space search the next space coming up in the string, and calculate if the distance between both string positions still fits on the current line.
            {
                int result_index = i;
                bool searching = true;
                while(searching)
                {
                    result_index++;
                    
                    if(result_index >= p_string.getSize())
                    {
                        searching = false;
                    }
                    if(p_string[result_index] == ' ')
                    {
                        searching = false;
                    }
                }
                float distanceToBounds = getSize().x - (x + current_glyph.advance);
                float distanceOfChars = 0.f;
                for (int j = i; j < result_index; j++)
                {
                    distanceOfChars += font.getGlyph(p_string[j], character_size, false).advance;
                }

                current_line.push_back( { y, (int)vertices.getVertexCount(), (int)vertices.getVertexCount() + 5 });
                addGlyphQuad(vertices, sf::Vector2f(x, y), text_color, current_glyph);
                x += current_glyph.advance;

                if(distanceOfChars > distanceToBounds) // We need to break the line.
                {
                    x = 0.f;
                    y += line_spacing;
                    text_info.push_back(current_line);
                    current_line.clear();
                }
            }
            else
            {
                current_line.push_back( { y, (int)vertices.getVertexCount(), (int)vertices.getVertexCount() + 5 });
                addGlyphQuad(vertices, sf::Vector2f(x, y), text_color, current_glyph);
                x += current_glyph.advance;
            }

            // Update text_bounds.
            text_bounds.width = std::max(text_bounds.width, x);
            text_bounds.height = std::max(text_bounds.height, y + current_glyph.bounds.top + current_glyph.bounds.height);
        }

        // Make sure there is always at least one character for alignment and getting a correct cursor position.
        if(p_string.getSize() == 0)
        {
            const sf::Glyph& current_glyph = font.getGlyph(' ', character_size, false);
            current_line.push_back( { y, (int)vertices.getVertexCount(), (int)vertices.getVertexCount() + 5 });
            addGlyphQuad(vertices, sf::Vector2f(x, y), text_color, current_glyph);

            // Update text_bounds.
            text_bounds.width = std::max(text_bounds.width, x);
            text_bounds.height = std::max(text_bounds.height, y + current_glyph.bounds.top + current_glyph.bounds.height);
        }
        text_info.push_back(current_line);
        
        end_of_string = sf::Vector2f(x, y);
    }

    void TextField::applyDebugGeometry() const
    {
        debug_vertices.clear();

        // Draw text bounds.
        debug_vertices.append(sf::Vertex(sf::Vector2f(text_bounds.left, text_bounds.top), sf::Color::Blue));
        debug_vertices.append(sf::Vertex(sf::Vector2f(text_bounds.left + text_bounds.width, text_bounds.top), sf::Color::Blue));
        debug_vertices.append(sf::Vertex(sf::Vector2f(text_bounds.left + text_bounds.width, text_bounds.top), sf::Color::Blue));
        debug_vertices.append(sf::Vertex(sf::Vector2f(text_bounds.left + text_bounds.width, text_bounds.top + text_bounds.height), sf::Color::Blue));
        debug_vertices.append(sf::Vertex(sf::Vector2f(text_bounds.left + text_bounds.width, text_bounds.top + text_bounds.height), sf::Color::Blue));
        debug_vertices.append(sf::Vertex(sf::Vector2f(text_bounds.left, text_bounds.top + text_bounds.height), sf::Color::Blue));
        debug_vertices.append(sf::Vertex(sf::Vector2f(text_bounds.left, text_bounds.top + text_bounds.height), sf::Color::Blue));
        debug_vertices.append(sf::Vertex(sf::Vector2f(text_bounds.left, text_bounds.top), sf::Color::Blue));

        // Draw base line.
        // for (int i = 0; i < text_info.size(); i++)
        // {
        //     if(text_info[i].size() == 0) continue;

        //     debug_vertices.append(sf::Vertex(sf::Vector2f(
        //         vertices[text_info[i][0].start_vertex].position.x, 
        //         text_info[i][0].baseline), sf::Color::Red));

        //     debug_vertices.append(sf::Vertex(sf::Vector2f(
        //         vertices[text_info[i][text_info[i].size() - 1].start_vertex].position.x, 
        //         text_info[i][text_info[i].size() - 1].baseline), sf::Color::Red));
        // }

    }

    Button::Button(Canvas& p_canvas)
        : Widget::Widget(p_canvas), 
        background(p_canvas), 
        text(p_canvas), 
        hovered_color(sf::Color(229, 241, 251)), 
        pressed_color(sf::Color(175, 232, 255)),
        standard_color(sf::Color(225, 225, 225)),
        margin_left(0.f), 
        margin_right(0.f), 
        margin_bottom(0.f), 
        margin_top(0.f)
    {
        setSize(sf::Vector2f(150.f, 20.f));
        text.setString("New Button...");
        text.setCharacterSize(12);
        text.setTextColor(sf::Color::Black);
        text.setHorizontalAlignment(ui::TextField::Middle);
        text.setVerticalAlignment(ui::TextField::Middle);
        text.setMask(this);

        background.setMask(this);
        background.setOutlineColor(sf::Color(173, 173, 173));
        background.setOutlineThickness(1.f);

        addChild(&background);
        addChild(&text);

        f_event = [](){};
    }

    void Button::setSize(const sf::Vector2f& p_size)
    {
        Widget::setSize(p_size);
        text.setPosition(margin_left, margin_top);
        text.setSize(sf::Vector2f(p_size.x - margin_left - margin_right, p_size.y - margin_top - margin_bottom));
        background.setSize(p_size);
    }

    void Button::updateLogic()
    {
        if(isHovered() || areChildrenHovered())
        {
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                background.setFillColor(pressed_color);
            }
            else
            {
                background.setFillColor(hovered_color);
            }
        }
        else
        {
            background.setFillColor(standard_color);
        }
    }

    void Button::updateEvents(sf::Event& p_event)
    {
        if(isSelected() || areChildrenSelected())
        {
            if(p_event.type == sf::Event::MouseButtonReleased)
            {
                if(p_event.mouseButton.button == sf::Mouse::Left)
                {
                    f_event();
                    canvas.select(nullptr);
                }
            }
        }
    }

    void Button::setOnClickEvent(const std::function<void()>& p_event)
    {
        f_event = p_event;
    }

    void Button::setHoveredColor(const sf::Color& p_color)
    {
        hovered_color = p_color;
    }

    void Button::setPressedColor(const sf::Color& p_color)
    {
        pressed_color = p_color;
    }

    void Button::setStandardColor(const sf::Color& p_color)
    {
        standard_color = p_color;
    }

    void Button::setMask(Widget* p_widget)
    {
        Widget::setMask(p_widget);
        background.setMask(this);
        text.setMask(this);
    }

    void Button::setTextMargin(float p_margin_left, float p_margin_right, float p_margin_top, float p_margin_bottom)
    {
        margin_left = p_margin_left;
        margin_right = p_margin_right;
        margin_top = p_margin_top;
        margin_bottom = p_margin_bottom;

        setSize(getSize());
    }

    ScrollBar::ScrollBar(Canvas& p_canvas) 
    	: Widget::Widget(p_canvas), 
	    background(p_canvas),
	    move_up_button(p_canvas), 
	    move_down_button(p_canvas), 
	    scroll_handle(p_canvas),
        handle_margin(0.f),
        handle_color(sf::Color(205, 205, 205)),
        hovered_handle_color(sf::Color(150, 150, 150))
    {
	    addChild(&background);
	    addChild(&move_up_button);
	    addChild(&move_down_button);
	    addChild(&scroll_handle);

	    background.setFillColor(sf::Color(240, 240, 240));
	    background.setMask(this);
	    scroll_handle.setFillColor(handle_color);
        scroll_handle.setMask(this);

        move_up_button.text.setString("");
	    move_up_button.setStandardColor(sf::Color(240, 240, 240));
	    move_up_button.setHoveredColor(sf::Color(200, 200, 200));
	    move_up_button.setMask(this);
	    move_up_button.text.setHorizontalAlignment(ui::TextField::Middle);
	    move_up_button.text.setVerticalAlignment(ui::TextField::Middle);
        move_up_button.background.setOutlineThickness(0.f);

        move_down_button.text.setString("");
	    move_down_button.setStandardColor(sf::Color(240, 240, 240));
	    move_down_button.setHoveredColor(sf::Color(200, 200, 200));
	    move_down_button.setMask(this);
	    move_down_button.text.setHorizontalAlignment(ui::TextField::Middle);
	    move_down_button.text.setVerticalAlignment(ui::TextField::Middle);
        move_down_button.background.setOutlineThickness(0.f);

        move_up_button.background.setTexture(canvas.scroll_bar_up_texture);
        move_down_button.background.setTexture(canvas.scroll_bar_down_texture);

	    move_up_button.setOnClickEvent([&]()
	    {
	    	setScrollState(state.scroll_handle_position - 10.f, state.scroll_handle_size, state.scroll_list_length);
	    });
	    move_down_button.setOnClickEvent([&]()
	    {
	    	setScrollState(state.scroll_handle_position + 10.f, state.scroll_handle_size, state.scroll_list_length);
	    });

	    setSize(sf::Vector2f(15.f, 200.f));
        setScrollState(0.f, 100.f, 100.f);
    }

    void ScrollBar::setSize(const sf::Vector2f& p_size)
    {
	    Widget::setSize(p_size);
	    background.setSize(p_size);

	    // Refresh size.
    	move_up_button.setSize(sf::Vector2f(getSize().x, getSize().x)); 
        move_down_button.setSize(sf::Vector2f(getSize().x, getSize().x));
    	move_down_button.setPosition(sf::Vector2f(0.f, getSize().y - getSize().x));
    	setScrollState(state.scroll_handle_position, state.scroll_handle_size, state.scroll_list_length);
    }
    
    void ScrollBar::setScrollState(float p_position, float p_size, float p_list_length)
    {
    	state.scroll_handle_size = p_size > p_list_length ? p_list_length : p_size;
    	if(p_position < 0.f)
    	{
    		state.scroll_handle_position = 0.f;
    	}
    	else if(p_position + p_size > p_list_length)
    	{
    		state.scroll_handle_position = p_list_length - p_size;
    	}
    	else 
    	{
    		state.scroll_handle_position = p_position;
    	}
    	state.scroll_list_length = p_list_length;

        float size_ratio = state.scroll_list_length <= state.scroll_handle_size ? 1 : (state.scroll_handle_size / state.scroll_list_length);
        float position_ratio = state.scroll_list_length <= state.scroll_handle_size ? 0 : (state.scroll_handle_position / state.scroll_list_length);
    	float scroll_bar_size = (getSize().y - move_down_button.getSize().y * 2);
    	scroll_handle.setSize(sf::Vector2f(getSize().x - handle_margin * 2.f, size_ratio * scroll_bar_size));
    	scroll_handle.setPosition(sf::Vector2f(handle_margin, move_down_button.getSize().y + position_ratio * scroll_bar_size));
    	f_onScroll();
    }

    const ScrollBar::ScrollState& ScrollBar::getScrollState()
    {
    	return state;
    }

    void ScrollBar::setOnScrollListener(const std::function<void()>& p_event)
    {
    	f_onScroll = p_event;
    }

    void ScrollBar::updateEvents(sf::Event& p_event) 
    {
    	// Deselect the handle when the left mouse is released.
    	if(p_event.type == sf::Event::MouseButtonPressed)
    	{
    		if(p_event.mouseButton.button == sf::Mouse::Left && scroll_handle.isHovered())
    		{
    			handle_selected = true;
    		}
    	}
    	if(p_event.type == sf::Event::MouseButtonReleased)
    	{
    		if(p_event.mouseButton.button == sf::Mouse::Left)
    		{
    			handle_selected = false;
    		}
    	}
    }

    void ScrollBar::updateLogic()
    {
    	sf::Vector2f mouse_position = getWorldTransform().getInverse().transformPoint(getMousePosition(window, &canvas.getView()));
    	if(scroll_handle.isSelected() && handle_selected)
    	{
    		// Move the handle by calculating the difference between the current and last mouse position.
   			sf::Vector2f difference = mouse_position - last_mouse_position;
   			setScrollState(state.scroll_handle_position + (difference.y / getSize().y) * state.scroll_list_length, state.scroll_handle_size, state.scroll_list_length);
    	}
		last_mouse_position = mouse_position;
		
    	// Highlight handle when hovering over it.
    	if(scroll_handle.isHovered())
    	{
            int change = std::ceil((scroll_handle.getFillColor().r - hovered_handle_color.r) * 4.f * canvas.getDeltaTime());
    		scroll_handle.setFillColor(scroll_handle.getFillColor() - sf::Color(change, change, change, 0));
    	}
    	else
    	{
            int change = std::ceil((handle_color.r - scroll_handle.getFillColor().r) * 4.f * canvas.getDeltaTime());
    		scroll_handle.setFillColor(scroll_handle.getFillColor() + sf::Color(change, change, change, 0));
    	}
    }

    void ScrollBar::setMask(Widget* p_widget)
    {
        Widget::setMask(p_widget);
        background.setMask(this);
	    move_up_button.setMask(this);
	    move_down_button.setMask(this);
	    scroll_handle.setMask(this);
    }

    DockNode::DockNode(Canvas& p_canvas)
        : Widget::Widget(p_canvas)
    {
        setSize(sf::Vector2f(300.f, 300.f));
        setSelectable(false);
    }

    DockNode::~DockNode()
    {
        deleteResizeIndicators();
        deleteTitleButtons();
    }

    void DockNode::addNode(DockNode* p_docknode)
    {
        if(p_docknode == nullptr)
        {
            std::cout << "Tried adding nullptr." << std::endl;
            return;
        }
        if(p_docknode == this)
        {
            std::cout << "Tried adding this node as child." << std::endl;
            return;
        }
        if(std::find(child_nodes.begin(), child_nodes.end(), p_docknode) != std::end(child_nodes))
        {
            std::cout << "Tried adding node that was already added to this node!" << std::endl;
            return;
        }

        child_nodes.push_back(p_docknode);
        addChild(p_docknode);
        p_docknode->setPosition(0.f, 0.f);
        setSize(getSize());

        setLayer(0);

        recreateResizeIndicators();
        recreateTitleButtons();
    }

    bool DockNode::removeNode(DockNode* p_docknode)
    {
        auto found = std::find(child_nodes.begin(), child_nodes.end(), p_docknode);
        if(found != std::end(child_nodes))
        {
            child_nodes.erase(found);
            removeChild(p_docknode);
            setSize(getSize());

            recreateResizeIndicators();
            recreateTitleButtons();
            return true;
        }
        return false;
    }

    void DockNode::copyNodes(DockNode* p_copy_to)
    {
        std::vector<DockNode*> nodes(child_nodes.begin(), child_nodes.end());
        for (int i = 0; i < nodes.size(); i++)
        {
            removeNode(nodes[i]);
            p_copy_to->addNode(nodes[i]);
        }
    }

    int DockNode::getNodeCount()
    {
        return child_nodes.size();
    }

    DockNode* DockNode::getNode(int p_index)
    {
        if(p_index < 0 || p_index >= child_nodes.size())
        {
            std::cout << "getNode() Index out of bounds!" << std::endl;
            return nullptr;
        }
        return child_nodes[p_index];
    }

    void DockNode::updateLogic()
    {
        // Check which resize indicators are hovered.
        hovering_indicator = nullptr;
        for (int i = 0; i < resize_indicators.size(); i++)
        {
            if(resize_indicators[i]->hover_area->isHovered())
            {
                resize_indicators[i]->indicator->setFillColor(sf::Color::Blue);
                hovering_indicator = resize_indicators[i]->indicator;
            }   
            else
            {
                resize_indicators[i]->indicator->setFillColor(sf::Color::Transparent);
            }

            // Resize resize indicators.
            if(orientation == Horizontal)
            {
                resize_indicators[i]->indicator->setSize(sf::Vector2f(1.f, getSize().y));

                resize_indicators[i]->hover_area->setSize(sf::Vector2f(4.f, getSize().y));
                resize_indicators[i]->hover_area->setPosition(sf::Vector2f(resize_indicators[i]->indicator->getPosition().x - 2.f, resize_indicators[i]->indicator->getPosition().y));
            }
            else if (orientation == Vertical)
            {
                resize_indicators[i]->indicator->setSize(sf::Vector2f(getSize().x, 1.f));

                resize_indicators[i]->hover_area->setSize(sf::Vector2f(getSize().x, 4.f));
                resize_indicators[i]->hover_area->setPosition(sf::Vector2f(resize_indicators[i]->indicator->getPosition().x, resize_indicators[i]->indicator->getPosition().y - 2.f));
            }
        }

        // Move resize indicators.
        if(dragging_indicator != nullptr)
        {
            sf::FloatRect bounds = sf::FloatRect(0.f, 0.f, getSize().x, getSize().y);
            float bounds_offset = 10.f; // Small offset to the border of other widgets (looks better).
            sf::Vector2f mouse_position = getWorldTransform().getInverse().transformPoint(getMousePosition(window, &canvas.getView()));

            if(orientation == Horizontal)
            {
                dragging_indicator->setPosition((int)(mouse_position - dragging_position).x, (int)dragging_indicator->getPosition().y);

                // Keeping indicators in bounds.
                if(dragging_indicator->getPosition().x > bounds.width - bounds_offset)
                {
                    dragging_indicator->setPosition((int)bounds.width - bounds_offset, (int)dragging_indicator->getPosition().y);
                }
                else if(dragging_indicator->getPosition().x < bounds.left + bounds_offset)
                {
                    dragging_indicator->setPosition((int)bounds.left + bounds_offset, (int)dragging_indicator->getPosition().y);
                }
            }
            else if (orientation == Vertical)
            {
                dragging_indicator->setPosition((int)dragging_indicator->getPosition().x, (int)(mouse_position - dragging_position).y);

                // Keeping indicators in bounds.
                if(dragging_indicator->getPosition().y > bounds.height - bounds_offset)
                {
                    dragging_indicator->setPosition((int)dragging_indicator->getPosition().x, (int)bounds.height - bounds_offset);
                }
                else if(dragging_indicator->getPosition().y < bounds.top + bounds_offset)
                {
                    dragging_indicator->setPosition((int)dragging_indicator->getPosition().x, (int)bounds.top + bounds_offset);
                }
            }
        }

        resizeNodes();
    }

    void DockNode::updateEvents(sf::Event& p_event) 
    {
        // Dragging events.
        if(p_event.type == sf::Event::MouseButtonPressed)
        {
            if(p_event.mouseButton.button == sf::Mouse::Left)
            {
                if(hovering_indicator != nullptr)
                {
                    setLayer(0); // Puts this node along with its panels above all other nodes (all docknodes are supposed to have the layer 0, while free-floating panels are on 1).

                    dragging_indicator = hovering_indicator;
                    dragging_position = getWorldTransform().getInverse().transformPoint(getMousePosition(window, &canvas.getView())) - dragging_indicator->getPosition();

                    ui::Cursor::setCursor(orientation == Horizontal ? sf::Cursor::SizeHorizontal : sf::Cursor::SizeVertical, window);
                }

                dragged_panel = nullptr;
                for (int i = 0; i < buttons.size(); i++)
                {
                    if(buttons[i]->isHovered() || buttons[i]->areChildrenHovered())
                    {
                        if(i >= 0 && i < panels.size())
                        {
                            dragged_panel = panels[i];
                        }
                    }
                }
            }
        }
        if(p_event.type == sf::Event::MouseButtonReleased)
        {
            if(p_event.mouseButton.button == sf::Mouse::Left)
            {
                dragging_indicator = nullptr;
                ui::Cursor::setCursor(sf::Cursor::Arrow, window);
            }
        }
    }

    void DockNode::setLayer(int p_layer)
    {
        Widget::setLayer(p_layer);

        // All resize indicators are on layer 1, so they can always be interacted with, but they are only visible while hovering.
        for (int i = 0; i < resize_indicators.size(); i++)
        {
            resize_indicators[i]->indicator->setLayer(1);
            resize_indicators[i]->hover_area->setLayer(1);
        }
    }

    void DockNode::setPanelLayout(PanelLayout p_orientation)
    {
        orientation = p_orientation;
        setSize(getSize());

        if(orientation == Horizontal || orientation == Vertical)
        {
            recreateResizeIndicators();
            deleteTitleButtons();
        }
        else if(orientation == Center)
        {
            recreateTitleButtons();
            deleteResizeIndicators();
        }
    }

    void DockNode::setPanelState(PanelState p_panel_state)
    {
        panel_state = p_panel_state;

        for (int i = 0; i < panels.size(); i++)
        {
            if(p_panel_state == Static)
            {
                addChild(panels[i]);
                panels[i]->setPosition(0.f, 0.f);
                panels[i]->setDraggable(false);
                panels[i]->setResizable(false);
                panels[i]->setLayer(0);

                recreateTitleButtons();
            }
            else if(p_panel_state == Floating)
            {
                removeChild(panels[i]);
                panels[i]->setDraggable(true);
                panels[i]->setResizable(true);
                panels[i]->setLayer(1);

                deleteTitleButtons();
            }
        }
    }

    DockNode::PanelState DockNode::getPanelState()
    {
        return panel_state;
    }

    Panel* DockNode::getDraggedPanel()
    {
        return dragged_panel;
    }

    void DockNode::setSize(const sf::Vector2f& p_size)
    {
        Widget::setSize(p_size);
        if(panel_state == Static)
        {
            for (int i = 0; i < panels.size(); i++)
            {
                panels[i]->setPosition(0.f, 0.f);
                panels[i]->setSize(sf::Vector2f(getSize().x, getSize().y));
            }
        }

        for (int i = 0; i < child_nodes.size(); i++)
        {
            if(orientation == Horizontal)
            {
                child_nodes[i]->setSize(sf::Vector2f(child_nodes[i]->getSize().x, getSize().y));
            }
            else if(orientation == Vertical)
            {
                child_nodes[i]->setSize(sf::Vector2f(getSize().x, child_nodes[i]->getSize().y));
            }
            else if(orientation == Center)
            {
                child_nodes[i]->setSize(getSize());
            }
        }
    }

    void DockNode::setSizeProportionally(const sf::Vector2f& old_size, const sf::Vector2f& new_size)
    {
        sf::Vector2f last_indicator_position = sf::Vector2f();
        for (int i = 0; i < child_nodes.size(); i++)
        {
            sf::Vector2f child_old_size;
            sf::Vector2f child_new_size;

            // Get old child size.
            child_old_size = sf::Vector2f(orientation == Horizontal ? child_nodes[i]->getSize().x : old_size.x, orientation == Horizontal ? old_size.y : child_nodes[i]->getSize().y);

            // Resize.
            if(i != child_nodes.size() - 1)
            {
                sf::Vector2f ratio = sf::Vector2f(resize_indicators[i]->indicator->getPosition().x/old_size.x, resize_indicators[i]->indicator->getPosition().y/old_size.y);
                
                if(orientation == Horizontal)
                {
                    resize_indicators[i]->indicator->setPosition(ratio.x * new_size.x, resize_indicators[i]->indicator->getPosition().y);
                }
                else if(orientation == Vertical)
                {
                    resize_indicators[i]->indicator->setPosition(resize_indicators[i]->indicator->getPosition().x, ratio.y * new_size.y);
                }
            }

            // Get new child size.
            if(i == child_nodes.size() - 1)
            {
                child_new_size = new_size - last_indicator_position;
            }
            else
            {
                child_new_size = resize_indicators[i]->indicator->getPosition() + (orientation == Horizontal ? sf::Vector2f(0.f, new_size.y) : sf::Vector2f(new_size.x, 0.f)) - last_indicator_position;
                last_indicator_position = resize_indicators[i]->indicator->getPosition();
            }

            child_nodes[i]->setSizeProportionally(child_old_size, child_new_size);
        }
    }

    void DockNode::addPanel(Panel* p_panel)
    {
        if(p_panel == nullptr) return;
        if(std::find(panels.begin(), panels.end(), p_panel) != std::end(panels)) return;

        panels.push_back(p_panel);
        setPanelState(panel_state);
        setSize(getSize());
        
        if(panel_state == Static)
        {
            recreateTitleButtons();
        }
    }

    void DockNode::removePanel(Panel* p_panel)
    {
        auto it = std::find(panels.begin(), panels.end(), p_panel);
        if(it != std::end(panels))
        {
            panels.erase(it);

            removeChild(p_panel);
            p_panel->setDraggable(true);
            p_panel->setResizable(true);
            p_panel->setLayer(1);
            p_panel->setSize(sf::Vector2f(200.f, 300.f));
            p_panel->setVisible(true);
            setSize(getSize());

            if(panels.size() > 0) panels[0]->setVisible(true);

            dragged_panel = nullptr;

            if(panel_state == Static)
            {
                recreateTitleButtons();
            }
        }
    }

    void DockNode::recreateResizeIndicators()
    {
        deleteResizeIndicators();

        for (int i = 1; i < child_nodes.size(); i++)
        {
            Graphic* indicator = new Graphic(canvas);
            addChild(indicator);

            Widget* hover_area = new Widget(canvas);
            addChild(hover_area);

            indicator->setLayer(1);
            hover_area->setLayer(1);

            ResizeIndicator* resize_indicator = new ResizeIndicator();
            resize_indicator->indicator = indicator;
            resize_indicator->hover_area = hover_area;
            resize_indicators.push_back(resize_indicator);
        }

        if(child_nodes.size() <= 0) return;

        float width = getSize().x / child_nodes.size();
        float height = getSize().y / child_nodes.size();
        for (int i = 0, k = 1; i < resize_indicators.size(); i++, k++)
        {
            if(orientation == Horizontal)
            {
                resize_indicators[i]->indicator->setPosition(k * width, resize_indicators[i]->indicator->getPosition().y);
            }
            else if (orientation == Vertical)
            {
                resize_indicators[i]->indicator->setPosition(resize_indicators[i]->indicator->getPosition().x, k * height);
            }
        }

        resizeNodes();
    }

    void DockNode::deleteResizeIndicators()
    {
        for (int i = 0; i < resize_indicators.size(); i++)
        {
            removeChild(resize_indicators[i]->indicator);
            removeChild(resize_indicators[i]->hover_area);
            delete resize_indicators[i]->indicator;
            delete resize_indicators[i]->hover_area;
            delete resize_indicators[i];
        }
        resize_indicators.clear();
    }

    void DockNode::resizeNodes()
    {
        // Move/resize nodes accordingly to the resize indicators.
        for (int i = 0; i < child_nodes.size(); i++)
        {
            if(orientation == Horizontal)
            {
                if(i == 0)
                {
                    child_nodes[i]->setPosition(0.f, (int)child_nodes[i]->getPosition().y);
                    if(child_nodes.size() == 1)
                    {
                        child_nodes[i]->setSize(sf::Vector2f(getSize().x, child_nodes[i]->getSize().y));
                    }
                    else
                    {
                        child_nodes[i]->setSize(sf::Vector2f(resize_indicators[i]->indicator->getPosition().x - child_nodes[i]->getPosition().x, child_nodes[i]->getSize().y));
                    }
                }
                else if (i == child_nodes.size() - 1)
                {
                    child_nodes[i]->setPosition((int)resize_indicators[i - 1]->indicator->getPosition().x, (int)child_nodes[i]->getPosition().y);
                    child_nodes[i]->setSize(sf::Vector2f(getSize().x - child_nodes[i]->getPosition().x, child_nodes[i]->getSize().y));
                }
                else
                {
                    child_nodes[i]->setPosition((int)resize_indicators[i - 1]->indicator->getPosition().x, (int)child_nodes[i]->getPosition().y);
                    child_nodes[i]->setSize(sf::Vector2f(resize_indicators[i]->indicator->getPosition().x - child_nodes[i]->getPosition().x, child_nodes[i]->getSize().y));
                }
            }
            else if(orientation == Vertical)
            {
                if(i == 0)
                {
                    child_nodes[i]->setPosition((int)child_nodes[i]->getPosition().x, 0.f);
                    if(child_nodes.size() == 1)
                    {
                        child_nodes[i]->setSize(sf::Vector2f(child_nodes[i]->getSize().x, getSize().y));
                    }
                    else
                    {
                        child_nodes[i]->setSize(sf::Vector2f(child_nodes[i]->getSize().x, resize_indicators[i]->indicator->getPosition().y - child_nodes[i]->getPosition().y));
                    }
                }
                else if (i == child_nodes.size() - 1)
                {
                    child_nodes[i]->setPosition((int)child_nodes[i]->getPosition().x, (int)resize_indicators[i - 1]->indicator->getPosition().y);
                    child_nodes[i]->setSize(sf::Vector2f(child_nodes[i]->getSize().x, getSize().y - child_nodes[i]->getPosition().y));
                }
                else
                {
                    child_nodes[i]->setPosition((int)child_nodes[i]->getPosition().x, (int)resize_indicators[i - 1]->indicator->getPosition().y);
                    child_nodes[i]->setSize(sf::Vector2f(child_nodes[i]->getSize().x, resize_indicators[i]->indicator->getPosition().y - child_nodes[i]->getPosition().y));
                }
            }
        }
    }

    void DockNode::recreateTitleButtons()
    {
        deleteTitleButtons();

        for (int i = 0; i < panels.size(); i++)
        {
            Button* button = new Button(canvas);
            addChild(button);
            button->text.setString(panels[i]->getTitle());
            button->text.setTextColor(sf::Color::Black);
            button->setSize(sf::Vector2f(button_width, 20.f));
            button->setPosition(i * button_width, 0.f);
            button->setStandardColor(sf::Color(240, 240, 240));
            button->setLayer(1);
            buttons.push_back(button);
            Panel* current_panel = panels[i];
            button->setOnClickEvent([this, current_panel, button]()
            {
                for (int i = 0; i < panels.size(); i++)
                {
                    panels[i]->setVisible(false);
                    buttons[i]->setStandardColor(sf::Color(240, 240, 240));
                }
                current_panel->setVisible(true);
                button->setStandardColor(sf::Color(255, 255, 255));
            });
        }

        // Default set the first panel active.
        for (int i = 0; i < panels.size(); i++)
        {
            panels[i]->setVisible(false);
            panels[i]->setDragging(false);
            buttons[i]->setStandardColor(sf::Color(240, 240, 240));
        }
        if(panels.size() >= 1)
        {
            panels[0]->setVisible(true);
            buttons[0]->setStandardColor(sf::Color(255, 255, 255));
        }
    }

    void DockNode::deleteTitleButtons()
    {
        for (int i = 0; i < buttons.size(); i++)
        {
            removeChild(buttons[i]);
            delete buttons[i];
        }
        buttons.clear();
    }

    DockSpace::DockSpace(Canvas& p_canvas)
        : Widget::Widget(p_canvas), top_marker(canvas), bottom_marker(canvas), left_marker(canvas), right_marker(canvas), center_marker(canvas)
    {
        setSelectable(false);
        top_marker.setFillColor(sf::Color::Blue);
        bottom_marker.setFillColor(sf::Color::Blue);
        left_marker.setFillColor(sf::Color::Blue);
        right_marker.setFillColor(sf::Color::Blue);
        center_marker.setFillColor(sf::Color::Blue);

        top_marker.setSize(sf::Vector2f(40.f, 40.f));
        bottom_marker.setSize(sf::Vector2f(40.f, 40.f));
        left_marker.setSize(sf::Vector2f(40.f, 40.f));
        right_marker.setSize(sf::Vector2f(40.f, 40.f));
        center_marker.setSize(sf::Vector2f(30.f, 30.f));

        top_marker.setVisible(false);
        bottom_marker.setVisible(false);
        left_marker.setVisible(false);
        right_marker.setVisible(false);
        center_marker.setVisible(false);

        top_marker.setSelectable(false);
        bottom_marker.setSelectable(false);
        left_marker.setSelectable(false);
        right_marker.setSelectable(false);
        center_marker.setSelectable(false);

        // Create root_node.
        root = createPanel("Root Panel");
        nodes[0]->setPanelState(DockNode::Static);
        nodes[0]->setSize((sf::Vector2f)window.getSize());
    }

    DockSpace::~DockSpace()
    {
        for (int i = 0; i < panels.size(); i++)
        {
            delete panels[i];
        }
        panels.clear();
        
        for (int i = 0; i < nodes.size(); i++)
        {     
            delete nodes[i];
        }
        nodes.clear();
    }

    Panel* DockSpace::createPanel(const sf::String& p_title)
    {
        Panel* panel = new Panel(canvas);
        panels.push_back(panel);
        panel->setTitle(p_title);
        panel->setSize(sf::Vector2f(200.f, 300.f));
        
        DockNode* node = new DockNode(canvas);
        node->addPanel(panel);
        node->setPanelState(DockNode::Floating);
        nodes.push_back(node);

        return panel;
    }

    void DockSpace::insertPanel(Panel* p_floating_panel, Panel* p_inserted_panel, int p_pos)
    {
        DockNode* floating_node = nullptr;
        DockNode* inserted_node = nullptr;
        for (int i = 0; i < nodes.size(); i++)
        {
            for (int j = 0; j < nodes[i]->panels.size(); j++)
            {
                if(p_floating_panel == nodes[i]->panels[j])
                {
                    floating_node = nodes[i];
                }
                if(p_inserted_panel == nodes[i]->panels[j])
                {
                    inserted_node = nodes[i];
                }
            }
        }
        insertNode(floating_node, inserted_node, p_pos);
    }

    void DockSpace::removePanel(Panel* p_panel)
    {
        DockNode* node = nullptr;
        for (int i = 0; i < nodes.size(); i++)
        {
            for (int j = 0; j < nodes[i]->panels.size(); j++)
            {
                if(p_panel == nodes[i]->panels[j])
                {
                    node = nodes[i];
                }
            }
        }
        removeNode(node);
    }

    void DockSpace::insertNode(DockNode* p_floating_node, DockNode* p_inserted_node, int p_pos)
    {
        if(p_pos > 4 || p_pos < 0) return;

        if(p_floating_node == nullptr || p_inserted_node == nullptr)
        {
            std::cout << "Tried to insert node. Node was nullptr." << std::endl;
            return;
        }
        if(p_floating_node->getPanelState() != DockNode::PanelState::Floating)
        {
            std::cout << "Floating node was not actually floating." << std::endl;
            return;
        }
        if(p_inserted_node->getPanelState() != DockNode::PanelState::Static)
        {
            std::cout << "Static node was not actually static." << std::endl;
            return;
        }

        // Insert node in panel bar.
        if(p_pos == 4 && p_floating_node->panels.size() == 1)
        {
            p_inserted_node->addPanel(p_floating_node->panels[0]);
            nodes.erase(std::find(nodes.begin(), nodes.end(), p_floating_node));
            delete p_floating_node;
            return;
        }

        // Add dragged panel to hovered panel.
        DockNode* created_node = new DockNode(canvas);
        p_floating_node->setPanelState(DockNode::Static);
        created_node->panels = p_inserted_node->panels;
        p_inserted_node->setPanelState(DockNode::Floating);
        p_inserted_node->panels.clear();
        p_inserted_node->setPanelState(DockNode::Static);
        created_node->setPanelState(DockNode::Static);

        switch(p_pos)
        {
            case 0:
                p_inserted_node->setPanelLayout(DockNode::Vertical);
                p_inserted_node->addNode(p_floating_node);
                p_inserted_node->addNode(created_node);
            break;
            case 1:
                p_inserted_node->setPanelLayout(DockNode::Horizontal);
                p_inserted_node->addNode(created_node);
                p_inserted_node->addNode(p_floating_node);
            break;
            case 2:
                p_inserted_node->setPanelLayout(DockNode::Vertical);
                p_inserted_node->addNode(created_node);
                p_inserted_node->addNode(p_floating_node);
            break;
            case 3:
                p_inserted_node->setPanelLayout(DockNode::Horizontal);
                p_inserted_node->addNode(p_floating_node);
                p_inserted_node->addNode(created_node);
            break;
        }

        nodes.push_back(created_node);
    }

    void DockSpace::removeNode(DockNode* p_remove_node)
    {
        if(p_remove_node == nullptr)
        {
            std::cout << "Tried removing node. Node was nullptr." << std::endl;
            return;
        }

        if(p_remove_node->panels.size() > 1)
        {
            std::cout << "Only nodes with one panel can be turned into floating panels." << std::endl;
            return;
        }

        if(p_remove_node->getPanelState() != DockNode::Static)
        {
            std::cout << "Node was already removed." << std::endl;
            return;
        }

        // Make it impossible to remove the last node.
        if(p_remove_node == nodes[0])
        {
            std::cout << "Forbidden to remove last node." << std::endl;
            return;
        }
        // Remove the node.
        for (int i = 0; i < nodes.size(); i++)
        {
            if(nodes[i]->getPanelState() == DockNode::Floating) continue;

            bool success = nodes[i]->removeNode(p_remove_node);
            if(success)
            {
                p_remove_node->setPanelState(DockNode::Floating);
                p_remove_node->panels[0]->setSize(sf::Vector2f(200.f, 300.f));
            }

            if(nodes[i]->getNodeCount() == 1)
            {
                DockNode* last_node = nodes[i]->getNode(0);

                // Remove last_node.
                if(last_node->panels.size() == 0)
                {
                    //std::cout << "Case Node Nullptr" << std::endl;
                    nodes[i]->removeNode(last_node);

                    // Copy all children of last_node to root_node.
                    last_node->copyNodes(nodes[i]);

                    nodes.erase(std::find(nodes.begin(), nodes.end(), last_node));
                    delete last_node;
                    break;
                }
                else if (last_node->panels.size() > 0)
                {
                    //std::cout << "Case Node Panel" << std::endl;

                    nodes[i]->panels = last_node->panels;
                    last_node->setPanelState(DockNode::Floating);
                    last_node->panels.clear();
                    nodes[i]->setPanelState(DockNode::Static);

                    nodes[i]->removeNode(last_node);
                    nodes.erase(std::find(nodes.begin(), nodes.end(), last_node));
                    delete last_node;
                    break;
                }

            }
        }
    }

    void DockSpace::updateLogic()
    {
        hovered_node = nullptr;
        dragged_node = nullptr;
        Panel* hovered_panel = nullptr;
        for (int i = 0; i < nodes.size(); i++)
        {
            for (int j = 0; j < nodes[i]->panels.size(); j++)
            {
                if(nodes[i]->panels[j]->isDragging())
                {
                    dragged_node = nodes[i];
                }
                else if((nodes[i]->panels[j]->isHovered() || nodes[i]->panels[j]->areChildrenHovered()) && nodes[i]->getPanelState() == DockNode::Static)
                {
                    hovered_node = nodes[i];
                    hovered_panel = nodes[i]->panels[j];
                }
            }
        }

        bool show_markers = dragged_node != nullptr && hovered_node != nullptr;

        top_marker.setVisible(show_markers);
        bottom_marker.setVisible(show_markers);
        left_marker.setVisible(show_markers);
        right_marker.setVisible(show_markers);
        center_marker.setVisible(show_markers);

        if(show_markers)
        {
            // Position markers.
            sf::Vector2f hovered_panel_middle = hovered_panel->getWorldTransform().transformPoint(hovered_panel->getPosition() + hovered_panel->getSize()/2.f);
            top_marker.setPosition(hovered_panel_middle.x - (top_marker.getSize().x/2.f), hovered_panel_middle.y - (top_marker.getSize().y/2.f) - top_marker.getSize().y);
            bottom_marker.setPosition(hovered_panel_middle.x - (bottom_marker.getSize().x/2.f), hovered_panel_middle.y - (bottom_marker.getSize().y/2.f) + bottom_marker.getSize().y);
            left_marker.setPosition(hovered_panel_middle.x - (left_marker.getSize().x/2.f) - left_marker.getSize().x, hovered_panel_middle.y - (left_marker.getSize().y/2.f));
            right_marker.setPosition(hovered_panel_middle.x - (right_marker.getSize().x/2.f) + right_marker.getSize().x, hovered_panel_middle.y - (right_marker.getSize().y/2.f));
            center_marker.setPosition(hovered_panel_middle.x - (center_marker.getSize().x/2.f), hovered_panel_middle.y - (center_marker.getSize().y/2.f));
            
            // Move markers into front.
            top_marker.setLayer(hovered_panel->getLayer());
            bottom_marker.setLayer(hovered_panel->getLayer());
            left_marker.setLayer(hovered_panel->getLayer());
            right_marker.setLayer(hovered_panel->getLayer());
            center_marker.setLayer(hovered_panel->getLayer());

            // Check if mouse is hovering markers.
            insert_index = -1;
            top_marker.setFillColor(sf::Color::Blue);
            bottom_marker.setFillColor(sf::Color::Blue);
            left_marker.setFillColor(sf::Color::Blue);
            right_marker.setFillColor(sf::Color::Blue);
            center_marker.setFillColor(sf::Color::Blue);
            if(sf::FloatRect(top_marker.getPosition(), top_marker.getSize()).contains(getMousePosition(window, &canvas.getView())))
            {
                insert_index = 0;
                top_marker.setFillColor(sf::Color::Black);
            }
            else if(sf::FloatRect(right_marker.getPosition(), right_marker.getSize()).contains(getMousePosition(window, &canvas.getView())))
            {
                insert_index = 1;
                right_marker.setFillColor(sf::Color::Black);
            }
            else if(sf::FloatRect(bottom_marker.getPosition(), bottom_marker.getSize()).contains(getMousePosition(window, &canvas.getView())))
            {
                insert_index = 2;
                bottom_marker.setFillColor(sf::Color::Black);
            }
            else if(sf::FloatRect(left_marker.getPosition(), left_marker.getSize()).contains(getMousePosition(window, &canvas.getView())))
            {
                insert_index = 3;
                left_marker.setFillColor(sf::Color::Black);
            }
            else if(sf::FloatRect(center_marker.getPosition(), center_marker.getSize()).contains(getMousePosition(window, &canvas.getView())))
            {
                insert_index = 4;
                center_marker.setFillColor(sf::Color::Black);
            }
        }
    }

    void DockSpace::updateEvents(sf::Event& p_event)
    {
        if(p_event.type == sf::Event::Resized)
        {
            // Resize root_node.
            nodes[0]->setPosition(0.f, 0.f);

            sf::Vector2f old_size = nodes[0]->getSize();
            nodes[0]->setSize((sf::Vector2f)window.getSize());
            nodes[0]->setSizeProportionally(old_size, (sf::Vector2f)window.getSize());
        }

        if(p_event.type == sf::Event::MouseButtonReleased)
        {
            if(p_event.mouseButton.button == sf::Mouse::Left)
            {
                if(dragged_node != nullptr && hovered_node != nullptr)
                {
                    insertNode(dragged_node, hovered_node, insert_index);
                }
            }
        }

        if(p_event.type == sf::Event::MouseButtonPressed)
        {
            if(p_event.mouseButton.button == sf::Mouse::Left)
            {
                mouse_press_position = getMousePosition(window, &canvas.getView());
            }
        }

        if(p_event.type == sf::Event::MouseMoved)
        {
            sf::Vector2f current_mouse_position = getMousePosition(window, &canvas.getView());
            sf::Vector2f mouse_difference = current_mouse_position - mouse_press_position;
            float mouse_move_distance = sqrtf(mouse_difference.x * mouse_difference.x + mouse_difference.y * mouse_difference.y);

            if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && mouse_move_distance > 5.f)
            {
                for (int i = 0; i < nodes.size(); i++)
                {
                    // Remove panels.
                    if(nodes[i]->panels.size() == 1)
                    {
                        Panel* panel_to_remove = nodes[i]->getDraggedPanel();
                        if(panel_to_remove != nullptr)
                        {
                            removeNode(nodes[i]);
                            panel_to_remove->setDragging(true);
                            break;
                        }
                    }

                    // Remove panels from panel bar.
                    if(nodes[i]->panels.size() > 1)
                    {
                        Panel* panel_to_remove = nodes[i]->getDraggedPanel();
                        if(panel_to_remove != nullptr)
                        {
                            nodes[i]->removePanel(panel_to_remove);
                            DockNode* node = new DockNode(canvas);
                            nodes.push_back(node);
                            node->addPanel(panel_to_remove);
                            node->setPanelState(ui::DockNode::Floating);
                            panel_to_remove->setPosition(getMousePosition(window, &window.getView()));
                            panel_to_remove->setDragging(true);
                            break;
                        }
                    }
                }
            }
        }
    }

    sf::String DockSpace::printNodes()
    {
        std::stringstream ss;
        ss << "Total node count: " << nodes.size() << std::endl;
        for (int i = 0; i < nodes.size(); i++)
        {
            ss << i << ". node "; 
            ss << "(with " << nodes[i]->getNodeCount() << " children)";
            ss << ", Panels: " << nodes[i]->panels.size();
            ss << ", State: " << (nodes[i]->getPanelState() == DockNode::Static ? "Static" : "Floating") << std::endl;
        }

        ss << "Dragged node: " << (dragged_node == nullptr ? "null" : "node") << std::endl;
        ss << "Hovered node: " << (hovered_node == nullptr ? "null" : "node") << std::endl;

        return ss.str();
    }

    Panel::Panel(Canvas& p_canvas)
        : Widget::Widget(p_canvas), 
        resize_hover_area { Graphic(canvas), Graphic(canvas), Graphic(canvas), Graphic(canvas) },
        background(canvas), 
        title_background(canvas), 
        title(canvas), 
        resize_indicator(canvas),
        scrolllist(canvas)
    {
        for (int i = 0; i < resize_hover_area.size(); i++)
        {
            addChild(&resize_hover_area[i]);
            resize_hover_area[i].setFillColor(sf::Color::Transparent);
        }
        addChild(&background);
        addChild(&title_background);
        addChild(&title);
        addChild(&resize_indicator);
        addChild(&scrolllist);
        background.setFillColor(sf::Color::White);
        background.setOutlineColor(sf::Color(76, 74, 72));
        background.setOutlineThickness(1.f);
        title_background.setFillColor(sf::Color(240, 240, 240));
        title_background.setOutlineColor(sf::Color(76, 74, 72));
        title_background.setOutlineThickness(1.f);

        title.text.setTextColor(sf::Color::Black);
        title.text.setVerticalAlignment(TextField::Alignment::Middle);
        title.text.setOverflowType(ui::TextField::Dotted);
        title.text.setSize(sf::Vector2f(100.f, 20.f));
        title.text.setString("New Panel");
        title.setStandardColor(sf::Color::Transparent);

        resize_indicator.setFillColor(sf::Color::Blue);
        resize_indicator.setSize(sf::Vector2f());

        setSize(sf::Vector2f(200.f, 300.f));
    }

    Panel::~Panel()
    {
        // for(auto i : items)
        // {
        //     delete i.widget;
        // }
        std::vector<Widget*> widgets;
        for (int i = 0; i < scrolllist.getContentCount(); i++)
        {
            widgets.push_back(scrolllist.getContent(i));
        }
        scrolllist.clearContent();
        for (int i = 0; i < widgets.size(); i++)
        {
            delete widgets[i];
        }
        widgets.clear();
    }

    void Panel::setDraggable(bool p_state)
    {
        draggable = p_state;
    }

    void Panel::setDragging(bool p_state)
    {
        is_dragging = p_state;
    }

    bool Panel::isDragging()
    {
        return is_dragging;
    }

    void Panel::setResizable(bool p_state)
    {
        is_resizeable = p_state;
    }

    bool Panel::isDraggingTitle()
    {
        return is_dragging_title;
    }

    void Panel::updateLogic()
    {
        //refresh();

        if(is_dragging && draggable)
        {
            setPosition(getMousePosition(window, &canvas.getView()) - mouse_offset);
        }

        if(!is_resizeable) return;

        resize_hover_area[0].setPosition(-5.f, 0.f);
        resize_hover_area[0].setSize(sf::Vector2f(5.f, getSize().y));
        resize_hover_area[1].setPosition(0.f, -5.f);
        resize_hover_area[1].setSize(sf::Vector2f(getSize().x, 5.f));
        resize_hover_area[2].setPosition(getSize().x, 0.f);
        resize_hover_area[2].setSize(sf::Vector2f(5.f, getSize().y));
        resize_hover_area[3].setPosition(0.f, getSize().y);
        resize_hover_area[3].setSize(sf::Vector2f(getSize().x, 5.f));

        is_hovering = -1;
        for (int i = 0; i < resize_hover_area.size(); i++)
        {
            if(resize_hover_area[i].isHovered())
            {
                is_hovering = i;
            }
        }
        if(is_hovering > -1)
        {
            resize_indicator.setPosition(resize_hover_area[is_hovering].getPosition());
            if(is_hovering == 0) resize_indicator.setPosition(-1.f, 0.f);
            if(is_hovering == 1) resize_indicator.setPosition(0.f, -1.f);
            if(is_hovering == 0 || is_hovering == 2) resize_indicator.setSize(sf::Vector2f(1.f, resize_hover_area[is_hovering].getSize().y));
            else resize_indicator.setSize(sf::Vector2f(resize_hover_area[is_hovering].getSize().x, 1.f));
        }
        else
        {
            resize_indicator.setSize(sf::Vector2f());
        }

        sf::Vector2f mouse_position = getMousePosition(window, &canvas.getView()) - mouse_offset;
        switch(is_resizing)
        {
            case 0:
            setSize(sf::Vector2f(resize_begin_position.x - mouse_position.x, getSize().y));
            setPosition(mouse_position.x, getPosition().y);
            break;
            case 1:
            setSize(sf::Vector2f(getSize().x, resize_begin_position.y - mouse_position.y));
            setPosition(getPosition().x, mouse_position.y);
            break;
            case 2:
            setSize(sf::Vector2f(mouse_position.x + resize_begin_position.x - getPosition().x - getPosition().x, getSize().y));
            break;
            case 3:
            setSize(sf::Vector2f(getSize().x, mouse_position.y + resize_begin_position.y - getPosition().y - getPosition().y));
            break;
        }
    }

    void Panel::updateEvents(sf::Event& p_event)
    {
        if(p_event.type == sf::Event::MouseButtonPressed)
        {
            if(p_event.mouseButton.button == sf::Mouse::Left)
            {
                is_resizing = -1;
                if(is_resizeable)
                {
                    for (int i = 0; i < resize_hover_area.size(); i++)
                    {
                        if(resize_hover_area[i].isHovered())
                        {
                            is_resizing = i;
                            resize_begin_position = getPosition() + getSize();
                            mouse_offset = getMousePosition(window, &canvas.getView()) - getPosition();

                            if(is_resizing == 0 || is_resizing == 2) ui::Cursor::setCursor(sf::Cursor::SizeHorizontal, window);
                            if(is_resizing == 1 || is_resizing == 3) ui::Cursor::setCursor(sf::Cursor::SizeVertical, window);
                        }
                    }
                }
                
                if((isHovered() || areChildrenHovered()) && is_resizing == -1)
                {
                    is_dragging = true;
                    if(draggable)
                    {
                        mouse_offset = getMousePosition(window, &canvas.getView()) - getPosition();
                        canvas.moveInFront(this);
                        setSelectable(false);
                        background.setFillColor(sf::Color(255, 255, 255, 100));
                        ui::Cursor::setCursor(sf::Cursor::Type::Hand, window);
                    }
                }

                if(title.isHovered() || title.areChildrenHovered())
                {
                    is_dragging_title = true;
                }
            }
        }
        if(p_event.type == sf::Event::MouseButtonReleased)
        {
            if(p_event.mouseButton.button == sf::Mouse::Left)
            {
                is_resizing = -1;
                is_dragging = false;
                is_dragging_title = false;
                setSelectable(true);
                background.setFillColor(sf::Color(255, 255, 255, 255));
                ui::Cursor::setCursor(sf::Cursor::Type::Arrow, window);
            }
        }
    }

    void Panel::setTitle(const sf::String& p_string)
    {
        title.text.setString(p_string);
    }

    const sf::String& Panel::getTitle()
    {
        return title.text.getString();
    }

    void Panel::setSize(const sf::Vector2f& p_size)
    {
        Widget::setSize(p_size);
        background.setSize(p_size);
        title.setSize(sf::Vector2f(100.f, 20.f));
        title_background.setSize(sf::Vector2f(p_size.x, 20.f));
        scrolllist.setPosition(sf::Vector2f(0.f, 20.f));
        scrolllist.setSize(sf::Vector2f(p_size.x, p_size.y - 20.f));

        //refresh();
    }

    // void Panel::refresh()
    // {
    //     float height = 0.f;
    //     float left = 0.f;
    //     for (int i = 0; i < items.size(); i++)
    //     {
    //         if(items[i].widget->isVisible() == false) continue;

    //         items[i].widget->setPosition(0.f + left, 20.f + height);
    //         left += items[i].widget->getSize().x + 5.f;
            
    //         if(items[i].break_line)
    //         {
    //             left = 0.f;
    //             height += items[i].widget->getSize().y + 5.f;
    //         }
    //     }
    // }

    void Panel::setLayer(int layer_index)
    {
        Widget::setLayer(layer_index);
    }

    void Panel::breakLine()
    {
        //if(items.size() == 0) return;
        //items[items.size() - 1].break_line = true;
        scrolllist.breakLine();
    }

    ui::Graphic* Panel::createGraphic()
    {
        Graphic* widget = new Graphic(canvas);
        widget->setMask(this);

        scrolllist.addContent(widget);
        // items.push_back({ widget });
        // addChild(widget);
        // setLayer(getLayer()); // Put all new children on the current layer of the panel.
        // refresh();

        return widget;
    }

    ui::TextField* Panel::createText(const sf::String& p_content)
    {
        TextField* widget = new TextField(canvas);
        widget->setString(p_content);
        widget->setSize(sf::Vector2f(getSize().x, 100.f));
        widget->setOverflowType(TextField::OverflowType::Dotted);
        widget->matchSizeToText();
        widget->setMask(this);

        scrolllist.addContent(widget);
        // items.push_back({ widget });
        // addChild(widget);
        // setLayer(getLayer()); // Put all new children on the current layer of the panel.
        // refresh();
        
        return widget;
    }

    ui::Button* Panel::createButton(const sf::String& p_text)
    {
        Button* widget = new Button(canvas);
        widget->text.setString(p_text);
        widget->text.matchSizeToText();
        widget->setSize(sf::Vector2f(widget->text.getSize().x + 20.f, widget->text.getSize().y));
        widget->setMask(this);

        scrolllist.addContent(widget);
        // items.push_back({ widget });
        // addChild(widget);
        // setLayer(getLayer()); // Put all new children on the current layer of the panel.
        // refresh();

        return widget;
    }

    ui::Button* Panel::createButton(const sf::String& p_text, const std::function<void()>& p_action)
    {
        Button* widget = new Button(canvas);
        widget->text.setString(p_text);
        widget->text.matchSizeToText();
        widget->setSize(sf::Vector2f(widget->text.getSize().x + 20.f, widget->text.getSize().y));
        widget->setOnClickEvent(p_action);
        widget->setMask(this);

        scrolllist.addContent(widget);
        // items.push_back({ widget });
        // addChild(widget);
        // setLayer(getLayer()); // Put all new children on the current layer of the panel.
        // refresh();

        return widget;
    }

    ui::ScrollList* Panel::createScrollList()
    {
        ScrollList* widget = new ScrollList(canvas);
        widget->setMask(this);

        scrolllist.addContent(widget);
        // items.push_back({ widget });
        // addChild(widget);
        // setLayer(getLayer()); // Put all new children on the current layer of the panel.
        // refresh();

        return widget;
    }

    ui::DropDownList* Panel::createDropDownList()
    {
        DropDownList* widget = new DropDownList(canvas);
        widget->setMask(this);

        scrolllist.addContent(widget);
        // items.push_back({ widget });
        // addChild(widget);
        // setLayer(getLayer()); // Put all new children on the current layer of the panel.
        // refresh();

        return widget;
    }

    ui::InputField* Panel::createInputField()
    {
        InputField* widget = new InputField(canvas);
        widget->setMask(this);
        
        scrolllist.addContent(widget);
        // items.push_back({ widget });
        // addChild(widget);
        // setLayer(getLayer()); // Put all new children on the current layer of the panel.
        // refresh();

        return widget;
    }

    ui::InputList* Panel::createInputList()
    {
        InputList* widget = new InputList(canvas);
        widget->setMask(this);

        scrolllist.addContent(widget);
        // items.push_back({ widget });
        // addChild(widget);
        // setLayer(getLayer()); // Put all new children on the current layer of the panel.
        // refresh();

        return widget;
    }
    
    ScrollList::ScrollList(Canvas& p_canvas)
        : Widget::Widget(p_canvas), background(p_canvas), scroll_bar(p_canvas)
    {
        addChild(&background);
        addChild(&scroll_bar);

        setSize(sf::Vector2f(200.f, 300.f));

        background.setOutlineThickness(1.f);
        background.setOutlineColor(sf::Color(70, 70, 70));
        background.setFillColor(sf::Color::Transparent);

        scroll_bar.setMask(this);
        scroll_bar.setOnScrollListener([&]()
        {
            int starting_position;
            if(scroll_bar.getScrollState().scroll_list_length <= scroll_bar.getScrollState().scroll_handle_size) 
            {
                starting_position = 0;
            }
            else 
            {
                starting_position = (scroll_bar.getScrollState().scroll_handle_position / scroll_bar.getScrollState().scroll_list_length) * getGroupSize().y;
            } 
            list(5.f, 5.f, sf::Vector2f(0.f, -starting_position));
        });
    }

    void ScrollList::setSize(const sf::Vector2f& p_size)
    {
        Widget::setSize(p_size);
        scroll_bar.setSize(sf::Vector2f(scroll_bar.getSize().x, getSize().y));
        scroll_bar.setPosition(sf::Vector2f(getSize().x - scroll_bar.getSize().x, 0.f));
        background.setSize(p_size);

        refreshScrollbar();
    }

    void ScrollList::setMask(Widget* p_widget)
    {
        Widget::setMask(p_widget);
        for (int i = 0; i < getContentCount(); i++)
        {
            getContent(i)->setMask(this);
        }
        scroll_bar.setMask(this);
    }

    void ScrollList::setLayer(int p_layer)
    {
        // Layers have to be set in this specific order, so that the children are always between background and scroll_bar.
        Widget::setLayer(p_layer);
        background.setLayer(p_layer);
        for (int i = 0; i < getContentCount(); i++)
        {
            getContent(i)->setLayer(p_layer);
        }
        scroll_bar.setLayer(p_layer);
    }

    void ScrollList::addContent(Widget* p_widget)
    {
        if(std::find(content.begin(), content.end(), p_widget) != std::end(content))
        {
            std::cout << "Tried adding content twice to scrolllist." << std::endl;
            return;
        }
        content.push_back(p_widget);
        p_widget->setMask(this);
        addChild(p_widget);
        refresh();
        std::cout << "Added content to scrolllist (" << getContentCount() << ")." << std::endl;
    }

    void ScrollList::removeContent(Widget* p_widget)
    {
        auto found = std::find(content.begin(), content.end(), p_widget);
        if(found == std::end(content))
        {
            std::cout << "Tried removing widget that wasn't in the scrolllist's content." << std::endl;
            return;
        }
        content.erase(found);
        removeChild(p_widget);
        refresh();
        std::cout << "Removed content to scrolllist (" << getContentCount() << ")." << std::endl;
    }

    void ScrollList::clearContent()
    {
        for (int i = 0; i < getContentCount(); i++)
        {
            removeChild(getContent(i));
        }
        content.clear();
        refresh();
    }

    Widget* ScrollList::getContent(int p_index) const
    {
        if(p_index < 0 || p_index > content.size() - 1)
        {
            std::cout << "Content index ouf of bounds. Returned nullptr." << std::endl;
            return nullptr;
        }
        return content[p_index];
    }

    int ScrollList::getContentCount() const
    {
        return content.size();
    }

    void ScrollList::breakLine()
    {
        if(content.size() <= 0) return;
        break_indices.push_back(content.size() - 1);
    }

    void ScrollList::refresh()
    {
        setLayer(getLayer()); // Move the content between background and scrollbar of the scrolllist.
        list(5.f, 5.f);
        refreshScrollbar();
    }

    void ScrollList::updateEvents(sf::Event& p_event) 
    {
        if(p_event.type == sf::Event::MouseWheelScrolled)
        {
            /* ScrollList should only be scrollable if it or its content is hovered. An exception are ScrollLists inside ScrollLists. 
            When hovering over the child, the parent ScrollList should not be scrollable. dynamic_cast is used to check whether the underlying widget is in fact a ScrollList. 
            If the child scrolllist is hovered or any of its children you can no longer scroll the parent scrolllist. */
            for (int i = 0; i < getContentCount(); i++)
            {
                ScrollList* scroll_list = dynamic_cast<ScrollList*>(getContent(i)); 
                if(scroll_list != nullptr && (scroll_list->isHovered() || scroll_list->areChildrenHovered()))
                {
                    return;
                }
            }

            // Scroll if anything else is hovered.
            if(isHovered() || areChildrenHovered())
            {
                scroll_bar.setScrollState(scroll_bar.getScrollState().scroll_handle_position - (10.f * p_event.mouseWheelScroll.delta), getSize().y, getGroupSize().y);
            }
        }
    }

    void ScrollList::refreshScrollbar()
    {
        scroll_bar.setScrollState(scroll_bar.getScrollState().scroll_handle_position, getSize().y, getGroupSize().y);
        scroll_bar.setVisible(isVisible() && getSize().y < getGroupSize().y);
    }

    sf::Vector2f ScrollList::getGroupSize() const
    {
        if(getContentCount() < 1) return sf::Vector2f();
        sf::Vector2f smallest_position = getContent(0)->getPosition();
        sf::Vector2f biggest_position = getContent(0)->getPosition() + getContent(0)->getSize();
        for (int i = 0; i < getContentCount(); i++)
        {
            if(getContent(i)->getPosition().x < smallest_position.x || getContent(i)->getPosition().y < smallest_position.y)
            {
                smallest_position = getContent(i)->getPosition();
            }
            sf::Vector2f current_biggest_position = getContent(i)->getPosition() + getContent(i)->getSize();
            if(current_biggest_position.x > biggest_position.x || current_biggest_position.y > biggest_position.y)
            {
                biggest_position = current_biggest_position;
            }
        }
        return biggest_position - smallest_position;
    }

    void ScrollList::list(float p_horizontal_spacing, float p_vertical_spacing, const sf::Vector2f& p_position)
    {
        float current_height = 0.f;
        float current_left = 0.f;
        for (int i = 0; i < getContentCount(); i++)
        {
            if(!getContent(i)->isVisible()) continue;

            getContent(i)->setPosition(p_position + sf::Vector2f(current_left, current_height));

            current_left += getContent(i)->getSize().x + p_horizontal_spacing;

            if(std::find(break_indices.begin(), break_indices.end(), i) != break_indices.end())
            {
                current_left = 0.f;
                current_height += getContent(i)->getSize().y + p_vertical_spacing;
            }
        }
    }

    DropDownList::DropDownList(Canvas& p_canvas)
        : Widget::Widget(p_canvas), dropdown_button(p_canvas), placeholder_text(p_canvas), scroll_list(p_canvas), icon(p_canvas)
    {
        addChild(&dropdown_button);
        addChild(&placeholder_text);
        addChild(&scroll_list);
        addChild(&icon);

        scroll_list.setVisible(false);
        scroll_list.background.setFillColor(sf::Color::White);
        scroll_list.background.setOutlineThickness(0.f);

        setString("New DropDownList");
        dropdown_button.setOnClickEvent([&]()
        {
            if(!scroll_list.isVisible())
                f_onSelect();
            scroll_list.setVisible(true);
        });
        dropdown_button.setTextMargin(5.f, 0.f, 0.f, 0.f);
        dropdown_button.text.setHorizontalAlignment(ui::TextField::Left);

        placeholder_text.setSelectable(false);
        placeholder_text.setVerticalAlignment(ui::TextField::Middle);
        placeholder_text.setTextColor(sf::Color(100, 100, 100));

        icon.setSelectable(false);
        icon.setTexture(canvas.dropdown_arrow_texture);

        f_onSelect = [](){};
        f_onDeselect = [](){};

        setSize(sf::Vector2f(300.f, 20.f));
        setList({ "Option A", "Option B", "Option C", "Option D", "Option E", "Option F" });
    }

    void DropDownList::setSize(const sf::Vector2f& p_size)
    {
        Widget::setSize(p_size);
        dropdown_button.setSize(p_size);
        placeholder_text.setSize(p_size);
        scroll_list.setPosition(0.f, dropdown_button.getSize().y);
        scroll_list.setSize(sf::Vector2f(p_size.x, 100.f));
        icon.setSize(sf::Vector2f(p_size.y, p_size.y));
        icon.setPosition(getPosition().x + getSize().x - icon.getSize().x, getPosition().y);

        // Resize all buttons.
        for (int i = 0; i < scroll_list.getContentCount(); i++)
        {
            scroll_list.getContent(i)->setSize(sf::Vector2f(p_size.x - scroll_list.scroll_bar.getSize().x, scroll_list.getContent(i)->getSize().y));
        }
    }

    void DropDownList::updateEvents(sf::Event& p_event)
    {
        if(p_event.type == sf::Event::MouseButtonReleased)
        {
            if(p_event.mouseButton.button == sf::Mouse::Left)
            {
                if(!scroll_list.isMouseOver() && !dropdown_button.isMouseOver())
                {
                    if(scroll_list.isVisible())
                        f_onDeselect();
                    scroll_list.setVisible(false);
                }
            }
        }
    }

    void DropDownList::setOnSelectEvent(const std::function<void()>& p_event)
    {
        f_onSelect = p_event;
    }

    void DropDownList::setOnDeselectEvent(const std::function<void()>& p_event)
    {
        f_onDeselect = p_event;
    }

    void DropDownList::setString(const sf::String& p_string)
    {
        dropdown_button.text.setString(p_string);
        placeholder_text.setVisible(p_string.getSize() == 0);
    }

    sf::String DropDownList::getString()
    {
        return dropdown_button.text.getString();
    }

    void DropDownList::setPlaceholderText(const sf::String& p_string)
    {
        placeholder_text.setString(p_string);
    }

    void DropDownList::setList(const std::vector<sf::String>& p_elements)
    {
        // Remove old buttons.
        for (int i = scroll_list.getContentCount() - 1; i >= 0; i--)
        {
            Widget* current_widget = scroll_list.getContent(i); 
            scroll_list.removeContent(current_widget);
            delete current_widget;
        }
        
        // Add new buttons.
        for (int i = 0; i < p_elements.size(); i++)
        {
            Button* button = new Button(canvas);
            button->setSize(sf::Vector2f(dropdown_button.getSize().x - scroll_list.scroll_bar.getSize().x, 20.f));
            button->text.setString(p_elements[i]);
            button->setStandardColor(sf::Color(255, 255, 255));
            button->setTextMargin(5.f, 0.f, 0.f, 0.f);
            button->background.setOutlineThickness(0.f);

            // The button click listener gets a copy of a pointer to this DropDownList, as well as a pointer to itself.
            button->setOnClickEvent([this, button]()
            {
                scroll_list.setVisible(false);
                setString(button->text.getString());
                f_onDeselect();
            });
 
            scroll_list.addContent(button);
            scroll_list.breakLine();
        }
        scroll_list.setVisible(false);
    }

    void DropDownList::setListLength(float p_length)
    {
        scroll_list.setSize(sf::Vector2f(dropdown_button.getSize().x, p_length));
    }

    void DropDownList::setLayer(int p_layer)
    {
        Widget::setLayer(p_layer);
        dropdown_button.setLayer(p_layer);
        scroll_list.setLayer(1);
        icon.setLayer(p_layer);
    }

    void DropDownList::setVisible(bool p_state)
    {
        Widget::setVisible(p_state);
        if(p_state)
        {
            scroll_list.setVisible(false);
        }
    }

    InputField::InputField(Canvas& p_canvas)
        : Widget::Widget(p_canvas), background(p_canvas), text(p_canvas), placeholder_text(p_canvas), cursor(p_canvas)
    {
        addChild(&text);
        addChild(&placeholder_text);
        addChild(&cursor);
        addChild(&background);

        text.setTextColor(sf::Color::Black);
        text.setVerticalAlignment(TextField::Alignment::Middle);
        text.setString("");
        text.enableWrapping(false);
        text.setMask(this);
        placeholder_text.setTextColor(placeholder_text_color);
        placeholder_text.setVerticalAlignment(TextField::Alignment::Middle);
        placeholder_text.setString("Type here...");
        placeholder_text.enableWrapping(true);
        placeholder_text.setVisible(false);
        placeholder_text.setPosition(text_padding, 0.f);
        cursor.setFillColor(sf::Color::Black);
        cursor.setOutlineThickness(0.f);
        cursor.setMask(this);
        cursor.setVisible(false);

        setSize(sf::Vector2f(300.f, 20.f));
        setCharacterSize(12);
        refreshCursor();
 
        f_customIsCharValid = [](sf::Uint32 p_char){ return true; };
        f_onSelect = [](){};
        f_onDeselect = [](){};
        f_onTyped = [](){};
    }

    void InputField::setSize(const sf::Vector2f& p_size)
    {
        Widget::setSize(p_size);
        text.setSize(p_size);
        placeholder_text.setSize(p_size);
        background.setSize(p_size);
    }

    void InputField::setString(const sf::String& p_string)
    {
        text.setString(p_string);
        cursor_index = 0;
        drag_cursor_index = cursor_index;
        refreshCursor();
    }

    const sf::String& InputField::getString()
    {
        return text.getString();
    }

    void InputField::setPlaceholderText(const sf::String& p_string)
    {
        placeholder_text.setString(p_string);
    }

    void InputField::setCharacterSize(unsigned int p_size)
    {
        text.setCharacterSize(p_size);
        placeholder_text.setCharacterSize(p_size);
        cursor.setSize(sf::Vector2f(1.f, p_size));
    }

    void InputField::setInputType(sf::Uint32 p_type)
    {
        type = p_type;
    }

    void InputField::setCharacterLimit(int p_min, int p_max)
    {
        max_character_limit = p_max;
        min_character_limit = p_min;
    }

    void InputField::setValueLimit(float p_min, float p_max)
    {
        max_value_limit = p_max;
        min_value_limit = p_min;
    }

    bool InputField::isInputValid()
    {
        return !(limit_error || input_error || date_error);
    }

    void InputField::setValidChars(const std::function<bool(sf::Uint32 p_char)>& p_function)
    {
        f_customIsCharValid = p_function;
    }

    void InputField::setOnSelectEvent(const std::function<void()>& p_event)
    {
        f_onSelect = p_event;
    }

    void InputField::setOnDeselectEvent(const std::function<void()>& p_event)
    {
        f_onDeselect = p_event;
    }

    void InputField::setOnTypedEvent(const std::function<void()>& p_event)
    {
        f_onTyped = p_event;
    }

    void InputField::updateLogic()
    {
        if(isSelected() || areChildrenSelected())
        {
            if(isInputValid())
            {
                background.setFillColor(standard_color);
            }
            else
            {
                background.setFillColor(error_color);
            }

            // Make the text cursor blink.
            blinking_timer += Canvas::getDeltaTime();
            if(blinking_timer >= blinking_interval)
            {
                cursor.setVisible(!cursor.isVisible());
                blinking_timer = 0.f;
            }

            background.setOutlineColor(selected_color);
            background.setOutlineThickness(1.f);
        }
        else
        {
            if(isHovered() || areChildrenHovered())
            {
                background.setFillColor(highlight_color);
            }
            else
            {
                if(isInputValid())
                {
                    background.setFillColor(standard_color);
                }
                else
                {
                    background.setFillColor(error_color);
                }
            }

            cursor.setVisible(false);
            background.setOutlineColor(unselected_color);
            background.setOutlineThickness(1.f);
        }

        // Update visibility of the placeholder text.
        placeholder_text.setVisible(text.getString().getSize() == 0);

        if(is_making_mouse_selection)
        {
            sf::Vector2f mouse_position = getWorldTransform().getInverse().transformPoint(getMousePosition(window, &canvas.getView())); // Get mouse position in local space of the inputfield and not in world space.
            drag_cursor_index = text.findIndex(mouse_position); 
            moveTextHorizontally(drag_cursor_index);

            cursor.setPosition(text.findCharacterPos(cursor_index));
            text.setSelection(std::min(drag_cursor_index, cursor_index), std::max(drag_cursor_index, cursor_index));
        }
    }

    void InputField::updateEvents(sf::Event& p_event)
    {
        if(isSelected() || areChildrenSelected())
        {
            if(p_event.type == sf::Event::KeyPressed)
            {
                if(p_event.key.code == sf::Keyboard::Left)
                {
                    cursor.setVisible(true);
                    blinking_timer = 0.f;

                    if(is_making_shift_selection)
                    {
                        is_making_mouse_selection = false;
                        cursor_index--;
                        refreshCursor();
                        text.setSelection(std::min(drag_cursor_index, cursor_index), std::max(drag_cursor_index, cursor_index));
                    }
                    else if(cursor_index != drag_cursor_index) // If we are not making a shift selection, but something is selected.
                    {
                        is_making_mouse_selection = false;
                        cursor_index = std::min(cursor_index, drag_cursor_index);
                        refreshCursor();
                        drag_cursor_index = cursor_index;
                        text.setSelection(std::min(drag_cursor_index, cursor_index), std::max(drag_cursor_index, cursor_index));
                    }
                    else // If we are not making any selection.
                    {
                        cursor_index--;
                        refreshCursor();
                        drag_cursor_index = cursor_index;
                    }
                }
                if(p_event.key.code == sf::Keyboard::Right)
                {
                    cursor.setVisible(true);
                    blinking_timer = 0.f;

                    if(is_making_shift_selection)
                    {
                        is_making_mouse_selection = false;
                        cursor_index++;
                        refreshCursor();
                        text.setSelection(std::min(drag_cursor_index, cursor_index), std::max(drag_cursor_index, cursor_index));
                    }
                    else if(cursor_index != drag_cursor_index)
                    {
                        is_making_mouse_selection = false;
                        cursor_index = std::max(cursor_index, drag_cursor_index);
                        refreshCursor();
                        drag_cursor_index = cursor_index;
                        text.setSelection(std::min(drag_cursor_index, cursor_index), std::max(drag_cursor_index, cursor_index));
                    }
                    else
                    {
                        cursor_index++;
                        refreshCursor();
                        drag_cursor_index = cursor_index;
                    }
                }
                if(p_event.key.code == sf::Keyboard::LShift || p_event.key.code == sf::Keyboard::RShift)
                {
                    is_making_shift_selection = true;
                }
            }
            if(p_event.type == sf::Event::KeyReleased)
            {
                input_error = false;
                if(p_event.key.code == sf::Keyboard::LShift || p_event.key.code == sf::Keyboard::RShift)
                {
                    is_making_shift_selection = false;
                }
            }

            // Change cursor index with mouse click and mouse selection.
            if(p_event.type == sf::Event::MouseButtonPressed)
            {
                if(p_event.mouseButton.button == sf::Mouse::Left)
                {
                    cursor_index = text.findIndex(getWorldTransform().getInverse().transformPoint(getMousePosition(window, &canvas.getView())));
                    refreshCursor();
                    drag_cursor_index = cursor_index;
                    is_making_mouse_selection = true;
                }
            }
            if(p_event.type == sf::Event::MouseButtonReleased)
            {
                if(p_event.mouseButton.button == sf::Mouse::Left)
                {
                    is_making_mouse_selection = false;
                }
            }

            // Handle inputted text.
            if(p_event.type == sf::Event::TextEntered)
            {
                sf::Uint32 input = p_event.text.unicode;

                cursor.setVisible(true);
                blinking_timer = 0.f;

                // TAB (9)
                if(input == 9)
                {
                    return;
                }

                // Crtl + C (3)
                if(input == 3)
                {
                    // If the user made a selection.
                    if(drag_cursor_index != cursor_index)
                    {
                        sf::Clipboard::setString(text.getString().substring(std::min(cursor_index, drag_cursor_index), std::abs(cursor_index - drag_cursor_index)));
                    }
                    return;
                }

                // Crtl + A (1)
                if(input == 1)
                {
                    drag_cursor_index = 0;
                    cursor_index = text.getString().getSize();
                    refreshCursor();
                    text.setSelection(std::min(drag_cursor_index, cursor_index), std::max(drag_cursor_index, cursor_index));
                    return;
                }

                // On any press.
                bool made_selection = cursor_index != drag_cursor_index;
                if(made_selection)
                {
                    sf::String modified_string = text.getString();
                    modified_string.erase(std::min(cursor_index, drag_cursor_index), std::abs(cursor_index - drag_cursor_index));
                    cursor_index = std::min(cursor_index, drag_cursor_index);
                    drag_cursor_index = cursor_index;
                    text.setString(modified_string);
                    refreshCursor();
                    text.setSelection(std::min(drag_cursor_index, cursor_index), std::max(drag_cursor_index, cursor_index));
                }

                // Crtl + V (22)
                if(input == 22)
                {
                    sf::String toInsert = sf::Clipboard::getString();
                    // Check if string is not to big.
                    if(toInsert.getSize() + text.getString().getSize() > max_character_limit) 
                    {
                        input_error = true;
                        return;
                    }
                    // Check whether string contains undesired characters.
                    for (int i = 0; i < toInsert.getSize(); i++)
                    {
                        if(!isCharValid(toInsert[i])) 
                        {
                            input_error = true;
                            return;
                        }
                    }
                    if(type & Capitalized)
                    {
                        toInsert = toUppercase(toInsert);
                    }
                    sf::String modified_string = text.getString();
                    modified_string.insert(cursor_index, toInsert);
                    text.setString(modified_string);
                    cursor_index += toInsert.getSize();
                    drag_cursor_index = cursor_index;
                    refreshCursor();
                    return;
                }

                // On backspace press.
                if(input == L'\b')
                {
                    if(cursor_index > 0 && !made_selection)
                    {
                        sf::String modified_string = text.getString();
                        modified_string.erase(cursor_index - 1);
                        text.setString(modified_string);
                        cursor_index--;
                        drag_cursor_index = cursor_index;
                        refreshCursor();
                    }
                }
                // On valid character input.
                else if(isCharValid(input) && text.getString().getSize() < max_character_limit)
                {
                    sf::String modified_string = text.getString();
                    if(type & Capitalized)
                    {
                        input = toUppercase(input);
                    }
                    modified_string.insert(cursor_index, input);
                    text.setString(modified_string);
                    cursor_index++;
                    refreshCursor();
                    drag_cursor_index = cursor_index;
                }
                else
                {
                    input_error = true;
                }

                // Update character limit error.
                if(text.getString().getSize() < min_character_limit && text.getString().getSize() > 0)
                {
                    limit_error = true;
                }
                else
                {
                    limit_error = false;
                }

                refreshDateFormat();
                f_onTyped();
            }
        }

        // Change mouse cursor.
        if(p_event.type == sf::Event::MouseMoved)
        {
            bool is_currently_hovered = isHovered() || areChildrenHovered();

            if(is_currently_hovered && !was_hovered)
            {
                Cursor::setCursor(sf::Cursor::Text, window);
            }
            if(!is_currently_hovered && was_hovered)
            {
                Cursor::setCursor(sf::Cursor::Arrow, window);
            }

            was_hovered = is_currently_hovered;
        }

        // OnSelect and OnDeselect events.
        if(p_event.type == sf::Event::MouseButtonPressed)
        {
            bool is_currently_selected = isSelected() || areChildrenSelected();
            if(!was_selected && is_currently_selected)
            {
                refreshLimitedFormat();
                f_onSelect();
            }
            if(was_selected && !is_currently_selected)
            {
                refreshLimitedFormat();
                f_onDeselect();

                // Hide selection.
                drag_cursor_index = cursor_index;
                text.setSelection(std::min(drag_cursor_index, cursor_index), std::max(drag_cursor_index, cursor_index));

                // Move text back.
                moveTextHorizontally(0);
            }
            was_selected = is_currently_selected;
        }
    }

    void InputField::setMask(Widget* p_mask)
    {
        Widget::setMask(p_mask);
        cursor.setMask(this);
        text.setMask(this);
    }

    void InputField::setLayer(int p_layer)
    {
        Widget::setLayer(p_layer);
        background.setLayer(p_layer);
        text.setLayer(p_layer);
        placeholder_text.setLayer(p_layer);
        cursor.setLayer(p_layer);
    }

    void InputField::setVisible(bool p_state)
    {
        Widget::setVisible(p_state);
        background.setVisible(p_state);
        text.setVisible(p_state);
        if(!p_state)
        {
            placeholder_text.setVisible(false);
            cursor.setVisible(false);
        }
    }

    void InputField::refreshCursor()
    {
        // Out of bounds check.
        if(cursor_index < 0)
        {
            cursor_index = 0;
        }
        if(cursor_index > text.getString().getSize())
        {
            cursor_index = text.getString().getSize();
        }

        // Update text position.
        moveTextHorizontally(cursor_index);

        // Update cursor position.
        cursor.setPosition(text.findCharacterPos(cursor_index));
    }

    bool InputField::isCharValid(sf::Uint32 p_char)
    {
        if(type & Custom) return f_customIsCharValid(p_char);
        if(type == Date) return isdigit(p_char) || p_char == '.';
        if(type & Standard) return true;

        bool value = false;

        if(type & IntegerNumber)
            value |= isdigit(p_char) || p_char == '+' || p_char == '-';
        if(type & DecimalNumber)
            value |= isdigit(p_char) || p_char == ',' || p_char == '.' || p_char == '+' || p_char == '-';
        if(type & HexadecimalNumber)
            value |= isdigit(p_char) || p_char == 'a' || p_char == 'b' || p_char == 'c' || p_char == 'd' || p_char == 'e' || p_char == 'f'
            || p_char == 'A' || p_char == 'B' || p_char == 'C' || p_char == 'D' || p_char == 'E' || p_char == 'F' || p_char == 'x';
        if(type & BinaryNumber)
            value |= p_char == '0' || p_char == '1' || p_char == 'b';
        if(type & Alphanumeric)
            value |= isalnum(p_char);
        if(type & Alpha)
            value |= isalpha(p_char);

        return value;
    }

    void InputField::refreshDateFormat()
    {
        if(type == Date)
        {
            setCharacterLimit(10, 10);
            date_error = (text.getString().getSize() > 0 && text.getString().getSize() < 10);
            if(text.getString().getSize() == 10)
            {
                date_error = 
                    text.getString()[0] == '.' || 
                    text.getString()[1] == '.' || 
                    text.getString()[2] != '.' || 
                    text.getString()[3] == '.' || 
                    text.getString()[4] == '.' || 
                    text.getString()[5] != '.' || 
                    text.getString()[6] == '.' || 
                    text.getString()[7] == '.' || 
                    text.getString()[8] == '.' || 
                    text.getString()[9] == '.';
            }
        }
    }

    void InputField::refreshLimitedFormat()
    {
        if(type & LimitedValue)
        {
            if(type & IntegerNumber)
            {
                text.setString(toString(clampValue(stringToInt(text.getString()), (int)min_value_limit, (int)max_value_limit), 2));
            }
            else if(type & DecimalNumber)
            {
                text.setString(toString(clampValue(stringToFloat(text.getString()), min_value_limit, max_value_limit), 2));
            }
            else if(type & HexadecimalNumber)
            {
                text.setString(toHexString(clampValue(hexadecimalToInt(text.getString()), (unsigned int)min_value_limit, (unsigned int)max_value_limit)));
            }
            else if(type & BinaryNumber)
            {
                text.setString(toBinaryString(clampValue(binaryToInt(text.getString()), (unsigned int)min_value_limit, (unsigned int)max_value_limit)));
            }
        }
    }

    void InputField::moveTextHorizontally(int p_index)
    {
        // If the cursor goes out of bounds we move the text so it still is inside, also adding a small padding.
        sf::Vector2f index_position = text.findCharacterPos(p_index);
        if(index_position.x < 0.f)
        {
            text.move(sf::Vector2f(-index_position.x + text_padding, 0.f));
        }
        else if (index_position.x >= getSize().x)
        {
            text.move(sf::Vector2f(getSize().x - index_position.x - text_padding, 0.f));
        }

        // Makes that if the text is longer than the actual inputfield and you change its size the text will be moved to the right of the inputfield accordingly.
        sf::Vector2f first_position = text.findCharacterPos(0);
        sf::Vector2f last_position = text.findCharacterPos(text.getString().getSize());
        float text_width = last_position.x - first_position.x;
        if(text_width > getSize().x - 2.f * text_padding)
        {
            if(last_position.x < getSize().x)
            {
                text.move(sf::Vector2f(getSize().x - last_position.x - text_padding, 0.f));
            }
        }
        // If the text is shorter just fix it in place with some padding.
        else
        {
            text.setPosition(sf::Vector2f(text_padding, 0.f));
        }
    }

    InputList::InputList(Canvas& p_canvas)
        : Widget::Widget(p_canvas), inputfield(p_canvas), scroll_list(p_canvas)
    {
        addChild(&inputfield);
        addChild(&scroll_list);

        scroll_list.setVisible(false);

        inputfield.setOnSelectEvent([&]()
        {
            if(!scroll_list.isVisible())
                f_onSelect();
            scroll_list.setVisible(true);
        });

        f_onSelect = [](){};
        f_onDeselect = [](){};

        setSize(sf::Vector2f(300.f, 20.f));
    }

    void InputList::setSize(const sf::Vector2f& p_size)
    {
        Widget::setSize(p_size);
        inputfield.setSize(p_size);
        scroll_list.setPosition(0.f, inputfield.getSize().y);
        scroll_list.setSize(sf::Vector2f(p_size.x, 100.f));

        // Resize all buttons.
        for (int i = 0; i < scroll_list.getContentCount(); i++)
        {
            scroll_list.getContent(i)->setSize(sf::Vector2f(p_size.x - scroll_list.scroll_bar.getSize().x, scroll_list.getContent(i)->getSize().y));
        }
        scroll_list.refresh();
    }

    void InputList::updateEvents(sf::Event& p_event)
    {
        if(p_event.type == sf::Event::MouseButtonReleased)
        {
            if(p_event.mouseButton.button == sf::Mouse::Left)
            {
                if(!scroll_list.isMouseOver() && !inputfield.isMouseOver())
                {
                    if(scroll_list.isVisible())
                        f_onDeselect();
                    scroll_list.setVisible(false);
                }
            }
        }
    }

    void InputList::setOnSelectEvent(const std::function<void()>& p_event)
    {
        f_onSelect = p_event;
    }

    void InputList::setOnDeselectEvent(const std::function<void()>& p_event)
    {
        f_onDeselect = p_event;
    }

    void InputList::setString(const sf::String& p_string)
    {
        inputfield.setString(p_string);
    }

    sf::String InputList::getString()
    {
        return inputfield.getString();
    }

    void InputList::setList(const std::vector<sf::String>& p_elements)
    {
        // Remove old buttons.
        for (int i = 0; i < scroll_list.getContentCount(); i++)
        {
            Widget* current_widget = scroll_list.getContent(i); 
            scroll_list.removeContent(current_widget);
            delete current_widget;
        }
        
        // Add new buttons.
        for (int i = 0; i < p_elements.size(); i++)
        {
            Button* button = new Button(canvas);
            button->setSize(sf::Vector2f(inputfield.getSize().x - scroll_list.scroll_bar.getSize().x, 20.f));
            button->text.setString(p_elements[i]);
            button->setStandardColor(sf::Color(255, 255, 255));
            button->setTextMargin(5.f, 0.f, 0.f, 0.f);
            button->background.setOutlineThickness(0.f);

            // The button click listener gets a copy of a pointer to this DropDownList, as well as a pointer to itself.
            button->setOnClickEvent([this, button]()
            {
                scroll_list.setVisible(false);
                inputfield.setString(button->text.getString());
                f_onDeselect();
            });
 
            scroll_list.addContent(button);
        }
        scroll_list.refresh();
        scroll_list.setVisible(false);
    }

    void InputList::setListLength(float p_length)
    {
        scroll_list.setSize(sf::Vector2f(inputfield.getSize().x, p_length));
    }

    void InputList::setMask(Widget* p_widget)
    {
        Widget::setMask(p_widget);
        inputfield.setMask(p_widget);
        scroll_list.setMask(p_widget);
    }

    void InputList::setLayer(int p_layer)
    {
        Widget::setLayer(p_layer);
        inputfield.setLayer(p_layer);
        scroll_list.setLayer(p_layer);
    }

    void InputList::setVisible(bool p_state)
    {
        Widget::setVisible(p_state);
        inputfield.setVisible(p_state);
        if(!p_state)
            scroll_list.setVisible(p_state);
    }

    void InputList::setUpdated(bool p_state)
    {
        Widget::setUpdated(p_state);
        inputfield.setUpdated(p_state);
        scroll_list.setUpdated(p_state);
    }

    void InputList::setSelectable(bool p_state)
    {
        Widget::setSelectable(p_state);
        inputfield.setSelectable(p_state);
        scroll_list.setSelectable(p_state);
    }

    float Canvas::s_delta_time;

    Canvas::Canvas(sf::RenderWindow& p_window, sf::Font& p_font)
        : window(p_window), font(p_font)
    {
        // Initialize canvas view.
        view.reset(sf::FloatRect(0.f, 0.f, window.getSize().x, window.getSize().y));
        view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));

        // Load clipping shader.
        if(!sf::Shader::isAvailable())
        {
            std::cout << "Shaders are not available on this system!" << std::endl;
        }
        else
        {
            // It seems like there is a maximum of uniforms (GL_MAX_FRAGMENT_UNIFORM_COMPONENTS or GL_MAX_UNIFORM_LOCATIONS) 
            // that can be passed to a shader which is dependent on the driver/graphics card/manufacturer. An array size
            // of 32 does not seem to exceed this limit. TODO: Find an optimal value.
            sf::String shader_code = 
            "uniform int array_size;"
            "uniform vec2 sizes[32];" 
            "uniform mat4 inverse_transforms[32];"
            "uniform vec2 window_size;"
            "uniform sampler2D texture;"
            "uniform bool use_texture;"
            "void main()"
            "{"
            "    vec4 position = vec4(gl_FragCoord.x, window_size.y - gl_FragCoord.y, 0.0, 1.0);"
            "    vec4 pixel = gl_Color;"
            "    if(use_texture)"
            "    {"
            "        pixel *= texture2D(texture, gl_TexCoord[0].xy);"
            "    }"
            "    bool draw = true;"
            "    for (int i = 0; i < array_size; i++)"
            "    {"
            "        vec4 local_position = inverse_transforms[i] * position;"
            "        if (local_position.x < 0.0 || local_position.y < 0.0 || local_position.x > sizes[i].x || local_position.y > sizes[i].y) draw = false;"
            "    }"
            "    if(draw)"
            "    {"
            "        gl_FragColor = pixel;"
            "    }"
            "    else"
            "    {"
            "        gl_FragColor = vec4(pixel.xyz, 0.0);"
            "    }"
            "}";
            clipping_shader.loadFromMemory(shader_code, sf::Shader::Fragment);
            //clipping_shader.loadFromFile("clipping_shader.glsl", sf::Shader::Fragment);
        }
    }
    
    void Canvas::drawAll() 
    {
        // Save the current view as copy and assign the canvas view.
        sf::View currentView = window.getView();
        window.setView(view);

        // Draw all visible widgets.
        for (int i = 0; i < widget_list.size(); i++)
        {
            if(widget_list[i]->isVisible())
            {
                sf::RenderStates states;
                // Apply shader.
                states.shader = &clipping_shader;

                // Apply widget transform.
                states.transform *= widget_list[i]->getWorldTransform();

                window.draw(*widget_list[i], states);
            }
        }

        window.setView(currentView);
    }

    void Canvas::updateAllLogic() 
    {
        // Update all widgets.
        for (int i = 0; i < widget_list.size(); i++)
        {
            if(widget_list[i]->isUpdated() && widget_list[i]->isVisible())
            {
                widget_list[i]->updateLogic();
            }
        }

     	// Set hovered widget.
        std::vector<int> widgets_under_mouse;
        for (int i = 0; i < widget_list.size(); i++)
        {
            if(widget_list[i]->isMouseOver() && widget_list[i]->isSelectable() && widget_list[i]->isVisible())
            {
                widgets_under_mouse.push_back(i);
            }
        }
        hovered_widget = widgets_under_mouse.size() ? widget_list[widgets_under_mouse[widgets_under_mouse.size() - 1]] : nullptr;

		// DeltaTime.
        Canvas::s_delta_time = clock.restart().asSeconds();
    }
    
    void Canvas::updateAllEvents(sf::Event& p_event) 
    {
        if(p_event.type == sf::Event::MouseButtonPressed)
        {
            if(p_event.mouseButton.button == sf::Mouse::Left)
            {     
                // Set selected widget.
                std::vector<int> widgets_under_mouse;
                for (int i = 0; i < widget_list.size(); i++)
                {
                    if(widget_list[i]->isMouseOver() && widget_list[i]->isSelectable() && widget_list[i]->isVisible())
                    {
                        widgets_under_mouse.push_back(i);
                    }
                }
                if(widgets_under_mouse.size() == 0)
                {
                    select(nullptr);
                }
                else
                {
                    int selected_widget_index = widgets_under_mouse[widgets_under_mouse.size() - 1];
                    select(widget_list[selected_widget_index]);
                }
            }
        }
        if(p_event.type == sf::Event::Resized)
        {
            view.reset(sf::FloatRect(0.f, 0.f, window.getSize().x, window.getSize().y));
            view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
        }
        if(p_event.type == sf::Event::MouseWheelMoved)
        {
            if(p_event.mouseWheel.delta > 0.f)
            {
                control_index++;
            }
            else if(p_event.mouseWheel.delta < 0.f)
            {
                control_index--;
            }
            if(control_index < 0) control_index = 0;
            if(control_index >= widget_list.size()) control_index = widget_list.size() - 1;
        }
        if(p_event.type == sf::Event::KeyPressed)
        {
            if(p_event.key.code == sf::Keyboard::V)
            {
                widget_list[control_index]->setVisible(!widget_list[control_index]->isVisible());
            }
        }

        // Update widget events.
        for (int i = 0; i < widget_list.size(); i++)
        {
            if(widget_list[i]->isUpdated() && widget_list[i]->isVisible())
            {
                widget_list[i]->updateEvents(p_event);
            }
        }
    }
    
    void Canvas::add(Widget* p_widget) 
    {
        // Check if widget alreay exists.
        if(find(p_widget) != -1) return;

        // Add widget.
        widget_list.push_back(p_widget);

        moveToLayer(p_widget, p_widget->getLayer());
    }

    int Canvas::find(Widget* p_widget) const
    {
        for (int i = 0; i < widget_list.size(); i++)
        {
            if(widget_list[i] == p_widget)
            {
                return i;
            }
        }
        return -1;
    }

    void Canvas::remove(Widget* p_widget) 
    {
        for (int i = 0; i < widget_list.size(); i++)
        {
            if(widget_list[i] == p_widget)
            {
                widget_list.erase(widget_list.begin() + i);
                return;
            }
        }
    }
    
    void Canvas::moveToLayer(Widget* p_widget, int p_layer)
    {
        // First remove the given widget.
        remove(p_widget);
        // Insert the widget behind the last widget that has the same layer as the given one.
        for (int i = 0; i < widget_list.size(); i++)
        {
            if(p_layer < widget_list[i]->getLayer())
            {
                widget_list.insert(widget_list.begin() + i, p_widget);
                return;
            }
        }
        // If the layer is too high/not present, just insert at the end.
        widget_list.insert(widget_list.begin() + widget_list.size(), p_widget);
    }

    void Canvas::moveInFront(Widget* p_widget)
    {
        p_widget->setLayer(widget_list.size() - 1);
    }

    Widget* Canvas::getSelected() 
    {
        return selected_widget;
    }

    Widget* Canvas::getHovered()
    {
        return hovered_widget;
    }
    
    const sf::View& Canvas::getView() const
    {
        return view;
    }
    
    sf::Shader& Canvas::getClippingShader()
    {
        return clipping_shader;
    }

    void Canvas::select(Widget* p_widget)
    {
        selected_widget = p_widget;
    }

    sf::String Canvas::print()
    {
        sf::String value = "Widgets in drawing order: \n\n";;

        for (int i = 0; i < widget_list.size(); i++)
        {
            Widget* current_widget = widget_list[i];
            
            sf::String name = "Widget";
            Button* button = dynamic_cast<Button*>(current_widget);
            if(button != nullptr) name = "Button";
            TextField* text = dynamic_cast<TextField*>(current_widget);
            if(text != nullptr) name = "TextField";
            Graphic* graphic = dynamic_cast<Graphic*>(current_widget);
            if(graphic != nullptr) name = "Graphic";
            ScrollBar* scrollbar = dynamic_cast<ScrollBar*>(current_widget);
            if(scrollbar != nullptr) name = "ScrollBar";
            ScrollList* scrolllist = dynamic_cast<ScrollList*>(current_widget);
            if(scrolllist != nullptr) name = "ScrollList";
            DropDownList* dropdownlist = dynamic_cast<DropDownList*>(current_widget);
            if(dropdownlist != nullptr) name = "DropDownList";
            DockNode* docknode = dynamic_cast<DockNode*>(current_widget);
            if(docknode != nullptr) name = "DockNode";
            DockSpace* dockspace = dynamic_cast<DockSpace*>(current_widget);
            if(dockspace != nullptr) name = "DockSpace";
            Panel* panel = dynamic_cast<Panel*>(current_widget);
            if(panel != nullptr) name = "Panel";

            if(current_widget->isHovered()) name.insert(0, "=>");
            if(i == control_index) name.insert(0, "(x)");

            value += toString(i) + ". " + name + ": layer=" + toString(current_widget->getLayer()) + " visible=" + (current_widget->isVisible() ? "True" : "False") + "\n";
        }
        return value;
    }

    float Canvas::getDeltaTime()
    {
        return s_delta_time;
    }

    void Canvas::setIcons(const sf::Texture* p_scroll_bar_up, const sf::Texture* p_scroll_bar_down, const sf::Texture* p_dropdown_arrow)
    {
        scroll_bar_up_texture = p_scroll_bar_up;
        scroll_bar_down_texture = p_scroll_bar_down;
        dropdown_arrow_texture = p_dropdown_arrow;
    }

    Cursor::CursorData::CursorData()
    {
        for (int i = 0; i < 13; i++)
        {
            if(!system_cursors[i].loadFromSystem((sf::Cursor::Type)i))
            {
                std::cout << "Failed to load system cursor (" << i << ")!" << std::endl;
            }
        }
    }

    void Cursor::CursorData::setCursor(sf::Cursor::Type p_type, sf::RenderWindow& p_window)
    {
        p_window.setMouseCursor(system_cursors[(int)p_type]);
    }

    Cursor::CursorData Cursor::s_data;

    void Cursor::setCursor(sf::Cursor::Type p_type, sf::RenderWindow& p_window)
    {
        s_data.setCursor(p_type, p_window);
    }

    sf::Vector2f getMousePosition(sf::RenderWindow& window, const sf::View* view) 
    {
        sf::Vector2f mouse_position;
        if(view == nullptr)
        {
            // "mapPixelToCoords" is used to retrieve the correct coordinates in world space even if the view is distorted.
            mouse_position = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        }
        else
        {
            mouse_position = window.mapPixelToCoords(sf::Mouse::getPosition(window), *view);
        }
        return mouse_position;
    }

    sf::Vector2f getMousePosition(sf::RenderWindow& window, const sf::Vector2f& relative_to, const sf::View* view) 
    {
        sf::Vector2f mouse_position;
        if(view == nullptr)
        {
            // "mapPixelToCoords" is used to retrieve the correct coordinates in world space even if the view is distorted.
            mouse_position = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        }
        else
        {
            mouse_position = window.mapPixelToCoords(sf::Mouse::getPosition(window), *view);
        }
        mouse_position -= relative_to;
        return mouse_position;
    }

    bool mouseOverCircle(const sf::Vector2f& center, float radius, sf::RenderWindow& window, sf::View* view) 
    {
        sf::Vector2f mousePosition = getMousePosition(window, view);
        sf::Vector2f distanceVector = mousePosition - sf::Vector2f(center.x + radius, center.y + radius);
        float distance = std::sqrt(distanceVector.x * distanceVector.x + distanceVector.y * distanceVector.y);

        return distance < radius;
    }

    std::vector<sf::String> splitStringByChar(const sf::String& str, const wchar_t& c)
    {
        std::wstringstream strstream;
        strstream << str.toWideString();
        std::vector<sf::String> segmentList;
        std::wstring segment;

        while (std::getline(strstream, segment, c))
        {
            segmentList.push_back(segment);
        }

        return segmentList;
    }

    float stringToFloat(const sf::String& p_string)
    {
        if(p_string.getSize() == 0) return 0.f;
        if(p_string.toAnsiString().find_first_not_of("+-0123456789.,") != std::string::npos) return 0.f;

        // Replace dots with comma.
        sf::String replaced_string(p_string);
        replaced_string.replace(".", ",");
        bool negative = replaced_string.find("-") != sf::String::InvalidPos;
        replaced_string.replace("-", "");
        replaced_string.replace("+", "");

        // Split number into two parts.
        std::vector<sf::String> number_parts = splitStringByChar(replaced_string, ',');
        float sum = 0.f;
        float factor = std::pow(10.f, number_parts[0].getSize() - 1.f);
        float value = 0.f;
        if(number_parts.size() < 1) return 0.f;
        // Add numbers before the comma to sum.
        for (int i = 0; i < number_parts[0].getSize(); i++)
        {
            value = number_parts[0][i] - 0x30;
            sum += factor * value;
            factor /= 10.f;
        }
        // Add numbers after the comma to sum.
        if(number_parts.size() >= 2)
        {
            factor = 0.1f;
            for (int i = 0; i < number_parts[1].getSize(); i++)
            {
                value = number_parts[1][i] - 0x30;
                sum += factor * value;
                factor /= 10.f;
            }
        }

        if(negative) return -sum;
        return sum;
    }

    int stringToInt(const sf::String& p_string)
    {
        if(p_string.getSize() == 0) return 0.f;
        if(p_string.toAnsiString().find_first_not_of("+-0123456789.,") != std::string::npos) return 0.f;

        // Replace dots with comma.
        sf::String replaced_string(p_string);
        replaced_string.replace(".", ",");
        bool negative = replaced_string.find("-") != sf::String::InvalidPos;
        replaced_string.replace("-", "");
        replaced_string.replace("+", "");

        // Discard everthing after a comma.
        std::vector<sf::String> number_parts = splitStringByChar(replaced_string, ',');
        if(number_parts.size() < 1) return 0.f;
        sf::String splitted_string = number_parts[0];

        // Split number into two parts.
        float sum = 0.f;
        float factor = std::pow(10.f, splitted_string.getSize() - 1.f);
        float value = 0.f;
        // Add numbers before the comma to sum.
        for (int i = 0; i < splitted_string.getSize(); i++)
        {
            value = splitted_string[i] - 0x30;
            sum += factor * value;
            factor /= 10.f;
        }
        
        if(negative) return -sum;
        return sum;
    }

    sf::String toUppercase(const sf::String& p_string)
    {
        sf::String uppercase_string;
        for (int i = 0; i < p_string.getSize(); i++)
        {
            // Add transformed char to new string.
            uppercase_string += toUppercase(p_string[i]);
        }
        return uppercase_string;
    }
    sf::Uint32 toUppercase(sf::Uint32 p_char)
    {
        // Find conversion to uppercase.
        if((p_char >= 0x61 && p_char <= 0x7a) || // ASCII letters.
            (p_char >= 0xe0 && p_char <= 0xfe && p_char != 0xf7)) // Extendend ANSI letters.
        {
            p_char -= 0x20;
        }

        return p_char;
    }

    unsigned int hexadecimalToInt(sf::String p_string)
    {
        // Check if p_string is valid.
        p_string.replace(" ", "");
        int found = p_string.find("0x");
        if(found != sf::String::InvalidPos) p_string.erase(found, 2);
        p_string = toUppercase(p_string);
        if(p_string.toAnsiString().find_first_not_of("0123456789ABCDEF") != std::string::npos) return 0;        

        // Calculate integer value.
        unsigned int sum = 0;
        unsigned int factor = std::pow(16, p_string.getSize() - 1);
        for (int i = 0; i < p_string.getSize(); i++)
        {
            unsigned int value = 0;
            if(p_string[i] >= 0x30 && p_string[i] <= 0x39) value = p_string[i] - 0x30;
            else if(p_string[i] >= 0x41 && p_string[i] <= 0x46) value = p_string[i] - 0x37;
            sum += factor * value;
            factor /= 16;
        }
        return sum;
    }

    unsigned int binaryToInt(sf::String p_string)
    {
        // Check if p_string is valid.
        p_string.replace(" ", "");
        int found = p_string.find("0b"); 
        if(found != sf::String::InvalidPos) p_string.erase(found, 2);
        if(p_string.toAnsiString().find_first_not_of("01") != std::string::npos) return 0;

        // Calculate integer value.
        unsigned int sum = 0;
        unsigned int factor = std::pow(2, p_string.getSize() - 1);
        for (int i = 0; i < p_string.getSize(); i++)
        {
            sum += p_string[i] == '1' ? factor : 0;
            factor /= 2;
        }
        
        return sum;
    }

    sf::String toBinaryString(int p_value, int p_max_digits, sf::String p_base_string)
    {
        sf::String characters = "01";
        sf::String result;
        if(p_max_digits > sizeof(int) * 8) 
        {
            p_max_digits = sizeof(int) * 8;
            std::cout << "Integer has a maximum of " << sizeof(int) << " bytes." << std::endl;
        }
        for (int i = p_max_digits - 1; i >= 0; i--)
        {
            result += characters[(p_value >> i) & 1];
        }
        result.insert(0, p_base_string);
        return result;
    }

    sf::String toHexString(int p_value, bool p_uppercase, int p_max_digits, sf::String p_base_string)
    {
        sf::String characters = p_uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
        sf::String result;
        if(p_max_digits > sizeof(int) * 2) 
        {
            p_max_digits = sizeof(int) * 2;
            std::cout << "Integer has a maximum of " << sizeof(int) << " bytes." << std::endl;
        }
        for (int i = p_max_digits * 4 - 4; i >= 0; i -= 4)
        {
            result += characters[(p_value >> i) & 0x0f];
        }
        result.insert(0, p_base_string);
        return result;
    }
}
