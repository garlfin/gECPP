//
// Created by scion on 10/31/2023.
//

#pragma once

#include "GL/Binary/Binary.h"

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
		GET_CONST(bool, IsFirst, _isFirst);

		u8 TickOffset = 0;
		u8 TickSkip = 1;

		NODISCARD bool Tick(float delta)
		{
			_isFirst = false;
			_frame++;

			return _isFirst || !TickSkip ||                                      // first frame or no skip
				   _frame >= TickOffset && (_frame - TickOffset) % TickSkip == 0; // equal to tick
		}

	 private:
		bool _isFirst = true; // padded anyways, no need for bitfield
		u64 _frame = 0;
	};

	CONSTEXPR_GLOBAL CameraTiming DefaultCameraTiming(0, 1);
}