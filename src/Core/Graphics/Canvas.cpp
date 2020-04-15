#include <SFML/Graphics/Vertex.hpp>

#include <Graphics/Canvas.hpp>
#include <System/Loaders.hpp>
#include <Utils/StringUtils.hpp>

namespace obe::Graphics::Canvas
{
    CanvasElement::CanvasElement(Canvas& parent, const std::string& id)
        : ProtectedIdentifiable(id)
        , parent(parent)
    {
    }

    void CanvasElement::setLayer(const unsigned int layer)
    {
        if (this->layer != layer)
        {
            this->layer = layer;
            parent.requiresSort();
        }
    }

    Line::Line(Canvas& parent, const std::string& id)
        : CanvasElement(parent, id)
    {
    }

    void Line::draw(RenderTarget target)
    {
        const Transform::UnitVector p1px = p1.to<Transform::Units::ScenePixels>();
        const Transform::UnitVector p2px = p2.to<Transform::Units::ScenePixels>();
        const sf::Vertex firstVertex(sf::Vector2f(p1px.x, p1px.y), p1color);
        const sf::Vertex secondVertex(sf::Vector2f(p2px.x, p2px.y), p2color);
        const sf::Vertex line[] = { firstVertex, secondVertex };
        target.draw(line, 2, sf::Lines);
    }

    CanvasPositionable::CanvasPositionable(Canvas& parent, const std::string& id)
        : CanvasElement(parent, id)
    {
        // Default Canvas elements unit is ScenePixels
        position.unit = Transform::Units::ScenePixels;
    }

    Rectangle::Rectangle(Canvas& parent, const std::string& id)
        : CanvasPositionable(parent, id)
    {
        this->size.unit = Transform::Units::ScenePixels;
    }

    void Rectangle::draw(RenderTarget target)
    {
        target.draw(shape);
    }

    Text::Text(Canvas& parent, const std::string& id)
        : CanvasPositionable(parent, id)
        , h_align()
        , v_align()
    {
        texts.emplace_back();
    }

    void Text::draw(RenderTarget target)
    {
        Transform::UnitVector offset(Transform::Units::ScenePixels);
        if (h_align == TextHorizontalAlign::Center)
            offset.x -= shape.getGlobalBounds().getSize().x / 2;
        else if (h_align == TextHorizontalAlign::Right)
            offset.x -= shape.getGlobalBounds().getSize().x;
        if (v_align == TextVerticalAlign::Center)
            offset.y -= shape.getGlobalBounds().getSize().y / 2;
        else if (v_align == TextVerticalAlign::Bottom)
            offset.y -= shape.getGlobalBounds().getSize().y;
        shape.move(offset);
        target.draw(shape);
        shape.move(-offset);
    }

    void Text::refresh()
    {
        shape.clear();
        for (auto text : texts)
        {
            if (!text.string.empty())
            {
                shape.append(text);
            }
        }
    }

    Graphics::Text& Text::currentText()
    {
        return texts.back();
    }

    Circle::Circle(Canvas& parent, const std::string& id)
        : CanvasPositionable(parent, id)
    {
    }

    void Circle::draw(RenderTarget target)
    {
        target.draw(shape);
    }

    Polygon::Polygon(Canvas& parent, const std::string& id)
        : CanvasPositionable(parent, id)
    {
    }

    void Polygon::draw(RenderTarget target)
    {
        target.draw(shape);
    }

    Image::Image(Canvas& parent, const std::string& id)
        : CanvasPositionable(parent, id)
    {
    }

    void Image::draw(RenderTarget target)
    {
        target.draw(sprite);
    }

    void Canvas::sortElements()
    {
        std::sort(m_elements.begin(), m_elements.end(),
            [](const auto& elem1, const auto& elem2) {
                return elem1->layer > elem2->layer;
            });
    }

    Canvas::Canvas(unsigned int width, unsigned int height)
    {
        m_canvas.create(width, height);
    }

    CanvasElement* Canvas::get(const std::string& id)
    {
        for (auto& elem : m_elements)
        {
            if (elem->getId() == id)
            {
                return elem.get();
            }
        }
        return nullptr;
    }

    void Canvas::render(Sprite& target)
    {
        m_canvas.clear(sf::Color(0, 0, 0, 0));

        if (m_sortRequired)
        {
            this->sortElements();
            m_sortRequired = false;
        }

        for (auto& element : m_elements)
        {
            if (element->visible)
                element->draw(m_canvas);
        }
        m_canvas.display();
        target.setTexture(m_canvas.getTexture());
    }

    void Canvas::clear()
    {
        m_elements.clear();
    }

    void Canvas::remove(const std::string& id)
    {
        m_elements.erase(std::remove_if(m_elements.begin(), m_elements.end(),
                             [&id](auto& elem) { return elem->getId() == id; }),
            m_elements.end());
    }

    const Texture& Canvas::getTexture() const
    {
        return m_canvas.getTexture();
    }

    void Canvas::requiresSort()
    {
        m_sortRequired = true;
    }
} // namespace obe::Graphics::Canvas