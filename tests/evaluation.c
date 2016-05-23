/*
 * File:   evaluation.c
 * Author: simon
 *
 * Created on May 23, 2016, 12:30:21 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>

#include "../expression_evaluator.h"
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

void test1()
  {
  const char *end;
  struct Token *parsed;
  long int result;
  int ret = parse_expression("12+13-10",&parsed,&end);
  ret = evaluate_expression(parsed,&result,&end);
  CU_ASSERT(ret == SUCCESS);
  CU_ASSERT(result == 15);
  }

void test2()
  {
  const char *end;
  struct Token *parsed;
  long int result;
  int ret = parse_expression("12+13/10",&parsed,&end);
  ret = evaluate_expression(parsed,&result,&end);
  CU_ASSERT(ret == SUCCESS);
  CU_ASSERT(result == 13);
  }

void test3()
  {
  const char *end;
  struct Token *parsed;
  long int result;
  int ret = parse_expression("(1+2)*3",&parsed,&end);
  ret = evaluate_expression(parsed,&result,&end);
  CU_ASSERT(ret == SUCCESS);
  CU_ASSERT(result == 9);
  }

void test4()
  {
  const char *end;
  struct Token *parsed;
  long int result;
  int ret = parse_expression("(1+2)*3+2*3+3",&parsed,&end);
  ret = evaluate_expression(parsed,&result,&end);
  CU_ASSERT(ret == SUCCESS);
  CU_ASSERT(result == 18);
  }

int main()
  {
  CU_pSuite pSuite = NULL;

  /* Initialize the CUnit test registry */
  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  /* Add a suite to the registry */
  pSuite = CU_add_suite("evaluation", init_suite, clean_suite);
  if (NULL == pSuite)
    {
    CU_cleanup_registry();
    return CU_get_error();
    }

  /* Add the tests to the suite */
  if ((NULL == CU_add_test(pSuite, "test1", test1)) ||
      (NULL == CU_add_test(pSuite, "test2", test2)) ||
      (NULL == CU_add_test(pSuite, "test3", test3)) ||
      (NULL == CU_add_test(pSuite, "test4", test4)) )
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
