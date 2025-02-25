//
// Created by scion on 11/13/2024.
//

#include "Mesh.h"

#include <Window.h>

namespace gE
{
    void Mesh::IDeserialize(istream& in, SETTINGS_T s)
    {
        Read(in, Name);

        if(Version == 2)
        {
            Read<u8>(in);
            //MaterialNames = ReadArray<u8, std::string>(in);

            Read(in, Bounds);
        }

        const TYPE_T* type = ReadType<Window*>(in);
        GE_ASSERTM(type, "NO TYPE INFO!");

        // Super dumb but super works.
        Serializable* vao = type->Factory(in, s);
        VAO = ptr_cast((API::IVAO*) vao->GetUnderlying());

        if(Version == 2 && Read<bool>(in))
        {
            const TYPE_T* shapeType = ReadType<Window*>(in);
            GE_ASSERTM(shapeType, "NO SHAPE TYPE INFO!");

            Serializable* shape = shapeType->Factory(in, s);
            Shape = ptr_cast((Jolt::Shape*) shape->GetUnderlying());
        }
    }

    void Mesh::ISerialize(ostream& out) const
    {
        Write(out, Name);
        Write(out, (u8) 0);
        //WriteArray<u8>(out, MaterialNames);

        Write(out, Bounds);

        const TYPE_T* type = GetMeshType();
        GE_ASSERTM(type, "NO TYPE INFO!");

        WriteType(out, *type);
        Write(out, VAO->GetSettings());

        Write(out, (bool) Shape);
        if(Shape)
        {
            const TYPE_T* shapeType = GetShapeType();
            GE_ASSERTM(shapeType, "NO SHAPE TYPE INFO!");

            WriteType(out, *shapeType);
            Write(out, Shape->GetSettings());
        }
    }

    void Mesh::Free()
    {
        Name.clear();
        MaterialNames.Free();
        VAO.Free();
        Shape.Free();
        BoneWeights.Free();
    }

    bool Mesh::IsFree() const
    {
        return Name.empty() && MaterialNames.IsFree() && VAO.IsFree() && Shape.IsFree() && BoneWeights.IsFree();
    }

    REFLECTABLE_ONGUI_IMPL(Mesh,
    {
        DrawField(Field{ "Name" }, Name, depth);
        DrawField(Field{ "VAO" }, VAO->GetSettings(), depth);

        if(Shape)
            DrawField(Field{ "Shape" }, Shape->GetSettings(), depth);
    });

    REFLECTABLE_FACTORY_IMPL(Mesh, Mesh);
}
