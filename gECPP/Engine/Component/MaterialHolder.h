#pragma once

#include <gECPP/Engine/Component/Component.h>
#include "Engine/Renderer/Material.h"

#ifndef GE_MAX_MATERIAL
#define GE_MAX_MATERIAL 4
#endif

namespace gE
{
	class MaterialHolder : public Component
	{
	 public:
		explicit MaterialHolder(gE::Entity* o)
			: Component(o)
		{

		};

		MaterialHolder(gE::Entity* o, Nullable<const Array<Reference<Material>>> materials)
			: Component(o)
		{
			if(materials) materials->CopyToCArray(_materials);
		}

		NODISCARD ALWAYS_INLINE Material& GetMaterial(u8 i) const { GE_ASSERT(i < GE_MAX_MATERIAL, "MATERIAL OUT OF RANGE"); return _materials[i]; }
		ALWAYS_INLINE void SetMaterial(u8 i, const Reference<Material>& mat) { GE_ASSERT(i < GE_MAX_MATERIAL, "MATERIAL OUT OF RANGE"); _materials[i] = mat; }
		ALWAYS_INLINE void SetMaterial(u8 i, Reference<Material>&& mat) { GE_ASSERT(i < GE_MAX_MATERIAL, "MATERIAL OUT OF RANGE"); _materials[i] = mat; }

		void OnUpdate(float d) override { };
		void OnRender(float d) override { };

		~MaterialHolder() override = default;

	 protected:
	 private:
		Reference<Material> _materials[GE_MAX_MATERIAL] {};
	};
}

