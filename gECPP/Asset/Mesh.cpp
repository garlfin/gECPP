//
// Created by scion on 11/13/2024.
//

#include "Mesh.h"

#include "Engine/Window/Window.h"

namespace gE
{
    void Mesh::ISerialize(istream& in, SETTINGS_T s)
    {
        Read(in, Name);
        //MaterialNames = ReadArray<u8, std::string>(in);

        //Read(in, AABB);

        const TypeSystem::Type* type = ReadType<Window*>(in);
        GE_ASSERT(type, "NO TYPE INFO!");

        // Super dumb but super works.
        API::VAO* result = (API::VAO*) type->Factory(in, s);
        VAO = ptr_cast<API::IVAO>((API::IVAO*) result->GetUnderlying());
    }

    void Mesh::IDeserialize(ostream& out) const
    {
        Write(out, Name);

        const TypeSystem::Type* type = GetMeshType();
        GE_ASSERT(type, "NO TYPE INFO!");

        WriteType<Window*>(out, *type);
        Write(out, VAO->GetData());
    }
}
