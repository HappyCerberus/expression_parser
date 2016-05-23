/*
 * File:   simple_parsing.c
 * Author: simon
 *
 * Created on May 22, 2016, 5:20:34 PM
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

void empty_expression()
  {
  const char *end;
  struct Token *parsed;
  int ret = parse_expression("",&parsed,&end);
  CU_ASSERT(ret == SUCCESS);
  CU_ASSERT(parsed != NULL);
  CU_ASSERT(parsed->type == TOKEN_EMPTY);
  CU_ASSERT(parsed->value == 0);
  }

void single_number_expression()
  {
  const char *end;
  struct Token *parsed;
  int ret = parse_expression("12345",&parsed,&end);
  CU_ASSERT(ret == SUCCESS);
  CU_ASSERT(parsed != NULL);
  CU_ASSERT(parsed->type == TOKEN_NUMBER);
  CU_ASSERT(parsed->value == 12345);
  }

void single_number_expression_with_an_error()
  {
  const char *end;
  struct Token *parsed;
  int ret = parse_expression("12x45",&parsed,&end);
  CU_ASSERT(ret == ERR_INTEGER_PARSE);
  CU_ASSERT(*end == 'x');
  }

void signed_number_expression()
  {
  const char *end;
  struct Token *parsed;
  int ret = parse_expression("-12345",&parsed,&end);
  CU_ASSERT(ret == SUCCESS);
  CU_ASSERT(parsed != NULL);
  CU_ASSERT(parsed->type == TOKEN_NUMBER);
  CU_ASSERT(parsed->value == -12345);
  }

int main()
  {
  printf("Running test suite\n");

  CU_pSuite pSuite = NULL;

  /* Initialize the CUnit test registry */
  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  /* Add a suite to the registry */
  pSuite = CU_add_suite("simple_parsing", init_suite, clean_suite);
  if (NULL == pSuite)
    {
    CU_cleanup_registry();
    return CU_get_error();
    }

  /* Add the tests to the suite */
  if ((NULL == CU_add_test(pSuite, "Empty Expression Test", empty_expression)) ||
      (NULL == CU_add_test(pSuite, "Expression with a single number", single_number_expression)) ||
//      (NULL == CU_add_test(pSuite, "Expression with a malformed number", single_number_expression_with_an_error)) ||
      (NULL == CU_add_test(pSuite, "Expression with a signed number", signed_number_expression)))

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
