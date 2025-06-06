//
// Created by scion on 10/31/2023.
//

#pragma once

namespace gE
{
	struct CameraTiming
	{
	 public:
		constexpr CameraTiming(u8 offset, u8 skip) : TickOffset(offset), TickSkip(skip) { };
		constexpr CameraTiming() = default;

		GET_CONST(u64, Frame, _frame);
		GET_CONST(bool, IsFirst, !_frame);

		u8 TickOffset = 0;
		u8 TickSkip = 0;

		NODISCARD bool Tick()
		{
			bool tick = !_frame || // first frame
						(_frame >= TickOffset &&
						(_frame - TickOffset) % (TickSkip + 1) == 0); // equal to tick
			_frame++;
			return tick;
		}

	 private:
		u64 _frame = 0;
	};
}
