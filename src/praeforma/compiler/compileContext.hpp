/*************************************************************************
 *
 * FormaB - the bootstrap Forma compiler (compileContext.hpp)
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

#include "util/scopeTracker.hpp"

#include "ast/astBase.hpp"

#include "praeforma/context.hpp"

namespace pre::cc {
class BlockContext;

using BlockCtxPtr = fun::FLinearPtr<BlockContext>;

class CompileContext {
  FPContext *                               m_ctx;
  fun::FScopeTracker<const fps::FASTBase *> m_pos;

public:
  constexpr auto &ctx() const { return *m_ctx; }
  constexpr auto &fiCtx() const { return m_ctx->fiCtx(); }
  constexpr auto &pos() { return m_pos; }

  CompileContext(FPContext *ctx, const fps::FASTBase *pos) :
      m_ctx(ctx),
      m_pos(pos) {}

  template <typename... TArgs>
  [[nodiscard]] BlockCtxPtr block(TArgs &&... args) {
    return flinear<BlockContext>(this,
                                 fiCtx().block(std::forward<TArgs>(args)...));
  }
};
} // namespace pre::cc
