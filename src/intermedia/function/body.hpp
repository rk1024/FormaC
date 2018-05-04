/*************************************************************************
 *
 * FormaB - the bootstrap Forma compiler (body.hpp)
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

#include <cassert>
#include <vector>

#include "diagnostic/location.hpp"

#include "block.hpp"

namespace fie {
class FIContext;

class FIFunctionBody {
  fdi::FLocation         m_loc;
  FIBlock *              m_entry;
  std::vector<FIBlock *> m_blocks;

public:
  constexpr auto &loc() const { return m_loc; }
  constexpr auto &entry() const { return m_entry; }
  constexpr auto &blocks() { return m_blocks; }
  constexpr auto &blocks() const { return m_blocks; }

  explicit FIFunctionBody(const fdi::FLocation &       loc,
                          FIBlock *                    entry,
                          const std::vector<FIBlock *> blocks) :
      m_loc(loc),
      m_entry(entry),
      m_blocks(blocks) {
#if !defined(NDEBUG)
    for (auto &blk : blocks) {
      if (blk == entry) goto good;
    }
    assert(false);
  good:;
#endif
  }

  FIValue *eval(FIContext &) const;
};
} // namespace fie
