//
// Created by scion on 2/24/2025.
//

#pragma once

#include "Reflectable.h"

template <class T, size_t SIZE>
std::string EnumData<T, SIZE>::ToString(T t) const
{
   std::string result;
   if(Type == EnumType::Bitfield)
   {
      INT_T remainder = (INT_T) t;

      for(const PAIR_T pair : Enums)
         if((INT_T) t & (INT_T) pair.first)
         {
            result += std::format("{} | ", pair.second);
            remainder -= (INT_T) t;
         }
      if(result.empty()) result = "None";
      else result += std::to_string(remainder);
   }
   else
   {
      typename ARR_T::const_iterator it = At(t);
      if(it == Enums.end()) result = std::to_string((INT_T) t);
      else result = it->second;
   }

   return result;
}

template <class T, size_t SIZE>
typename std::array<std::pair<T, std::string_view>, SIZE>::const_iterator EnumData<T, SIZE>::At(T t) const
{
   return std::find_if(Enums.begin(), Enums.end(), [t](const PAIR_T& pair){ return t == pair.first; });
}
