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
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

// parse a number from the input string
static int parse_number(const char const *head, const char const **tail, struct Token *output)
  {
  char *end = NULL;

  long int result = strtol(head,&end,10);

  // parsing error;
  if (end == head)
    return ERR_INTEGER_PARSE;

  // out of range value
  if ((result == LONG_MAX || result == LONG_MIN) && errno == ERANGE)
    return ERR_INTEGER_RANGE;

  // we have a valid number
  if (output != NULL)
    {
    output->value = result;
    output->type = TOKEN_NUMBER;
    output->position = head;
    }

  if (tail != NULL)
    // update the tail
    *tail = end;

  return SUCCESS;
  }

static void copy_token(struct Token *src, struct Token *dst)
  {
  dst->position = src->position;
  dst->type = src->type;
  dst->value = src->value;
  }

// parse the expression using the shunting-yard algorithm
int parse_expression(const char const * expression, struct Token **output_queue, const char ** err_pos)
  {
  int            ret        = SUCCESS;
  const char    *parse_head = expression;
  size_t         expr_len   = strlen(expression);

  // This algorithm needs two data structures a temporary stack and an output
  // queue. Since we know that at most the expression has a unique token as each
  // of the characters, we allocate both to strlen(expression)+1.
  struct Token *tmp_queue, *queue_tail;
  struct Token *tmp_stack, *stack_tail;

  tmp_queue = calloc(expr_len+1,sizeof(struct Token));
  if (tmp_queue == NULL)
    return ERR_INTERNAL;

  tmp_stack = calloc(expr_len+1,sizeof(struct Token));
  if (tmp_stack == NULL)
    {
    free(tmp_queue);
    return ERR_INTERNAL;
    }

  queue_tail = tmp_queue;
  stack_tail = tmp_stack;

  bool last_tok_was_number = false;

  while (true)
    {
    if (*parse_head == '\0')
      {
      // end of string - jump to post-processing
      break;
      }

    if (isspace(*parse_head))
      {
      // skip any whitespace
      ++parse_head;
      continue;
      }

    // a number either starts with a digit or +/- followed by a digit
    if (isdigit(*parse_head) ||
        (!last_tok_was_number && (*parse_head == '+' || *parse_head == '-')
                              && isdigit(parse_head[1])))
      {
      const char *tail = NULL;
      ret = parse_number(parse_head,&tail,queue_tail);
      if (ret == SUCCESS)
        // a number goes directly to output queue
        {
        ++queue_tail;
        parse_head = tail;
        }
      else
        {
        if (err_pos != NULL)
          *err_pos = parse_head;
        goto parse_err;
        }
      last_tok_was_number = true;
      continue;
      }

    last_tok_was_number = false;

    // left parenthesis is simply added to the stack
    if (*parse_head == '(')
      {
      stack_tail->position = parse_head;
      stack_tail->type = TOKEN_LEFTPAREN;
      stack_tail->value = 0;
      ++stack_tail;
      ++parse_head;
      continue;
      }

    // when we encounter the right parenthesis, we flip stack into queue until
    // we encounter left parenthesis, if we don't we have mismatched parenthesis
    if (*parse_head == ')')
      {
      while (stack_tail != tmp_stack && stack_tail[-1].type != TOKEN_LEFTPAREN)
        {
        copy_token(&stack_tail[-1],queue_tail);
        --stack_tail;
        ++queue_tail;
        }

      if (stack_tail == tmp_stack)
        {
        ret = ERR_UNMATCHED_PARENTHESIS;
        if (err_pos != NULL)
          *err_pos = parse_head;
        goto parse_err;
        }

      // remove the left paren from the stack
      --stack_tail;
      ++parse_head;
      continue;
      }

    long int op_value;
    // mapping from Operators->operator priority
    int op_prio[] = { 0, 0, 1, 1 };

    // we are expecting an operator or left/right paren
    switch (*parse_head)
      {
      case '+':
        op_value = OP_ADDITION;
        break;
      case '-':
        op_value = OP_SUBTRACTION;
        break;
      case '*':
        op_value = OP_MULTIPLICATION;
        break;
      case '/':
        op_value = OP_DIVISION;
        break;
      default:
        // unrecognized character
        if (err_pos != NULL)
          *err_pos = parse_head;
        ret = ERR_UNRECOGNIZED_CHARACTER;
        goto parse_err;
      }

    // now we process the operator

    // while the stack is not empty and the
    // priority of the operator on the stack >= priority of current operator
    // move operator from stack to queue
    while (stack_tail != tmp_stack)
      {
      if (stack_tail[-1].type == TOKEN_OPERATOR &&
          op_prio[stack_tail[-1].value] >= op_prio[op_value])
        {
        copy_token(&stack_tail[-1],queue_tail);
        --stack_tail;
        ++queue_tail;
        }
      else
        {
        break;
        }
      }

      // now put the current operator on the stack
      stack_tail->type = TOKEN_OPERATOR;
      stack_tail->value = op_value;
      stack_tail->position = parse_head;
      ++stack_tail;
      ++parse_head;
    }

  // we are done, now rewind the stack onto the queue
  while (stack_tail != tmp_stack)
    {
    --stack_tail;
    if (stack_tail->type == TOKEN_LEFTPAREN)
      {
      ret = ERR_UNMATCHED_PARENTHESIS;
      if (err_pos != NULL)
        *err_pos = stack_tail->position;
      goto parse_err;
      }
    else
      {
      copy_token(stack_tail,queue_tail);
      ++queue_tail;
      }
    }

  free(tmp_stack);
  if (output_queue != NULL)
    *output_queue = tmp_queue;
  return SUCCESS;

parse_err:
    free(tmp_queue);
    free(tmp_stack);
    return ret;
  }