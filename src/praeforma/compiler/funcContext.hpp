/*************************************************************************
 *
 * FormaB - the bootstrap Forma compiler (funcContext.hpp)
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

#include "compileContext.hpp"

namespace pre::cc {
class FuncContext : public CompileContext {
  std::vector<fie::FIBlock *> m_blocks;
  std::uint32_t               m_nextReg = 0;

  fie::FIRegId regId(const std::string &name) {
    return fie::FIRegId(m_nextReg++, name);
  }

public:
  constexpr auto &blocks() const { return m_blocks; }

  FuncContext(FPContext *ctx, const fps::FASTBase *pos) :
      CompileContext(ctx, pos) {}

  template <typename... TArgs>
  [[nodiscard]] BlockCtxPtr block(TArgs &&... args) {
    auto *block = fiCtx().block(std::forward<TArgs>(args)...);

    m_blocks.push_back(block);

    return flinear<BlockContext>(this, block);
  }

  friend class BlockContext;
};
} // namespace pre::cc