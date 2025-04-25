//
// Created by scion on 7/15/2024.
//

#pragma once

#include <Core/Math/Math.h>

inline float bit_sign(u8 b) { return b ? 1 : -1; }

namespace gE
{
	template<Dimension DIMENSION>
	struct AABB
	{
		typedef Position<DIMENSION> POS_T;
		typedef mat<(u8) DIMENSION + 1, (u8) DIMENSION + 1, float> MAT_T;
		typedef Position<(Dimension) ((u8) DIMENSION + 1)> TEMP_T;

		POS_T Min;
		POS_T Max;

		inline POS_T Center() const { return (Min + Max) / 2.f; }
		inline POS_T Extent() const { return abs(Max - Center()); }

		inline AABB& operator*=(const MAT_T& mat) { AABB b = *this * mat; return *this = b; }
		AABB operator*(const MAT_T& mat) const;

		inline AABB& operator&=(const AABB& o) { AABB b = *this & o; return *this = b; }
		AABB operator&(const AABB& o) const;
	};

	template<Dimension DIMENSION>
	AABB<DIMENSION> AABB<DIMENSION>::operator*(const MAT_T& mat) const
	{
		POS_T min(FLT_MAX), max(-FLT_MAX);

		const POS_T center = Center(),
					extent = Extent();

		for(int i = 0; i < 8; i++)
		{
			POS_T sign = POS_T(bit_sign(i & 1), bit_sign(i & 2), bit_sign(i & 4));
			TEMP_T temp = TEMP_T(center + extent * sign, 1.f);

			temp = mat * temp;

			min = glm::min(min, POS_T(temp));
			max = glm::max(max, POS_T(temp));
		}

		return { min, max };
	}

	template<Dimension DIMENSION>
	AABB<DIMENSION> AABB<DIMENSION>::operator&(const AABB& o) const
	{
		return { min(Min, o.Min), max(Max, o.Max) };
	}
}