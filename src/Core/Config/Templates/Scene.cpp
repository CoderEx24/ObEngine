#include <Config/Templates/GameObject.hpp>

namespace obe::Config::Templates
{
    vili::parser::state getSceneTemplates()
    {
        vili::parser::state state;

        // Units
        state.push_template("SceneUnits", "SceneUnits");
        state.push_template("ScenePixels", "ScenePixels");
        state.push_template("ViewUnits", "ViewUnits");
        state.push_template("ViewPercentage", "ViewPercentage");
        state.push_template("ViewPixels", "ViewPixels");

        // Sprite
        vili::node rect = vili::object { { "x", 0.f }, { "y", 0.f }, { "width", 0.f },
            { "height", 0.f }, { "unit", "SceneUnits" } };
        state.push_template("Rect", rect);
        vili::node positionTransform
            = vili::object { { "x", "Position" }, { "y", "Position" } };
        vili::node cameraTransform
            = vili::object { { "x", "Camera" }, { "y", "Camera" } };
        vili::node parallaxTransform
            = vili::object { { "x", "Parallax" }, { "y", "Parallax" } };

        state.push_template("PositionTransform", positionTransform);
        state.push_template("CameraTransform", cameraTransform);
        state.push_template("ParallaxTransform", parallaxTransform);

        return state;
    }
}