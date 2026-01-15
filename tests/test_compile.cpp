/*
 * test_compile.cpp - Compilation Test for df_game_r Restoration
 *
 * This file contains a subset of functions to test if the compilation
 * environment is correctly set up.
 */

#include <math.h>
#include "include/defs.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* ========================================================================
 * Test 1: Simple Function (CUser::GetUID)
 * ======================================================================== */

class CUser;

//----- (080C8C96) --------------------------------------------------------
int __cdecl CUser::GetUID(CUser *this)
{
  return *((unsigned __int16 *)this + 229971) | (*((__int16 *)this + 229970) << 16);
}


/* ========================================================================
 * Test 2: Boolean Function
 * ======================================================================== */

//----- (0807E9E4) --------------------------------------------------------
bool __cdecl comp_by_time(_DWORD *a1, _DWORD *a2)
{
  return *a1 > *a2;
}


/* ========================================================================
 * Test 3: Base64 Check Function
 * ======================================================================== */

//----- (0807E9F8) --------------------------------------------------------
_BOOL4 __cdecl is_base64(unsigned __int8 a1)
{
  return isalnum(a1) || a1 == 43 || a1 == 47;
}


/* ========================================================================
 * Test 4: MySQL Class Constructor
 * ======================================================================== */

class MySQL {
public:
  _DWORD field0;
  _DWORD connection;
  char padding[270470]; // Large padding to match offset 270474
  _BYTE is_connected;
};

//----- (083F3A9E) --------------------------------------------------------
void __cdecl MySQL::MySQL(MySQL *this)
{
  *((_DWORD *)this + 1) = 0;
  *(_DWORD *)this = 0;
  *((_BYTE *)this + 270474) = 0;
}


/* ========================================================================
 * Test 5: Namespace Function (charac_expand::CData)
 * ======================================================================== */

namespace charac_expand {
  class CData {
  public:
    _DWORD vtable;
  };
}

// Forward declaration for vtable
extern _DWORD (__cdecl *off_8B26868)(charac_expand::CData *__hidden this);

//----- (080C8CBA) --------------------------------------------------------
charac_expand::CData *__cdecl charac_expand::CData::CData(charac_expand::CData *this)
{
  *(_DWORD *)this = &off_8B26868;
  return this;
}


/* ========================================================================
 * Test 6: Complex Return Type (__PAIR64__)
 * ======================================================================== */

// Simulated function using __PAIR64__
int test_pair_function(int high, int low)
{
  _QWORD result = __PAIR64__(high, low);
  return (int)result;
}


/* ========================================================================
 * Test 7: Various Type Usage
 * ======================================================================== */

void test_types(void)
{
  _BYTE  b = 0xFF;
  _WORD  w = 0xFFFF;
  _DWORD d = 0xFFFFFFFF;
  _QWORD q = 0xFFFFFFFFFFFFFFFF;

  _BOOL4 bool_val = 1;

  __int8  i8 = -128;
  __int16 i16 = -32768;
  __int32 i32 = -2147483648;
  __int64 i64 = -9223372036854775807LL;

  printf("Types test: %u %u %u %llu %d\n", b, w, d, q, bool_val);
}


/* ========================================================================
 * Main Function for Testing
 * ======================================================================== */

int main(int argc, char **argv)
{
  printf("=== df_game_r Compilation Test ===\n\n");

  // Test 1: Type definitions
  printf("[TEST 1] Type definitions\n");
  printf("  sizeof(_BYTE)  = %zu (expected: 1)\n", sizeof(_BYTE));
  printf("  sizeof(_WORD)  = %zu (expected: 2)\n", sizeof(_WORD));
  printf("  sizeof(_DWORD) = %zu (expected: 4)\n", sizeof(_DWORD));
  printf("  sizeof(_QWORD) = %zu (expected: 8)\n", sizeof(_QWORD));
  printf("  sizeof(_BOOL4) = %zu (expected: 4)\n", sizeof(_BOOL4));

  // Test 2: Boolean function
  printf("\n[TEST 2] Boolean function\n");
  _DWORD arr1[] = {10, 20};
  _DWORD arr2[] = {5, 15};
  bool result = comp_by_time(arr1, arr2);
  printf("  comp_by_time({10, 20}, {5, 15}) = %s\n", result ? "true" : "false");

  // Test 3: Base64 check
  printf("\n[TEST 3] Base64 validation\n");
  printf("  is_base64('A') = %d\n", is_base64('A'));
  printf("  is_base64('+') = %d\n", is_base64('+'));
  printf("  is_base64('@') = %d\n", is_base64('@'));

  // Test 4: __PAIR64__ macro
  printf("\n[TEST 4] __PAIR64__ macro\n");
  _QWORD paired = __PAIR64__(0x12345678, 0xABCDEF00);
  printf("  __PAIR64__(0x12345678, 0xABCDEF00) = 0x%llx\n", paired);

  // Test 5: MySQL class size
  printf("\n[TEST 5] MySQL class size\n");
  printf("  sizeof(MySQL) = %zu (expected: >= 270475)\n", sizeof(MySQL));

  // Test 6: Type usage
  printf("\n[TEST 6] Type usage\n");
  test_types();

  printf("\n=== All tests completed ===\n");
  return 0;
}
