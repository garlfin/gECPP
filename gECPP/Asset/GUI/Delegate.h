
#pragma once

#include <Prototype.h>

#define UI_DELEGATE(NAME, TYPE, FUNC) \
	class NAME final : public gETF::UI::Delegate<TYPE> \
	{ \
 	 public: \
		explicit NAME(TYPE* entity) : gETF::UI::Delegate<TYPE>(entity) {}; \
		void OnCall(gETF::UI::Element*) override FUNC; \
	}

namespace gETF::UI
{
	class IDelegate
	{
	 public:
		explicit IDelegate(gE::Entity* entity) : Entity(entity) {};

		GET_CONST(gE::Entity*, Entity, Entity);

		virtual void OnCall(IElement*) = 0;

		virtual ~IDelegate() = default;

	 protected:
		gE::Entity* Entity;
	};

	template<class T>
	class Delegate : public IDelegate
	{
	 public:
		explicit Delegate(T* t) : IDelegate(t) {};

		GET_CONST(T*, Entity, (T*) Entity);

		~Delegate() override = default;
	};
}
