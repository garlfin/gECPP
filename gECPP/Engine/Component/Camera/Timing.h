//
// Created by scion on 10/31/2023.
//

#pragma once

#include <GL/Binary/Binary.h>

namespace gE
{
	struct CameraTiming
	{
	 public:
		constexpr CameraTiming(const CameraTiming& t) : TickOffset(t.TickOffset), TickSkip(t.TickSkip) { };
		constexpr CameraTiming(u8 offset, u8 skip) : TickOffset(offset), TickSkip(skip) { };
		constexpr CameraTiming() = default;

		OPERATOR_EQUALS(CameraTiming);

		GET_CONST(u64, Frame, _frame);
		GET_CONST(bool, IsFirst, !_frame);

		u8 TickOffset = 0;
		u8 TickSkip = 1;

		NODISCARD bool Tick(float delta)
		{
			bool v = !_frame || TickSkip &&                                          // first frame or no skip
					  _frame >= TickOffset && (_frame - TickOffset) % TickSkip == 0; // equal to tick
			 _frame++;
			return v;
		}

	 private:
		u64 _frame = 0;
	};

	CONSTEXPR_GLOBAL CameraTiming DefaultCameraTiming(0, 1);
}