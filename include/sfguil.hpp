#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <iostream> /* debugging */
#include <sstream>

namespace ui
{
    class Canvas;
    class Panel;

    enum AnchorPoint { TopLeft, Top, TopRight, Left, Middle, Right, BottomLeft, Bottom, BottomRight };
    enum Orientation { Horizontal, Vertical };

    /*
    Widgets define a space in which UI elements can be drawn. Everything that is a UI element derives from this class. Overridden functions should call the base class implementation.
    */
    class Widget : public sf::Drawable, public sf::Transformable
    {
    private:
        sf::Vector2f size;

        bool is_visible = true; 
        bool is_updated = true; 
        bool is_selectable = true;

        int layer = 0;

        Widget* mask = nullptr;
        std::vector<Widget*> children;
        Widget* parent = nullptr;
    protected:
        sf::RenderWindow& window;
        Canvas& canvas;
    public:
        Widget(Canvas& p_canvas);
        virtual ~Widget();

        virtual void setSize(const sf::Vector2f& p_size);
        const sf::Vector2f& getSize() const;
        sf::Transform getWorldTransform() const; // Transform relative to the window origin as opposed to getTransform() which is only relative to its parent.

        virtual void draw(sf::RenderTarget& p_target, sf::RenderStates p_states) const;
        virtual void updateLogic();
        virtual void updateEvents(sf::Event& p_event);

        // Whether updatedLogic() and updateEvents() and draw() should be called by the canvas.
        virtual void setVisible(bool p_state);
        bool isVisible();
        // Whether updatedLogic() and updateEvents() should be called by the canvas.
        virtual void setUpdated(bool p_state);
        bool isUpdated();
        // Canvas.getSelected() will never return this instance if it is not selectable, nor is the user able to select the widget by clicking left on it.
        virtual void setSelectable(bool p_state);
        bool isSelectable();

        // Set another widget as mask of this widget. Everything not drawn in the bounds of the mask will be invisible. Method using shaders.
        virtual void setMask(Widget* p_mask);
        Widget* getMask() const;

        // Layer determines the order in which the widgets are drawn. If widgets have the same layer, they will be drawn in the order the layer was set.
        virtual void setLayer(int layer_index);
        int getLayer();

        // Children are other widgets that are connected to this widget since most UI elements are build up from multiple elements like buttons.
        // Also children inherit certain properties like the transformation or state of their parent (visible, updated, selectable, mask, layer). Grouping widgets also allows easy mouse over checking.
        void addChild(Widget* p_widget, int p_index = -1);
        void removeChild(Widget* p_widget);
        Widget* getChild(int p_index) const;
        Widget* getParent() const;
        int getChildCount() const;

        sf::String printSelf(int depth);

        // Checks if the mouse is over this widget or any of its children.
        bool isMouseOver();
        bool isMouseOverChildren();
        bool isHovered();
        bool areChildrenHovered();
        bool isSelected();
        bool areChildrenSelected();
    };

    /*
    Graphics are widgets that represent visual content like a color or texture.
    */
    class Graphic : public Widget
    {
    private:
        mutable sf::RectangleShape background;
        bool preserve_aspect;
    public:
        Graphic(Canvas& p_canvas);

        void setTexture(const sf::Texture* p_texture, const sf::IntRect& p_texture_rect);     
        void setTexture(const sf::Texture* p_texture);
        const sf::Texture* getTexture();
        const sf::Vector2f& getImagePosition();
        const sf::Vector2f& getImageSize();
        void setOutlineColor(const sf::Color& p_color);
        void setOutlineThickness(float p_thickness);
        void setFillColor(const sf::Color& p_color);
        void setPreserveAspect(bool p_state);

        const sf::Color& getOutlineColor();
        float getOutlineThickness();
        const sf::Color& getFillColor();
        bool isPreservingAspect();

        void draw(sf::RenderTarget& p_target, sf::RenderStates p_states) const override;
    private:
        void preserveAspect() const;
    };

    class TextField : public Widget
    {
    public:
        enum Alignment
        {
            Left, Right, Middle, Top, Bottom
        };
        enum OverflowType
        {
            Overflow, Truncate, Dotted
        };
    private:
        // Text attributes.
        sf::String string;
        sf::Font& font;
        unsigned int character_size;
        float line_spacing_factor;
        float paragraph_spacing_factor;
        sf::Color text_color;
        Alignment horizontal_alignment;
        Alignment vertical_alignment;
        OverflowType overflow_type;
        bool wrapping;

        // Geometry.
        struct CharacterInfo
        {
            float baseline = 0.f; // Height of the baseline the character sits on.
            int start_vertex = 0, end_vertex = 0; // Index of the first and last vertex of the 6 vertices making up the glyph. start_vertex is the left bottom vertex, end_vertex is the right bottom vertex.
        };
        mutable std::vector<std::vector<CharacterInfo>> text_info; // Stores each line in a vector that stores the character info for each character.
        mutable sf::FloatRect text_bounds; // TODO: Update accordingly.
        mutable sf::VertexArray vertices;
        mutable bool geometry_need_update;

        mutable sf::Vector2f start_of_string; // (Needed?) First position in the text. Valid position even when there are no characters in the string. Calculation of vertices results in the cursor position moving slightly down which is prevented with this vector.
        mutable sf::Vector2f end_of_string; // Last position in the text. Due to whitespaces having no width, there would be no visual difference after placing a whitespace at the end of string when calculating the end of string from vertices. The vector makes sure if there is a last whitespace it will be visible.

        // Selection of a specific part of the text.
        int start_selection = 0, end_selection = 0;
        sf::Color selection_color;
        mutable sf::VertexArray selection_vertices;

        // Debug vertices.
        mutable sf::VertexArray debug_vertices;
    public:
        TextField(Canvas& p_canvas);

        void setString(const sf::String& p_string);
        void setFont(const sf::Font& p_font);
        void setCharacterSize(unsigned int p_size);
        void setLineSpacing(float p_spacing_factor);
        void setParagraphSpacing(float p_sapcing_factor);
        void setTextColor(const sf::Color& p_color);
        void setOverflowType(OverflowType p_type);
        void enableWrapping(bool p_state);
        void setHorizontalAlignment(Alignment p_type);
        void setVerticalAlignment(Alignment p_type);
        void setSize(const sf::Vector2f& p_size) override;
        void setSelection(int p_start, int p_end);
        void setSelectionColor(const sf::Color& p_color);

        const sf::String& getString() const;
        const sf::Font& getFont() const;
        unsigned int getCharacterSize() const;
        float getLineSpacing() const;
        float getParagraphSpacing() const;
        const sf::Color& getTextColor() const;
        OverflowType getOverflowType() const;
        bool isWrapping() const;
        Alignment getHorizontalAlignment() const;
        Alignment getVerticalAlignment() const;
        const sf::Color& getSelectionColor() const;

        void matchSizeToText();

        sf::Vector2f findCharacterPos(int p_index) const;
        int findIndex(const sf::Vector2f& p_position) const;
    private:
        void ensureGeometryUpdate() const;
        void draw(sf::RenderTarget& p_target, sf::RenderStates p_states) const override;
        void addGlyphQuad(sf::VertexArray& p_vertices, const sf::Vector2f& p_position, const sf::Color& p_color, const sf::Glyph& p_glyph) const;

        void applyHorizontalAlignment() const;
        void applyVerticalAlignment() const;
        sf::String applyOverflowType() const;
        void applySelection() const;
        void applyStringGeometry(const sf::String& p_string) const;
        void applyDebugGeometry() const;
    };

    class Button : public Widget
    {
    private:
        std::function<void()> f_event;
        sf::Color hovered_color;
        sf::Color pressed_color;
        sf::Color standard_color;

        float margin_left;
        float margin_right;
        float margin_top;
        float margin_bottom;
    public:
        Graphic background;
        TextField text;
    public:
        Button(Canvas& p_canvas);

        void setSize(const sf::Vector2f& p_size) override;
        void setOnClickEvent(const std::function<void()>& p_event);

        void setHoveredColor(const sf::Color& p_color);
        void setPressedColor(const sf::Color& p_color);
        void setStandardColor(const sf::Color& p_color);

        void setMask(Widget* p_widget) override;

        void setTextMargin(float p_margin_left, float p_margin_right, float p_margin_top, float p_margin_bottom);

        void updateLogic() override;
        void updateEvents(sf::Event& p_event) override;
    };

    class ScrollBar : public Widget
    {
    public:
    	struct ScrollState 
    	{
    		// Defines the visible part of the list in pixels.
    		float scroll_handle_position;
    		float scroll_handle_size;
            // Defines the lists whole length in pixels.
            float scroll_list_length;
    	};
    private:
	    Graphic background;
	    Button move_up_button;
	    Button move_down_button;
	    Graphic scroll_handle;

	    ScrollState state;
	    bool handle_selected = false;
	    sf::Vector2f last_mouse_position;
	    std::function<void()> f_onScroll = [](){};

        float handle_margin;
        sf::Color handle_color;
        sf::Color hovered_handle_color;
    public:
        ScrollBar(Canvas& p_canvas);

        void setSize(const sf::Vector2f& p_size) override;

        void setScrollState(float p_position, float p_size, float p_list_length);
        const ScrollState& getScrollState();
        void setOnScrollListener(const std::function<void()>& p_event);

        void updateEvents(sf::Event& p_event) override;
        void updateLogic() override;

        void setMask(Widget* p_widget) override;
    };

    class ScrollList : public Widget
    {
    private:
        std::vector<Widget*> content;
        std::vector<int> break_indices;
    public:
        Graphic background;
        ScrollBar scroll_bar;
    public:
        ScrollList(Canvas& p_canvas);

        void setSize(const sf::Vector2f& p_size) override;

        void setMask(Widget* p_widget) override;
        void setLayer(int p_layer) override;

        void addContent(Widget* p_widget);
        void removeContent(Widget* p_widget);
        void clearContent();
        Widget* getContent(int p_index) const;
        int getContentCount() const;

        void breakLine();

        // Call after changing transformation of a child. Automatically called after adding/removing content.
        void refresh();

        void updateEvents(sf::Event& p_event) override;
    private:
        void refreshScrollbar();

        sf::Vector2f getGroupSize() const;
        void list(float p_horizontal_spacing, float p_vertical_spacing, const sf::Vector2f& p_position = sf::Vector2f(0.f, 0.f));
    };

    class DropDownList : public Widget
    {
    public:
        Button dropdown_button;
        TextField placeholder_text;
        ScrollList scroll_list;
    private:
        Graphic icon;

        std::function<void()> f_onSelect;
        std::function<void()> f_onDeselect;
    public:
        DropDownList(Canvas& p_canvas);

        void setSize(const sf::Vector2f& p_size) override;

        void updateEvents(sf::Event& p_event) override;

        void setOnSelectEvent(const std::function<void()>& p_event);
        void setOnDeselectEvent(const std::function<void()>& p_event);

        void setString(const sf::String& p_string);
        sf::String getString();

        void setPlaceholderText(const sf::String& p_string);

        void setList(const std::vector<sf::String>& p_elements);
        void setListLength(float p_length);

        void setLayer(int p_layer) override;
        void setVisible(bool p_state) override;
    };

    class InputField : public Widget
    {
    public:
        enum Type 
        { 
            Standard                = 1 << 0, 
            IntegerNumber           = 1 << 1, 
            DecimalNumber           = 1 << 2, 
            HexadecimalNumber       = 1 << 3, 
            BinaryNumber            = 1 << 4, 
            Alphanumeric            = 1 << 5,
            Alpha                   = 1 << 6,
            
            Date                    = 1 << 7, 
            LimitedValue            = 1 << 8, 
            Capitalized             = 1 << 9,
            Custom                  = 1 << 10
        };
        sf::Color unselected_color = sf::Color(170, 170, 170);
        sf::Color selected_color = sf::Color(43, 156, 255);
        sf::Color highlight_color = sf::Color(100, 100, 100);
        sf::Color standard_color = sf::Color::White;
        sf::Color error_color = sf::Color(255, 100, 100);
        sf::Color placeholder_text_color = sf::Color(100, 100, 100);
    private:
        Graphic background;
        TextField text;
        TextField placeholder_text;
        Graphic cursor;

        int cursor_index = 0;
        int drag_cursor_index = 0;
        bool is_making_mouse_selection = false;
        bool is_making_shift_selection = false;
        int max_character_limit = 1000;
        int min_character_limit = 0;
        float max_value_limit = 0.f;
        float min_value_limit = 1000.f; 

        bool limit_error = false;
        bool input_error = false;
        bool date_error = false;
        sf::Uint32 type = Type::Standard;
        std::function<bool(sf::Uint32 p_char)> f_customIsCharValid;

        std::function<void()> f_onSelect;
        std::function<void()> f_onDeselect;
        std::function<void()> f_onTyped;
        bool was_selected = false;
        bool was_hovered = false;

        float blinking_timer = 0.f;
        float blinking_interval = 0.5f;

        float text_padding = 5.f;
    public:
        InputField(Canvas& p_canvas);

        void setSize(const sf::Vector2f& p_size) override;
        void setString(const sf::String& p_string);
        const sf::String& getString();
        void setPlaceholderText(const sf::String& p_string);
        void setCharacterSize(unsigned int p_size);
        void setInputType(sf::Uint32 p_type);
        void setCharacterLimit(int p_min, int p_max); // How many characters there can be in the inputfield at minimum and maximum.
        void setValueLimit(float p_min, float p_max); // If the inputfield expects number values, these are the smallest and biggest allowed numbers.
        bool isInputValid();
        void setValidChars(const std::function<bool(sf::Uint32 p_char)>& p_function); // Lets the user write a function that takes the inputted char in and outputs whether that char is valid. Set the type to Custom to use this.
        void setOnSelectEvent(const std::function<void()>& p_event);
        void setOnDeselectEvent(const std::function<void()>& p_event);
        void setOnTypedEvent(const std::function<void()>& p_event);

        void updateLogic() override;
        void updateEvents(sf::Event& p_event) override;

        void setMask(Widget* p_mask) override;
        void setLayer(int p_layer) override;
        void setVisible(bool p_state) override;
    private:
        void refreshCursor();
        bool isCharValid(sf::Uint32 p_char);
        void refreshDateFormat();
        void refreshLimitedFormat();
        void moveTextHorizontally(int p_index);
    };

    class InputList : public Widget
    {
    public:
        InputField inputfield;
        ScrollList scroll_list;
    private:
        std::function<void()> f_onSelect;
        std::function<void()> f_onDeselect;
    public:
        InputList(Canvas& p_canvas);

        void setSize(const sf::Vector2f& p_size) override;

        void updateEvents(sf::Event& p_event) override;

        void setOnSelectEvent(const std::function<void()>& p_event);
        void setOnDeselectEvent(const std::function<void()>& p_event);

        void setString(const sf::String& p_string);
        sf::String getString();

        void setList(const std::vector<sf::String>& p_elements);
        void setListLength(float p_length);

        void setMask(Widget* p_widget) override;
        void setLayer(int p_layer) override;
        void setVisible(bool p_state) override;
        void setUpdated(bool p_state) override;
        void setSelectable(bool p_state) override;
    };

    class DockNode : public Widget
    {
    public:
        enum PanelState { Floating, Static };
        enum PanelLayout { Horizontal, Vertical, Center };
        std::vector<Panel*> panels;
    private:
        std::vector<DockNode*> child_nodes;
        PanelLayout orientation = Horizontal;
        PanelState panel_state = Floating;

        // Resize indicator.
        ui::Graphic* hovering_indicator = nullptr;
        ui::Graphic* dragging_indicator = nullptr;
        sf::Vector2f dragging_position;

        struct ResizeIndicator
        {
            ui::Graphic* indicator = nullptr;
            ui::Widget* hover_area = nullptr;
        };
        std::vector<ResizeIndicator*> resize_indicators;

        // Panel Bar.
        std::vector<Button*> buttons;
        float button_width = 100.f;
        Panel* dragged_panel = nullptr;
    public:
        DockNode(Canvas& p_canvas);
        ~DockNode();

        void addNode(DockNode* p_docknode);
        bool removeNode(DockNode* p_docknode);
        int getNodeCount();
        DockNode* getNode(int p_index);
        void copyNodes(DockNode* p_copy_to);

        void setPanelLayout(PanelLayout p_orientation);
        PanelState getPanelState();

        Panel* getDraggedPanel();

        void updateLogic() override; 
        void updateEvents(sf::Event& p_event) override;
        void setLayer(int p_layer) override;

        void setSize(const sf::Vector2f& p_size) override;
        void setSizeProportionally(const sf::Vector2f& old_size, const sf::Vector2f& new_size);

        void addPanel(Panel* p_panel);
        void removePanel(Panel* p_panel);
        void setPanelState(PanelState p_panel_state);
    private:
        void recreateResizeIndicators();
        void deleteResizeIndicators();

        void resizeNodes();

        void recreateTitleButtons();
        void deleteTitleButtons();
    };

    class DockSpace : public Widget
    {
    public:
        Panel* root = nullptr;
    private:
        std::vector<DockNode*> nodes;
        std::vector<Panel*> panels;

        DockNode* hovered_node = nullptr;
        DockNode* dragged_node = nullptr;

        int insert_index = 0;

        ui::Graphic top_marker;
        ui::Graphic bottom_marker;
        ui::Graphic left_marker;
        ui::Graphic right_marker;
        ui::Graphic center_marker;

        sf::Vector2f mouse_press_position;
    public:
        DockSpace(Canvas& p_canvas);
        ~DockSpace();

        Panel* createPanel(const sf::String& p_title);

        void insertPanel(Panel* p_floating_panel, Panel* p_inserted_panel, int p_pos = 0);
        void removePanel(Panel* p_panel);

        void updateLogic() override;
        void updateEvents(sf::Event& p_event) override;
        sf::String printNodes();
    private:
        void insertNode(DockNode* p_floating_node, DockNode* p_inserted_node, int p_pos = 0);
        void removeNode(DockNode* p_node);
    };

    class Panel : public Widget
    {
    private:
        // Resizing.
        ui::Graphic resize_indicator;
        std::array<ui::Graphic, 4> resize_hover_area;
        bool is_resizeable = true;
        int is_resizing = -1;
        int is_hovering = 0;
        sf::Vector2f resize_begin_position;

        // Dragging.
        bool draggable = true;
        bool is_dragging = false;
        bool is_dragging_title = false;
        sf::Vector2f mouse_offset;

        // Panel.
        ui::Graphic background;
        ui::Graphic title_background;
        ui::Button title;
        ui::ScrollList scrolllist;
    public:
        Panel(Canvas& p_canvas);
        ~Panel();

        void setDraggable(bool p_state);
        void setDragging(bool p_state);
        bool isDragging();
        void setResizable(bool p_state);

        bool isDraggingTitle();

        void updateLogic() override;
        void updateEvents(sf::Event& p_event) override;

        void setTitle(const sf::String& p_string);
        const sf::String& getTitle();
        void setSize(const sf::Vector2f& p_size) override;
        //void refresh();

        void setLayer(int layer_index) override;

        void breakLine();
        //void addSpace();

        ui::Graphic* createGraphic();
        ui::TextField* createText(const sf::String& p_content);        
        ui::Button* createButton(const sf::String& p_text);
        ui::Button* createButton(const sf::String& p_text, const std::function<void()>& p_action);
        ui::ScrollList* createScrollList();
        ui::DropDownList* createDropDownList();
        ui::InputField* createInputField();
        ui::InputList* createInputList();

        template<typename T>
        T* create()
        {
            T* widget = new T(canvas);
            widget->setSize(sf::Vector2f(100.f, 100.f));
            widget->setMask(this);

            scrolllist.addContent(widget);
            // items.push_back({ widget });
            // addChild(widget);
            // setLayer(getLayer()); // Put all new children on the current layer of the panel.
            // refresh();

            return widget;
        }
    };


    /*
    A canvas draws, updates the logic, the events, and handles the layout of widgets. A canvas needs a reference to a window in order to know where it has to draw its widgets.
    */
    class Canvas
    {
    public:
        sf::RenderWindow& window;
        sf::Font& font;

        const sf::Texture* scroll_bar_up_texture = nullptr; 
        const sf::Texture* scroll_bar_down_texture = nullptr; 
        const sf::Texture* dropdown_arrow_texture = nullptr;

        std::vector<Widget*> widget_list;
    private:
        Widget* selected_widget = nullptr;
        Widget* hovered_widget = nullptr;
        
        // The view of the canvas always shows the 2d world that fits into the window with origin in the top left corner. It should be used for getting the mouse position. The views of all widgets act the same, only that they are showing a small part of the window. Viewport and depicted part of the 2d world always overlay.
        sf::View view;
        sf::Shader clipping_shader;

        static float s_delta_time;
        sf::Clock clock;

        int control_index = 0;
    public:
        Canvas(sf::RenderWindow& p_window, sf::Font& p_font);

        void drawAll();
        void updateAllLogic();
        void updateAllEvents(sf::Event& p_event);

        void add(Widget* p_widget);
        void remove(Widget* p_widget);

        // Changes the position of the widget in the widget_list, therefore changing its drawing order. This won't apply the given layer to the widget. Use Widget::setLayer().
        void moveToLayer(Widget* p_widget, int p_layer);
        // Move the given widget (and its children) to the top of the widget_list.
        void moveInFront(Widget* p_widget);
    
        // Get a pointer to the selected widget. If nothing is selected, nullptr is returned.
        Widget* getSelected();
        // Get a pointer to the top most widget the mouse is currently hovering over.
        Widget* getHovered();
        // Get the view of this canvas.
        const sf::View& getView() const;
        sf::Shader& getClippingShader();

        void select(Widget* p_widget);

        sf::String print();

        static float getDeltaTime();

        void setIcons(const sf::Texture* p_scroll_bar_up, const sf::Texture* p_scroll_bar_down, const sf::Texture* p_dropdown_arrow);
    private:
        // Find index of widget in list. If the widget is not stored, -1 is returned.
        int find(Widget* p_widget) const;
    };

    /*
    The ResourceHandler can hold multiple resources that can be accessed through a key.
    */
    template<typename Identifier, typename Item>
    class ResourceHandler
    {
    private:
        std::map<Identifier, Item*> m_resourceTable;
        Item m_errorItem;
    public:
        ~ResourceHandler()
        {
            for(const auto& element : m_resourceTable)
            {
                delete element.second;
            }
            m_resourceTable.clear();
        }
        bool load(Identifier p_identifier, const std::string& p_itemPath)
        {
            Item* item = new Item();
            if(!item->loadFromFile(p_itemPath))
            {
                throw std::runtime_error("ResourceHandler: Failed to load " + p_itemPath);
                sf::String temp(p_itemPath);
                std::wcerr << L"ResourceHandler: Failed to load " << temp.toWideString() << std::endl;
                return false;
            }
            auto inserted = m_resourceTable.insert(std::make_pair(p_identifier, item));
            if(inserted.second == false)
            {
                //assert(inserted.second) -> calls abort()
                throw std::logic_error("ResourceHandler: Loaded resource " + p_itemPath + " twice.");
                sf::String temp(p_itemPath);
                std::wcerr << L"ResourceHandler: Loaded resource " << temp.toWideString() << L" twice." << std::endl;
                return false;
            }
            return true;
        }
        Item& get(Identifier p_identifier)
        {
            auto search = m_resourceTable.find(p_identifier);
            if(search == m_resourceTable.end())
            {
                //assert(search != m_resourceTabel.end()) -> calls abort()
                //throw std::logic_error("ResourceHandler: Tried to get resource without loading it.");
                std::wcerr << L"ResourceHandler: Tried to get resource without loading it." << std::endl;
                return m_errorItem;
            }
            return *search->second;
        }
    };

    class Cursor
    {
    private:
        struct CursorData
        {
        public:
            // All cursor available by the system through SFML.
            sf::Cursor system_cursors[13];
        public:
            CursorData();

            void setCursor(sf::Cursor::Type p_type, sf::RenderWindow& p_window);
        };
        static CursorData s_data;
    public:
        static void setCursor(sf::Cursor::Type p_type, sf::RenderWindow& p_window);
    };

    /*
    Get the current mouse position relative to the window. If no view is provided the view currently used by the window is used. 
    @param window The window used.
    @param view The view whose local space the mouse position should be retrieved from.
    */
    sf::Vector2f getMousePosition(sf::RenderWindow& window, const sf::View* view = nullptr);
    /*
    Get the current mouse position relative to a point. If no point is specified the window's upper left corner is used. 
    If no view is provided the view currently used by the window is used. 
    @param window The window used.
    @param view The view whose local space the mouse position should be retrieved from.
    @param relative_to The point to which the mouse position should be relative to. 
    */
    sf::Vector2f getMousePosition(sf::RenderWindow& window, const sf::Vector2f& relative_to, const sf::View* view = nullptr);

    /*
    Check whether the mouse is over a circle constructed from a center and radius.
    @return True = Mouse over circle, False = Mouse not over circle
    */
    bool mouseOverCircle(const sf::Vector2f& center, float radius, sf::RenderWindow& window, sf::View* view = nullptr);

    /*
    Split a string by some character.
    */
    std::vector<sf::String> splitStringByChar(const sf::String& str, const wchar_t& c);

    template<typename T>
    T clampValue(const T& p_value, const T& p_min, const T& p_max)
    {
        T modified_value = p_value;
        if(modified_value < p_min) modified_value = p_min;
        else if (modified_value > p_max) modified_value = p_max;
        return modified_value;
    }

    sf::String toUppercase(const sf::String& p_string);
    sf::Uint32 toUppercase(sf::Uint32 p_char);

    /* 
    Transform some numeric value into a string with precision. 
    */
    template <typename T>
    sf::String toString(const T p_value, const int p_digits = 6)
    {
        std::ostringstream out;
        out.precision(p_digits);
        out << std::fixed << p_value;
        sf::String replaced_string(out.str());
        replaced_string.replace(".", ",");
        return replaced_string;
    }

    /* Transform a number into a string representing the given number in binary format. */
    sf::String toBinaryString(int p_value, int p_max_digits = 8, sf::String p_base_string = "0b");
    /* Transform a number into a string representing the given number in hexadecimal format. */
    sf::String toHexString(int p_value, bool p_uppercase = false, int p_max_digits = 4, sf::String p_base_string = "0x");
    /* Transform a string into a decimal number. Dots as well as commas are valid. Invalid strings (containing letters/symbols) return 0. */
    float stringToFloat(const sf::String& p_string);
    /* Transform a string into a integer. Invalid strings (containing letters/symbols) return 0. */
    int stringToInt(const sf::String& p_string);
    /* Transform a string representing a number in hexadecimal format into a number. */
    unsigned int hexadecimalToInt(sf::String p_string);
    /* Transform a string representing a number in binary format into a number. */
    unsigned int binaryToInt(sf::String p_string);
};
