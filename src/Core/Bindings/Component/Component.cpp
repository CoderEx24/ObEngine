#include <Bindings/Component/Component.hpp>

#include <Component/Component.hpp>

#include <sol/sol.hpp>

namespace obe::Component::Bindings
{
    void LoadClassComponentBase(sol::state_view state)
    {
        sol::table ComponentNamespace = state["obe"]["Component"].get<sol::table>();
        sol::usertype<obe::Component::ComponentBase> bindComponentBase
            = ComponentNamespace.new_usertype<obe::Component::ComponentBase>(
                "ComponentBase", sol::base_classes,
                sol::bases<obe::Types::Serializable, obe::Types::Identifiable>());
        bindComponentBase["inject"] = &obe::Component::ComponentBase::inject;
        bindComponentBase["remove"] = &obe::Component::ComponentBase::remove;
        bindComponentBase["dump"] = &obe::Component::ComponentBase::dump;
        bindComponentBase["load"] = &obe::Component::ComponentBase::load;
        bindComponentBase["type"] = &obe::Component::ComponentBase::type;
    }
};