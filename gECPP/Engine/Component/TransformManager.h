//
// Created by scion on 10/29/2023.
//

#pragma once

#include <Engine/Entity/Entity.h>
#include <Engine/Manager.h>
#include "Transform.h"

namespace gE
{
	class TransformManager : public Manager<Transform>
	{
	 public:
		using Manager<Transform>::Manager;

		void Register(Transform* t) override;
		void OnUpdate(float delta) override;
	};
}