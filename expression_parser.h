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

#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

enum TokenType { TOKEN_EMPTY, TOKEN_NUMBER, TOKEN_OPERATOR, TOKEN_LEFTPAREN, TOKEN_RIGHTPAREN};
enum Operators { OP_ADDITION, OP_SUBTRACTION, OP_MULTIPLICATION, OP_DIVISION };
enum ParseResult { SUCCESS, ERR_UNRECOGNIZED_CHARACTER, ERR_INTEGER_PARSE, ERR_INTEGER_RANGE, ERR_UNMATCHED_PARENTHESIS, ERR_NOT_ENOUGH_OPERANDS, ERR_TRAILING_OPERANDS, ERR_INTERNAL };

struct Token
  {
  long int value;       // value of the token (for numbers and operators)
  enum TokenType type;  // type of the token (empty, number, operator, paren.)
  const char *position; // position in the original expression for error reporting
  };
  
/** Implements the shunting-yard algorithm for parsing expressions
 * 
 * @param expression Input expression to be parsed
 * @param output_queue Output parsed expression as an array of tokens
 * @param err_pos Approximate position of the error (not available for all errors)
 * @return SUCCESS or error code representing the specific error
 */
int parse_expression(const char const * expression, struct Token **output_queue, const char ** err_pos);

#ifdef __cplusplus
}
#endif

#endif /* EXPRESSION_PARSER_H */

