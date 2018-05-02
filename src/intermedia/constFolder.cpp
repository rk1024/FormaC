/*************************************************************************
 *
 * FormaB - the bootstrap Forma compiler (constFolder.cpp)
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

#include "constFolder.hpp"

namespace fie {
FIFoldedConst *FIConstFolder::foldConstant(FIConst *Const) {
  return m_ctx->foldedConst(Const->name(),
                            m_ctx->val<FIDoubleConstValue>(Const->body().loc(),
                                                           1337.1337));
}
} // namespace fie
