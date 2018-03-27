/*************************************************************************
 *
 * FormaB - the bootstrap Forma compiler (type.hpp)
 * Copyright (C) 2017-2018 Ryan Schroeder, Colin Unger
 *
 * FormaB is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * FormaB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with FormaB.  If not, see <https://www.gnu.org/licenses/>.
 *
 ************************************************************************/

#pragma once

#include "_type.hpp"
#include "types.hpp"

namespace w {
template <>
struct Types<fun::FPtr<const TypeBase>> {
  static std::unordered_set<std::string> __ftv(
      const fun::FPtr<const TypeBase> &);

  static fun::FPtr<const TypeBase> __sub(const Subst &,
                                         const fun::FPtr<const TypeBase> &);
};
} // namespace w