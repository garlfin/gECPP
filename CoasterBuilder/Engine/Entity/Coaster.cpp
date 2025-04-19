//
// Created by scion on 4/11/2025.
//

#include "Coaster.h"

namespace Coaster
{
    void TrackPiece::IDeserialize(istream& in, SETTINGS_T s) {}
    void TrackPiece::ISerialize(ostream& out) const {}

    Reference<gE::Mesh> TrackPiece::GetMesh(TrackMod type) const
    {
        for(const auto& mesh : Meshes)
            if((bool)(mesh.first & type) || mesh.first == type)
                return mesh.second;

        return DEFAULT;
    }

    REFLECTABLE_ONGUI_IMPL(TrackPiece, )
    REFLECTABLE_FACTORY_IMPL(TrackPiece);

    void CoasterType::IDeserialize(istream& in, SETTINGS_T s) {}
    void CoasterType::ISerialize(ostream& out) const {}

    REFLECTABLE_ONGUI_IMPL(CoasterType, );
    REFLECTABLE_FACTORY_IMPL(CoasterType);

    Coaster::Coaster(gE::Window* window, const Reference<const CoasterType>& type) : Entity(window),
        _type(type),
        _editor(this),
        _splineRenderer(this, DEFAULT, _spline)
    {
        _splineRenderer.SetEnableDebugView(true);
        _spline.AddPoint({ glm::vec3(0.f, 0.f, 0.f) });

        _spline.AddPoint({ glm::vec3(1.f, 0.f, 0.f) });
        _spline.AddPoint({ glm::vec3(1.f, 1.f, 0.f) });

        _spline.AddPoint({ glm::vec3(1.f, 2.f, 0.f) });
        _spline.AddPoint({ glm::vec3(0.f, 2.f, 0.f) });

        _spline.AddPoint({ glm::vec3(-1.f, 2.f, 0.f) });
        _spline.AddPoint({ glm::vec3(-1.f, 1.f, 0.f) });

        _spline.AddPoint({ glm::vec3(-1.f, 0.f, 0.f) });
        _spline.AddPoint({ glm::vec3(0.f, 0.f, 0.f) });
    }
}
