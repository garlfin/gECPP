#pragma once

#include <gECPP/Engine/Component/Component.h>
#include "Engine/Renderer/Material.h"
#include "GL/Binary/Nullable.h"

#ifndef GE_MAX_MATERIAL
#define GE_MAX_MATERIAL 4
#endif

namespace gE
{
	class MaterialHolder : public Component
	{
	 public:
		explicit MaterialHolder(gE::Entity* o) : Component(o) { };

		explicit MaterialHolder(gE::Entity* o, const Array<Reference<Material>>& materials = {})
			: Component(o)
		{
			if(materials.Count()) materials.CopyToCArray(_materials);
		}

		NODISCARD Material& GetMaterial(u8 i) const;

		ALWAYS_INLINE void SetMaterial(u8 i, const Reference<Material>& mat)
		{
			GE_ASSERT(i < GE_MAX_MATERIAL, "MATERIAL OUT OF RANGE");
			_materials[i] = mat;
		}

		ALWAYS_INLINE void SetMaterial(u8 i, Reference<Material>&& mat)
		{
			GE_ASSERT(i < GE_MAX_MATERIAL, "MATERIAL OUT OF RANGE");
			_materials[i] = mat;
		}

		ALWAYS_INLINE void SetNullMaterial(u8 i)
		{
			GE_ASSERT(i < GE_MAX_MATERIAL, "MATERIAL OUT OF RANGE");
			_materials[i] = Reference<Material>();
		}

		void OnUpdate(float d) override { };
		void OnRender(float d, Camera*) override { };

		~MaterialHolder() override = default;

	 private:
		Reference<Material> _materials[GE_MAX_MATERIAL]{};
	};
}

