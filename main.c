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

/* Interactive mathematical expression evaluator
 *
 * The program works in two stages
 * 1] parse the expression using the shunting-yard algorithm
 *    https://en.wikipedia.org/wiki/Shunting-yard_algorithm
 * 2] evaluate the parsed expression
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "expression_parser.h"
#include "expression_evaluator.h"

const char * err_txt[] = {
  "SUCCESS",
  "Error: Unrecognized character",
  "Error: Couldn't parse integer",
  "Error: Integer out of range",
  "Error: Unmatched parenthesis",
  "Error: Not enough operands for operator",
  "Error: Trailing operands",
  "Error: Internal problem"
  };

// pretty print the error
void translate_error(int result, char *data, const char *pos)
  {
  char *endline;
  if ((endline=strchr(data, '\n')) != NULL)
    *endline = '\0';

  printf("%s\n",err_txt[result]);
  printf("\t%s\n",data);
  if (pos != NULL)
    {
    printf("\t%*s\n",(int)(pos-data+1),"^");
    }
  }

int main(void)
  {
  ssize_t read;
  size_t len = 0;
  char *line = NULL;

  // until we reach end of input, we process each line and report possible errors
  while ((read = getline(&line,&len,stdin)) != -1)
    {
    struct Token *parsed_expr = NULL;
    const char *end;
    int ret = parse_expression(line,&parsed_expr,&end);

    if (ret != SUCCESS)
      {
      translate_error(ret,line,end);
      free(parsed_expr);
      continue;
      }

    long int result;
    ret = evaluate_expression(parsed_expr,&result,&end);

    if (ret != SUCCESS)
      {
      translate_error(ret,line,end);
      free(parsed_expr);
      continue;
      }

    printf("Result is %ld\n",result);

    free(parsed_expr);
    }

  free(line);
  
  return (EXIT_SUCCESS);
  }

