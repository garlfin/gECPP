//
// Created by scion on 10/29/2023.
//

#pragma once

#include <Engine/Entity/Entity.h>
#include <Engine/Manager.h>
#include "Transform.h"

namespace gE
{
	class TransformManager : public TypedManager<Transform>
	{
	 public:
		using TypedManager<Transform>::TypedManager;

		void Register(Transform* t) override;
		void OnUpdate(float delta) override;
	};
}