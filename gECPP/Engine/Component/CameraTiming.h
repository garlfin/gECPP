//
// Created by scion on 10/31/2023.
//

#pragma once

#include "GL/Binary.h"

namespace gE
{
	struct CameraTiming
	{
	 public:
		inline constexpr CameraTiming(u8 fps, u8 offset, u8 skip) : FPS(fps), TickOffset(offset), TickSkip(skip) {};

		constexpr CameraTiming() = default;
		CameraTiming(const CameraTiming& t) : FPS(t.FPS), TickOffset(t.TickOffset), TickSkip(t.TickSkip) {};

		OPERATOR_EQUALS(CameraTiming);

		GET_CONST_VALUE(u64, Frame, _frame);
		GET_CONST_VALUE(float, Time, _time);

		u8 FPS = 144;
		u8 TickOffset = 0;
		u8 TickSkip = 1;

		NODISCARD bool Tick(float delta)
		{
			const float oneOverFPS = 1.0f / FPS;
			const bool firstFrame = _time == 0 && !_frame;

			_time += delta;

			if (FPS == 0 || _time >= oneOverFPS)
			{
				_time = 0;
				_frame++;
			} else return false;

			return firstFrame || !TickSkip || 									  // first frame or no skip
				   _frame >= TickOffset && (_frame - TickOffset) % TickSkip == 0; // equal to tick
		}

	 private:
		u64 _frame = 0;
		float _time = 0;
	};
}