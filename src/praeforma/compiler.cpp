/*************************************************************************
 *
 * FormaB - the bootstrap Forma compiler (compiler.cpp)
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

#include "compiler.hpp"

#include "compiler/context.hpp"

namespace pre {
cc::RegResult FPCompiler::emitStore(cc::BlockCtxPtr    ctx,
                                    const fps::FPExpr *node) const {
  auto _(ctx->pos().move(node));

  switch (node->alt()) {
  case fps::FPExpr::Infix: return emitStore(ctx.move(), node->infix());
  case fps::FPExpr::Control: return emitStore(ctx.move(), node->ctl());
  }
}

cc::RegResult FPCompiler::emitStore(cc::BlockCtxPtr      ctx,
                                    const fps::FPXInfix *node) const {
  auto _(ctx->pos().move(node));

  switch (node->alt()) {
  case fps::FPXInfix::Add:
    return makeMsg(
        ctx.move(), "add", "o@op:+:", node->infixl(), node->infixr());
  case fps::FPXInfix::Sub:
    return makeMsg(
        ctx.move(), "sub", "o@op:-:", node->infixl(), node->infixr());
  case fps::FPXInfix::Mul:
    return makeMsg(
        ctx.move(), "mul", "o@op:*:", node->infixl(), node->infixr());
  case fps::FPXInfix::Div:
    return makeMsg(
        ctx.move(), "div", "o@op:/:", node->infixl(), node->infixr());
  case fps::FPXInfix::Mod:
    return makeMsg(ctx.move(), "mod", "o@op:%:", node->infixl(), node->unary());
  case fps::FPXInfix::Unary: return emitStore(ctx.move(), node->unary());
  }
}

cc::RegResult FPCompiler::emitStore(cc::BlockCtxPtr      ctx,
                                    const fps::FPXUnary *node) const {
  auto _(ctx->pos().move(node));
  return emitStore(ctx.move(), node->prim());
}

cc::RegResult FPCompiler::emitStore(cc::BlockCtxPtr     ctx,
                                    const fps::FPXPrim *node) const {
  auto _(ctx->pos().move(node));

  switch (node->alt()) {
  case fps::FPXPrim::Ident: ctx->errorR("ident not implemented");
  case fps::FPXPrim::Number: return makeNumeric(ctx.move(), node->tok());
  case fps::FPXPrim::True: return ctx->store<fie::FIBoolConst>("true", true);
  case fps::FPXPrim::False: return ctx->store<fie::FIBoolConst>("false", false);
  case fps::FPXPrim::Paren: return emitStore(ctx.move(), node->paren());
  }
}

cc::RegResult FPCompiler::emitStore(cc::BlockCtxPtr      ctx,
                                    const fps::FPXParen *node) const {
  auto _(ctx->pos().move(node));

  return emitStore(ctx.move(), node->expr());
}

cc::RegResult FPCompiler::emitStore(cc::BlockCtxPtr        ctx,
                                    const fps::FPXControl *node) const {
  auto _(ctx->pos().move(node));

  auto [ctx2, cond] = emitStore(ctx.move(), node->cond());

  auto ctxThen = ctx2->newBlock("xi-then");
  auto ctxElse = ctx2->newBlock("xi-else");
  auto ctxDone = ctx2->newBlock("xi-done");

  ctx2->contBranch(cond, ctxThen->block(), ctxElse->block());

  ctxThen->contStatic(ctxDone->block());
  ctxElse->contStatic(ctxDone->block());

  auto [ctxThen2, then] = emitStore(ctxThen.move(), node->then());

  auto [ctxElse2, Else] = emitStore(ctxElse.move(), node->Else());

  return ctxDone->store<fie::FIPhiValue>("if",
                                         std::vector<fie::FIRegId>{then, Else});
}

fie::FIGlobalConstant *FPCompiler::compileDAssign(
    const fps::FPDAssign *assign) {

  cc::FuncContext fctx(m_ctx, assign);

  auto ctx = fctx.block("root");

  auto [ctx2, value] = emitStore(ctx.move(), assign->value());

  ctx2->contRet(value);

  return m_ctx->fiCtx().globalConstant(assign->name()->value(),
                                       fie::FIFunctionBody(fctx.blocks()));
}
} // namespace pre