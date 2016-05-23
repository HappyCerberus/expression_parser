/*
 * Copyright (c) 2016 Simon Toth (kontakt@simontoth.cz)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "expression_parser.h"
#include <string.h>
#include <stdlib.h>

int evaluate_expression(struct Token *token_array, long int *value, const char ** err_pos)
  {
  // token_array is expected in a postfix format, so we do a simple stack-based
  // postfix evaluation:
  // - numbers are simply put onto stack
  // - operators pull the required number of operands from the stack and
  //   put the result onto the stack
  // - at the end, we should have a single value in the stack, that is our result

  // determine the length of the array
  struct Token *head = token_array;
  while (head->type != TOKEN_EMPTY)
    ++head;

  size_t token_len = head-token_array;

  // create the stack of the required size
  struct Token *stack = calloc(token_len+1,sizeof(struct Token));
  if (stack == NULL)
    return ERR_INTERNAL;

  struct Token *stack_tail = stack;

  head = token_array;

  while (head->type != TOKEN_EMPTY)
    {
    // numbers are simply added to the stack
    if (head->type == TOKEN_NUMBER)
      {
      stack_tail->position = head->position;
      stack_tail->value = head->value;
      ++stack_tail;
      ++head;
      continue;
      }

    // operators are evaluated
    if (stack_tail-stack < 2) // we do not have two operands
      {
      *err_pos = head->position;
      free(stack);
      return ERR_NOT_ENOUGH_OPERANDS;
      }

    long int rhs = stack_tail[-1].value;
    --stack_tail;
    long int lhs = stack_tail[-1].value;
    --stack_tail;

    // we should properly check overflow here
    switch(head->value)
      {
      case OP_ADDITION:
        stack_tail->value = lhs+rhs;
        stack_tail->position = NULL;
        break;
      case OP_SUBTRACTION:
        stack_tail->value = lhs-rhs;
        stack_tail->position = NULL;
        break;
      case OP_MULTIPLICATION:
        stack_tail->value = lhs*rhs;
        stack_tail->position = NULL;
        break;
      case OP_DIVISION:
        stack_tail->value = lhs/rhs;
        stack_tail->position = NULL;
        break;
      }

      ++stack_tail;
      ++head;
    }

  if (stack_tail-stack != 1)
    {
    *err_pos = stack->position;
    free(stack);
    return ERR_TRAILING_OPERANDS;
    }

  *value = stack->value;

  free(stack);

  return SUCCESS;
  }
