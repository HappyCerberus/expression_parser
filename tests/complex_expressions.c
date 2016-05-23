/*
 * File:   complex_expressions.c
 * Author: simon
 *
 * Created on May 23, 2016, 11:01:33 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>

#include "../expression_parser.h"

/*
 * CUnit Test Suite
 */

int init_suite(void)
  {
  return 0;
  }

int clean_suite(void)
  {
  return 0;
  }

void simple_addition()
  {
  const char *end;
  struct Token *parsed;
  int ret = parse_expression("12+13",&parsed,&end);
  CU_ASSERT(ret == SUCCESS);
  CU_ASSERT(parsed != NULL);

  CU_ASSERT(parsed[0].type == TOKEN_NUMBER);
  CU_ASSERT(parsed[0].value == 12);
  CU_ASSERT(parsed[1].type == TOKEN_NUMBER);
  CU_ASSERT(parsed[1].value == 13);
  CU_ASSERT(parsed[2].type == TOKEN_OPERATOR);
  CU_ASSERT(parsed[2].value == OP_ADDITION);
  }

void combined_expression()
  {
  const char *end;
  struct Token *parsed;
  int ret = parse_expression("12+13-10",&parsed,&end);
  CU_ASSERT(ret == SUCCESS);
  CU_ASSERT(parsed != NULL);

  CU_ASSERT(parsed[0].type == TOKEN_NUMBER);
  CU_ASSERT(parsed[0].value == 12);
  CU_ASSERT(parsed[1].type == TOKEN_NUMBER);
  CU_ASSERT(parsed[1].value == 13);
  CU_ASSERT(parsed[2].type == TOKEN_OPERATOR);
  CU_ASSERT(parsed[2].value == OP_ADDITION);
  CU_ASSERT(parsed[3].type == TOKEN_NUMBER);
  CU_ASSERT(parsed[3].value == 10);
  CU_ASSERT(parsed[4].type == TOKEN_OPERATOR);
  CU_ASSERT(parsed[4].value == OP_SUBTRACTION);
  }

void operator_priority()
  {
  const char *end;
  struct Token *parsed;
  int ret = parse_expression("12+13*10",&parsed,&end);
  CU_ASSERT(ret == SUCCESS);
  CU_ASSERT(parsed != NULL);

  CU_ASSERT(parsed[0].type == TOKEN_NUMBER);
  CU_ASSERT(parsed[0].value == 12);
  CU_ASSERT(parsed[1].type == TOKEN_NUMBER);
  CU_ASSERT(parsed[1].value == 13);
  CU_ASSERT(parsed[2].type == TOKEN_NUMBER);
  CU_ASSERT(parsed[2].value == 10);
  CU_ASSERT(parsed[3].type == TOKEN_OPERATOR);
  CU_ASSERT(parsed[3].value == OP_MULTIPLICATION);
  CU_ASSERT(parsed[4].type == TOKEN_OPERATOR);
  CU_ASSERT(parsed[4].value == OP_ADDITION);
  }

int main()
  {
  CU_pSuite pSuite = NULL;

  /* Initialize the CUnit test registry */
  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  /* Add a suite to the registry */
  pSuite = CU_add_suite("complex_expressions", init_suite, clean_suite);
  if (NULL == pSuite)
    {
    CU_cleanup_registry();
    return CU_get_error();
    }

  /* Add the tests to the suite */
  if ((NULL == CU_add_test(pSuite, "Simple addition parsing", simple_addition)) ||
      (NULL == CU_add_test(pSuite, "Combined expression parsing", combined_expression)) ||
      (NULL == CU_add_test(pSuite, "Operator priority parsing", operator_priority)))
    {
    CU_cleanup_registry();
    return CU_get_error();
    }

  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
  }
