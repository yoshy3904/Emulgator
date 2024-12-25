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
        // Check if widget was already added as child.
        for (int i = 0; i < children.size(); i++)
        {
            if(p_widget == children[i])
            {
                return;
            }
        }
        // Add child.
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

    TextField::TextField(Canvas& p_canvas, sf::Font& p_font) 
        : Widget(p_canvas), 
        string(L"New Text"), 
        font(p_font),
        character_size(12),
        line_spacing_factor(1.f),
        paragraph_spacing_factor(1.25f),
        text_color(sf::Color::White),
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
        // applyDebugGeometry();
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
        for (int i = 0; i < text_info.size(); i++)
        {
            if(text_info[i].size() == 0) continue;

            debug_vertices.append(sf::Vertex(sf::Vector2f(
                vertices[text_info[i][0].start_vertex].position.x, 
                text_info[i][0].baseline), sf::Color::Red));

            debug_vertices.append(sf::Vertex(sf::Vector2f(
                vertices[text_info[i][text_info[i].size() - 1].start_vertex].position.x, 
                text_info[i][text_info[i].size() - 1].baseline), sf::Color::Red));
        }

    }

    Button::Button(Canvas& p_canvas, sf::Font& p_font)
        : Widget::Widget(p_canvas), 
        background(p_canvas), 
        text(p_canvas, p_font), 
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

    ScrollBar::ScrollBar(Canvas& p_canvas, sf::Font& p_font) 
    	: Widget::Widget(p_canvas), 
	    background(p_canvas),
	    move_up_button(p_canvas, p_font), 
	    move_down_button(p_canvas, p_font), 
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

    void ScrollBar::setIcons(const sf::Texture* p_up, const sf::Texture* p_down)
    {
        move_up_button.background.setTexture(p_up);
        move_down_button.background.setTexture(p_down);
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

    Layout::Layout(Canvas& p_canvas) 
        : Widget::Widget(p_canvas) 
    {

    }

    sf::Vector2f Layout::getGroupSize() const
    {
        if(getChildCount() < 1) return sf::Vector2f();
        sf::Vector2f smallest_position = getChild(0)->getPosition();
        sf::Vector2f biggest_position = getChild(0)->getPosition() + getChild(0)->getSize();
        for (int i = 0; i < getChildCount(); i++)
        {
            if(getChild(i)->getPosition().x < smallest_position.x || getChild(i)->getPosition().y < smallest_position.y)
            {
                smallest_position = getChild(i)->getPosition();
            }
            sf::Vector2f current_biggest_position = getChild(i)->getPosition() + getChild(i)->getSize();
            if(current_biggest_position.x > biggest_position.x || current_biggest_position.y > biggest_position.y)
            {
                biggest_position = current_biggest_position;
            }
        }
        return biggest_position - smallest_position;
    }

    void Layout::list(Orientation orientation, float p_spacing)
    {
        float current_height;
        if(orientation == Vertical)
        {
            current_height = 0.f;
            for (int i = 0; i < getChildCount(); i++)
            {
                getChild(i)->setPosition(sf::Vector2f(0.f, current_height));
                current_height += getChild(i)->getSize().y + p_spacing;
            }
        }
        else if (orientation == Horizontal)
        {
            current_height = 0.f;
            for (int i = 0; i < getChildCount(); i++)
            {
                getChild(i)->setPosition(sf::Vector2f(current_height, 0.f));
                current_height += getChild(i)->getSize().x + p_spacing;
            }
        }
    }

    ScrollList::ScrollList(Canvas& p_canvas, sf::Font& p_font)
        : Widget::Widget(p_canvas), content(p_canvas), background(p_canvas), scroll_bar(p_canvas, p_font)
    {
        addChild(&content);
        addChild(&background);
        addChild(&scroll_bar);

        setSize(sf::Vector2f(200.f, 300.f));

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
                starting_position = (scroll_bar.getScrollState().scroll_handle_position / scroll_bar.getScrollState().scroll_list_length) * content.getGroupSize().y;
            } 
            content.setPosition(sf::Vector2f(0.f, -starting_position));
        });
    }

    void ScrollList::setSize(const sf::Vector2f& p_size)
    {
        Widget::setSize(p_size);
        scroll_bar.setSize(sf::Vector2f(scroll_bar.getSize().x, getSize().y));
        scroll_bar.setPosition(sf::Vector2f(getSize().x - scroll_bar.getSize().x, 0.f));
        background.setSize(p_size);
        content.setSize(p_size);
    }

    void ScrollList::setMask(Widget* p_widget)
    {
        Widget::setMask(p_widget);
        content.setMask(this);
        scroll_bar.setMask(this);
    }

    void ScrollList::setLayer(int p_layer)
    {
        Widget::setLayer(p_layer);
        background.setLayer(p_layer);
        content.setLayer(p_layer);
        scroll_bar.setLayer(p_layer);
    }

    void ScrollList::updateEvents(sf::Event& p_event) 
    {
        if(p_event.type == sf::Event::MouseWheelScrolled)
        {
            /* ScrollList should only be scrollable if it or its content is hovered. An exception are ScrollLists inside ScrollLists. 
            When hovering over the child, the parent ScrollList should not be scrollable. dynamic_cast is used to check whether the underlying widget is in fact a ScrollList. 
            If the child scrolllist is hovered or any of its children you can no longer scroll the parent scrolllist. */
            for (int i = 0; i < content.getChildCount(); i++)
            {
                ScrollList* scroll_list = dynamic_cast<ScrollList*>(content.getChild(i)); 
                if(scroll_list != nullptr && (scroll_list->isHovered() || scroll_list->areChildrenHovered()))
                {
                    return;
                }
            }

            // Scroll if anything else is hovered.
            if(isHovered() || areChildrenHovered())
            {
                scroll_bar.setScrollState(scroll_bar.getScrollState().scroll_handle_position - (10.f * p_event.mouseWheelScroll.delta), getSize().y, content.getGroupSize().y);
            }
        }
    }

    // Refresh the scrolllist after adding/removing content. Updates layers and masks. Lists all content vertically.
    void ScrollList::refresh()
    {
        setLayer(getLayer());
        content.setMask(this);
        content.list(ui::Vertical, 0.f);

        // Refresh scrollbar.
        scroll_bar.setScrollState(scroll_bar.getScrollState().scroll_handle_position, getSize().y, content.getGroupSize().y);
    }

    DropDownList::DropDownList(Canvas& p_canvas, sf::Font& p_font)
        : font(p_font), Widget::Widget(p_canvas), dropdown_button(p_canvas, p_font), placeholder_text(p_canvas, p_font), scroll_list(p_canvas, p_font), icon(p_canvas)
    {
        addChild(&dropdown_button);
        addChild(&placeholder_text);
        addChild(&scroll_list);
        addChild(&icon);

        scroll_list.setVisible(false);

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

        f_onSelect = [](){};
        f_onDeselect = [](){};

        setSize(sf::Vector2f(300.f, 20.f));
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
        for (int i = 0; i < scroll_list.content.getChildCount(); i++)
        {
            scroll_list.content.getChild(i)->setSize(sf::Vector2f(p_size.x - scroll_list.scroll_bar.getSize().x, scroll_list.content.getChild(i)->getSize().y));
        }
        scroll_list.refresh();
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
        for (int i = scroll_list.content.getChildCount() - 1; i >= 0; i--)
        {
            Widget* current_widget = scroll_list.content.getChild(i); 
            scroll_list.content.removeChild(current_widget);
            delete current_widget;
        }
        
        // Add new buttons.
        for (int i = 0; i < p_elements.size(); i++)
        {
            Button* button = new Button(canvas, font);
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
 
            scroll_list.content.addChild(button);
        }
        scroll_list.refresh();
        scroll_list.setVisible(false);
    }

    void DropDownList::setListLength(float p_length)
    {
        scroll_list.setSize(sf::Vector2f(dropdown_button.getSize().x, p_length));
    }

    void DropDownList::setIcon(const sf::Texture* p_dropdown)
    {
        icon.setTexture(p_dropdown);
    }

    void DropDownList::setLayer(int p_layer)
    {
        Widget::setLayer(p_layer);
        dropdown_button.setLayer(p_layer);
        scroll_list.setLayer(p_layer);
        icon.setLayer(p_layer);
    }

    void DropDownList::setVisible(bool p_state)
    {
        Widget::setVisible(p_state);
        dropdown_button.setVisible(p_state);
        icon.setVisible(p_state);
        if(p_state == false)
        {
            scroll_list.setVisible(p_state);
        }
        if(p_state == false || getString().getSize() == 0)
        {
            placeholder_text.setVisible(p_state);
        }
    }

    InputField::InputField(Canvas& p_canvas, sf::Font& p_font)
        : Widget::Widget(p_canvas), background(p_canvas), text(p_canvas, p_font), placeholder_text(p_canvas, p_font), cursor(p_canvas)
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

    InputList::InputList(Canvas& p_canvas, sf::Font& p_font)
        : font(p_font), Widget::Widget(p_canvas), inputfield(p_canvas, p_font), scroll_list(p_canvas, p_font)
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
        for (int i = 0; i < scroll_list.content.getChildCount(); i++)
        {
            scroll_list.content.getChild(i)->setSize(sf::Vector2f(p_size.x - scroll_list.scroll_bar.getSize().x, scroll_list.content.getChild(i)->getSize().y));
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
        for (int i = 0; i < scroll_list.content.getChildCount(); i++)
        {
            Widget* current_widget = scroll_list.content.getChild(i); 
            scroll_list.content.removeChild(current_widget);
            delete current_widget;
        }
        
        // Add new buttons.
        for (int i = 0; i < p_elements.size(); i++)
        {
            Button* button = new Button(canvas, font);
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
 
            scroll_list.content.addChild(button);
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

    ContextMenu::ContextMenu(Canvas& p_canvas, sf::Font& p_font)
        : Widget::Widget(p_canvas), background(p_canvas), font(p_font), layout(canvas)
    {
        addChild(&background);
        addChild(&layout);
    }

    ContextMenu::~ContextMenu()
    {
        for (int i = 0; i < options.size(); i++)
        {
            delete options[i];   
            std::cout << "Deleted Context Button" << std::endl;
        }
        options.clear();
    }

    void ContextMenu::setSize(const sf::Vector2f& p_size)
    {
        Widget::setSize(p_size);
        background.setSize(p_size);

        for (int i = 0; i < options.size(); i++)
        {
            options[i]->button.setSize(sf::Vector2f(p_size.x, 25.f));
        }
        
        layout.list(ui::Vertical, 0.f);
    }

    void ContextMenu::addOption(const sf::String& p_name, const std::function<void()>& p_event, ContextMenu* p_context_menu)
    {
        ContextOption* context_option = new ContextOption({ Button(canvas, font), p_context_menu });
        std::cout << "Created Context Button" << std::endl;
        context_option->button.text.setString(p_name);
        context_option->button.setTextMargin(5.f, 0.f, 0.f, 0.f);
        context_option->button.background.setOutlineThickness(0.f);
        context_option->button.setOnClickEvent(p_event);
        layout.addChild(&context_option->button);
        options.push_back(context_option);
    }

    float Canvas::s_delta_time;

    Canvas::Canvas(sf::RenderWindow& p_window, sf::Font& p_font)
        : window(p_window), debug_text("", p_font)
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

        // Init debug text.
        debug_text.setOutlineColor(sf::Color::Black);
        debug_text.setOutlineThickness(3.f);
        debug_text.setCharacterSize(15);
    }
    
    void Canvas::drawAll() 
    {
        // Save the current view as copy and assign new one.
        sf::View currentView = window.getView();

        window.setView(view);

        // Draw all visible widgets.
        for (int i = 0; i < widget_list.size(); i++)
        {
            if(widget_list[i]->isVisible())
            {
                sf::RenderStates states;
                states.shader = &clipping_shader;
                states.transform *= widget_list[i]->getWorldTransform();
                window.draw(*widget_list[i], states);
            }
        }

        // Draw here.
        // window.draw(debug_text);

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

		// Debugging.
        updateDebugText();
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

    float Canvas::getDeltaTime()
    {
        return s_delta_time;
    }

    void Canvas::updateDebugText()
    {
        sf::String output = "List FPS: " + toString(1.0f/getDeltaTime(), 2) + 
                            " Mouse: x=" + toString(ui::getMousePosition(window, &getView()).x, 0) + "y=" + toString(ui::getMousePosition(window, &getView()).y, 0) + "\n\n";
        
        for (int i = 0; i < widget_list.size(); i++)
        {
			if (widget_list[i]->getParent() == nullptr)
			{
				 output += printWidget(widget_list[i], sf::String());
			}
        }
        
        debug_text.setString(output);
    }

    sf::String Canvas::printWidget(Widget* p_widget, sf::String p_indent)
    {
        // Display widget.
        sf::String output = p_indent;
        
        sf::String type = "Widget";
		Graphic* graphic = dynamic_cast<Graphic*>(p_widget);
		TextField* text_field = dynamic_cast<TextField*>(p_widget);
		Button* button = dynamic_cast<Button*>(p_widget);
		// ScrollList* scrolllist = dynamic_cast<ScrollList*>(p_widget);
		// DropDownList* dropdownlist = dynamic_cast<DropDownList*>(p_widget);
		// InputField* inputfield = dynamic_cast<InputField*>(p_widget);
		// ScrollBar* scrollbar = dynamic_cast<ScrollBar*>(p_widget);
		// LayoutGroup* layoutgroup = dynamic_cast<LayoutGroup*>(p_widget);
		if(graphic != nullptr) type = "Graphic";
		if(text_field != nullptr) type = "TextField";
		if(button != nullptr) type = "Button";
		// if(scrolllist != nullptr) type = "ScrollList";
		// if(inputfield != nullptr) type = "InputField";
		// if(scrollbar != nullptr) type = "ScrollBar";
	    // if(layoutgroup != nullptr) type = "LayoutGroup";
	    output += type;
	    
        if(!p_widget->isVisible()) output += " NOT_VISIBLE ";
        if(!p_widget->isUpdated()) output += " NOT_UPDATED ";
        if(!p_widget->isSelectable()) output += " NOT_SELECTABLE ";
        
        if(p_widget->isHovered()) output += " HOVERED ";
        if(p_widget->isSelected()) output += " SELECTED ";
        if(p_widget->isMouseOver()) output += " MOUSEOVER ";
        
        output += "\n";
        p_indent += "---";
        for (int j = 0; j < p_widget->getChildCount(); j++)
        {
            output += printWidget(p_widget->getChild(j), p_indent);
        }

        return output;
    }

    ResourceHandler<std::string, sf::Font> Fonts::s_fontTable;

    bool Fonts::load(const std::string& p_name, const std::string& p_filePath)
    {
        return s_fontTable.load(p_name, p_filePath);
    }
    sf::Font& Fonts::get(const std::string& p_name)
    {
        return s_fontTable.get(p_name);
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
