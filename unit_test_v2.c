/*
 * Unit Test Bootstrap
 * Autor: Tomasz Jaworski, 2018-2020
 *
 * Test dla zadania Projekt: Ocena 3,0
 * Autor testowanej odpowiedzi: Tymur Huselnykov
 * Test wygenerowano automatycznie o 2023-12-29 13:07:29.656531
 *
 * Debug: 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <setjmp.h>
#include <assert.h>

#if !defined(__clang__) && !defined(__GNUC__)
// Zakomentuj poniższy błąd, jeżeli chcesz przetestować testy na swoim kompilatorze C.
#error System testow jednostkowych jest przeznaczony dla kompilatorów GCC/Clang.
#endif

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
// Zakomentuj poniższy błąd, jeżeli chcesz przetestować testy na platformie Windows.
#error System testow jednostkowych NIE jest przeznaczony dla testów uruchamianych na platformach Windows.
#endif

#define _RLDEBUG_API_
#include "unit_helper_v2.h"
#include "rdebug.h"

#include "tested_declarations.h"
#include "rdebug.h"

//
// Elementy globalne dla całego testu
//




//
//  Test 1: Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku przekazania do niej wartości 0
//
void UTEST1(void)
{
    // informacje o teście
    test_start(1, "Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku przekazania do niej wartości 0", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);
                
                void *ptr = heap_malloc(0);
                
                test_error(ptr == NULL, "Funkcja heap_malloc() powinna zwrócić wartość NULL");

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                        
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 2: Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni większej niż przestrzeń obecna na stercie
//
void UTEST2(void)
{
    // informacje o teście
    test_start(2, "Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni większej niż przestrzeń obecna na stercie", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr = heap_malloc(67108862);

                test_error(ptr == NULL, "Funkcja heap_malloc() powinna zwrócić wartość NULL");

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                        
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 3: Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku pierwszej próby zaalokowania przestrzeni mniejszej niż przestrzeń obecna na stercie
//
void UTEST3(void)
{
    // informacje o teście
    test_start(3, "Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku pierwszej próby zaalokowania przestrzeni mniejszej niż przestrzeń obecna na stercie", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr = heap_malloc(637);

                test_error(ptr != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                heap_clean();
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 4: Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni mniejszej niż przestrzeń obecna na stercie, w przypadku przydzielenia wcześniej pamięci użytkownikowi
//
void UTEST4(void)
{
    // informacje o teście
    test_start(4, "Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni mniejszej niż przestrzeń obecna na stercie, w przypadku przydzielenia wcześniej pamięci użytkownikowi", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr1 = heap_malloc(54);
                test_error(ptr1 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));
                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr2 = heap_malloc(147);
                test_error(ptr2 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr2), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr2));
                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                heap_clean();
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 5: Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni mniejszej niż przestrzeń obecna na stercie (żądana pamięć wymaga zwiększenia limitu pamięci przydzielonego przez system), w przypadku przydzielenia wcześniej pamięci użytkownikowi
//
void UTEST5(void)
{
    // informacje o teście
    test_start(5, "Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni mniejszej niż przestrzeń obecna na stercie (żądana pamięć wymaga zwiększenia limitu pamięci przydzielonego przez system), w przypadku przydzielenia wcześniej pamięci użytkownikowi", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr1 = heap_malloc(28);
                test_error(ptr1 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));
                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr2 = heap_malloc(15861983);
                test_error(ptr2 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr2), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr2));
                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                        
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 6: Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni większej niż przestrzeń, która może być przydzielona przez system (żądana pamięć wymaga zwiększenia limitu pamięci przydzielonego przez system), w przypadku przydzielenia wcześniej pamięci użytkownikowi
//
void UTEST6(void)
{
    // informacje o teście
    test_start(6, "Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni większej niż przestrzeń, która może być przydzielona przez system (żądana pamięć wymaga zwiększenia limitu pamięci przydzielonego przez system), w przypadku przydzielenia wcześniej pamięci użytkownikowi", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr1 = heap_malloc(56);
                test_error(ptr1 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));
                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr2 = heap_malloc(67309505);
                test_error(ptr2 == NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                        
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 7: Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeń mniejszej niż przestrzeń poprzednio zwolniona
//
void UTEST7(void)
{
    // informacje o teście
    test_start(7, "Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeń mniejszej niż przestrzeń poprzednio zwolniona", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr1 = heap_malloc(57);
                void *ptr2 = heap_malloc(181);
                void *ptr3 = heap_malloc(239);

                test_error(ptr1 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(ptr2 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(ptr3 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");

                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));
                test_error(pointer_valid == get_pointer_type(ptr2), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr2));
                test_error(pointer_valid == get_pointer_type(ptr3), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr3));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                heap_free(ptr2);

                test_error(pointer_unallocated == get_pointer_type(ptr2), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr2));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr4 = heap_malloc(175);
                test_error(ptr2 == ptr4, "Funkcja heap_malloc() powinna zwrócić adres równy adresowi ptr2 (jest możliwość wykorzystania wolnego bloku pamięci do zaalokowania), a zwróciła różny");

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 8: Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni równej przestrzeni poprzednio zwolnionej
//
void UTEST8(void)
{
    // informacje o teście
    test_start(8, "Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni równej przestrzeni poprzednio zwolnionej", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr1 = heap_malloc(80);
                void *ptr2 = heap_malloc(125);
                void *ptr3 = heap_malloc(221);

                test_error(ptr1 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(ptr2 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(ptr3 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");

                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));
                test_error(pointer_valid == get_pointer_type(ptr2), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr2));
                test_error(pointer_valid == get_pointer_type(ptr3), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr3));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                heap_free(ptr2);

                test_error(pointer_unallocated == get_pointer_type(ptr2), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr2));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr4 = heap_malloc(125);
                test_error(ptr2 == ptr4, "Funkcja heap_malloc() powinna zwrócić adres równy adresowi ptr2 (jest możliwość wykorzystania wolnego bloku pamięci do zaalokowania), a zwróciła różny");

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 9: Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni mniejszej niż przestrzeń poprzednio zwolniona
//
void UTEST9(void)
{
    // informacje o teście
    test_start(9, "Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni mniejszej niż przestrzeń poprzednio zwolniona", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                size_t mem_sizes[] = {723, 662, 336, 586, 266, 820, 619, 611, 534, 392, 707, 247, 340, 626, 322, 898, 715, 808, 550, 113, 855, 950, 720, 507};
                void *ptrs[24];

                for (int i = 0; i < 24; ++i)
                {
                    ptrs[i] = heap_malloc(mem_sizes[i]);
                    test_error(ptrs[i] != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                    test_error(pointer_valid == get_pointer_type(ptrs[i]), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptrs[i]));

                    status = heap_validate();
                    test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                }           

                heap_free(ptrs[9]);

                test_error(pointer_unallocated == get_pointer_type(ptrs[9]), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptrs[9]));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr = heap_malloc(360);
                test_error(ptrs[9] == ptr, "Funkcja heap_malloc() powinna zwrócić adres równy adresowi ptrs[9] (jest możliwość wykorzystania wolnego bloku pamięci do zaalokowania), a zwróciła różny");

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 10: Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni równej przestrzeń poprzednio zwolnionej
//
void UTEST10(void)
{
    // informacje o teście
    test_start(10, "Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni równej przestrzeń poprzednio zwolnionej", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                size_t mem_sizes[] = {681, 895, 336, 783, 381, 975, 876, 643, 325, 557, 853, 968, 895, 189, 548, 306, 244, 185, 927, 424, 648, 341, 911, 899, 410, 989, 369, 199, 172, 766, 174, 744, 888, 953, 804, 852, 320, 578, 814, 139};
                void *ptrs[40];

                for (int i = 0; i < 40; ++i)
                {
                    ptrs[i] = heap_malloc(mem_sizes[i]);
                    test_error(ptrs[i] != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                    test_error(pointer_valid == get_pointer_type(ptrs[i]), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptrs[i]));

                    status = heap_validate();
                    test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                }           

                heap_free(ptrs[15]);

                test_error(pointer_unallocated == get_pointer_type(ptrs[15]), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptrs[15]));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr = heap_malloc(306);
                test_error(ptrs[15] == ptr, "Funkcja heap_malloc() powinna zwrócić adres równy adresowi ptrs[15] (jest możliwość wykorzystania wolnego bloku pamięci do zaalokowania), a zwróciła różny");

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 11: Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni większej niż przestrzeń poprzednio zwolnionej
//
void UTEST11(void)
{
    // informacje o teście
    test_start(11, "Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni większej niż przestrzeń poprzednio zwolnionej", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr1 = heap_malloc(48);
                void *ptr2 = heap_malloc(182);
                void *ptr3 = heap_malloc(224);

                test_error(ptr1 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(ptr2 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(ptr3 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");

                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));
                test_error(pointer_valid == get_pointer_type(ptr2), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr2));
                test_error(pointer_valid == get_pointer_type(ptr3), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr3));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                heap_free(ptr2);

                test_error(pointer_unallocated == get_pointer_type(ptr2), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr2));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr4 = heap_malloc(192);
                
                test_error(ptr2 != ptr4, "Funkcja heap_malloc() powinna zwrócić adres równy adresowi ptr2 (jest możliwość wykorzystania wolnego bloku pamięci do zaalokowania), a zwróciła różny");
                test_error(ptr4 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr4), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr4));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                        
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 12: Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni większej niż przestrzeń poprzednio zwolnionej
//
void UTEST12(void)
{
    // informacje o teście
    test_start(12, "Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni większej niż przestrzeń poprzednio zwolnionej", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr1 = heap_malloc(81);
                void *ptr2 = heap_malloc(3);
                void *ptr3 = heap_malloc(242);

                test_error(ptr1 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(ptr2 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(ptr3 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");

                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));
                test_error(pointer_valid == get_pointer_type(ptr2), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr2));
                test_error(pointer_valid == get_pointer_type(ptr3), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr3));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                heap_free(ptr2);

                test_error(pointer_unallocated == get_pointer_type(ptr2), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr2));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr4 = heap_malloc(11);

                test_error(ptr2 != ptr4, "Funkcja heap_malloc() powinna zwrócić adres równy adresowi ptr2 (jest możliwość wykorzystania wolnego bloku pamięci do zaalokowania), a zwróciła różny");
                test_error(ptr4 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr4), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr4));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
        
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 13: Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku przekazania do niej wartości 0
//
void UTEST13(void)
{
    // informacje o teście
    test_start(13, "Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku przekazania do niej wartości 0", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr = heap_calloc(0, 0);

                test_error(ptr == NULL, "Funkcja heap_calloc() powinna zwrócić wartość NULL");

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 14: Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku przekazania do niej wartości 0
//
void UTEST14(void)
{
    // informacje o teście
    test_start(14, "Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku przekazania do niej wartości 0", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr = heap_calloc(7, 0);

                test_error(ptr == NULL, "Funkcja heap_calloc() powinna zwrócić wartość NULL");

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
        
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 15: Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku przekazania do niej wartości 0
//
void UTEST15(void)
{
    // informacje o teście
    test_start(15, "Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku przekazania do niej wartości 0", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr = heap_calloc(0, 4);

                test_error(ptr == NULL, "Funkcja heap_calloc() powinna zwrócić wartość NULL");

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 16: Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni większej niż przestrzeń obecna na stercie
//
void UTEST16(void)
{
    // informacje o teście
    test_start(16, "Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni większej niż przestrzeń obecna na stercie", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr = heap_calloc(67108840, 1);

                test_error(ptr == NULL, "Funkcja heap_calloc() powinna zwrócić wartość NULL");

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 17: Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni większej niż przestrzeń obecna na stercie
//
void UTEST17(void)
{
    // informacje o teście
    test_start(17, "Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni większej niż przestrzeń obecna na stercie", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr = heap_calloc(16777216, 4);

                test_error(ptr == NULL, "Funkcja heap_calloc() powinna zwrócić wartość NULL");

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 18: Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku pierwszej próby zaalokowania przestrzeni mniejszej niż przestrzeń obecna na stercie
//
void UTEST18(void)
{
    // informacje o teście
    test_start(18, "Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku pierwszej próby zaalokowania przestrzeni mniejszej niż przestrzeń obecna na stercie", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr = heap_calloc(675, 1);

                test_error(ptr != NULL, "Funkcja heap_calloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr), "Funkcja heap_calloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr));

                for (int i = 0; i < 675; ++i)
                    test_error(ptr[i] == 0, "Funkcja heap_calloc() powinna zwrócić wskaźnik na obszar pamięci wypełniony wartościami 0");


                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 19: Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku pierwszej próby zaalokowania przestrzeni mniejszej niż przestrzeń obecna na stercie
//
void UTEST19(void)
{
    // informacje o teście
    test_start(19, "Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku pierwszej próby zaalokowania przestrzeni mniejszej niż przestrzeń obecna na stercie", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr = heap_calloc(134, 4);

                test_error(ptr != NULL, "Funkcja heap_calloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr), "Funkcja heap_calloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr));

                for (int i = 0; i < 536; ++i)
                    test_error(ptr[i] == 0, "Funkcja heap_calloc() powinna zwrócić wskaźnik na obszar pamięci wypełniony wartościami 0");


                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 20: Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni mniejszej niż przestrzeń obecna na stercie, w przypadku przydzielenia wcześniej pamięci użytkownikowi
//
void UTEST20(void)
{
    // informacje o teście
    test_start(20, "Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni mniejszej niż przestrzeń obecna na stercie, w przypadku przydzielenia wcześniej pamięci użytkownikowi", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr1 = heap_calloc(53, 43);
                test_error(ptr1 != NULL, "Funkcja heap_calloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_calloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));
                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                for (int i = 0; i < 2279; ++i)
                    test_error(ptr1[i] == 0, "Funkcja heap_calloc() powinna zwrócić wskaźnik na obszar pamięci wypełniony wartościami 0");

                char *ptr2 = heap_calloc(182, 43);
                test_error(ptr2 != NULL, "Funkcja heap_calloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr2), "Funkcja heap_calloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr2));
                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                for (int i = 0; i < 7826; ++i)
                    test_error(ptr2[i] == 0, "Funkcja heap_calloc() powinna zwrócić wskaźnik na obszar pamięci wypełniony wartościami 0");

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 21: Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni mniejszej niż przestrzeń obecna na stercie (żądana pamięć wymaga zwiększenia limitu pamięci przydzielonego przez system), w przypadku przydzielenia wcześniej pamięci użytkownikowi
//
void UTEST21(void)
{
    // informacje o teście
    test_start(21, "Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni mniejszej niż przestrzeń obecna na stercie (żądana pamięć wymaga zwiększenia limitu pamięci przydzielonego przez system), w przypadku przydzielenia wcześniej pamięci użytkownikowi", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr1 = heap_calloc(96, 1);
                test_error(ptr1 != NULL, "Funkcja heap_calloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_calloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));
                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                for (int i = 0; i < 96; ++i)
                    test_error(ptr1[i] == 0, "Funkcja heap_calloc() powinna zwrócić wskaźnik na obszar pamięci wypełniony wartościami 0");

                char *ptr2 = heap_calloc(16584004, 1);
                test_error(ptr2 != NULL, "Funkcja heap_calloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr2), "Funkcja heap_calloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr2));
                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                for (int i = 0; i < 16584004; ++i)
                    test_error(ptr2[i] == 0, "Funkcja heap_calloc() powinna zwrócić wskaźnik na obszar pamięci wypełniony wartościami 0");

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 22: Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni większej niż przestrzeń, która może być przydzielona przez system (żądana pamięć wymaga zwiększenia limitu pamięci przydzielonego przez system), w przypadku przydzielenia wcześniej pamięci użytkownikowi
//
void UTEST22(void)
{
    // informacje o teście
    test_start(22, "Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni większej niż przestrzeń, która może być przydzielona przez system (żądana pamięć wymaga zwiększenia limitu pamięci przydzielonego przez system), w przypadku przydzielenia wcześniej pamięci użytkownikowi", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr1 = heap_calloc(64, 4);
                test_error(ptr1 != NULL, "Funkcja heap_calloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_calloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));
                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                for (int i = 0; i < 256; ++i)
                    test_error(ptr1[i] == 0, "Funkcja heap_calloc() powinna zwrócić wskaźnik na obszar pamięci wypełniony wartościami 0");

                char *ptr2 = heap_calloc(67289483, 4);
                test_error(ptr2 == NULL, "Funkcja heap_calloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 23: Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeń mniejszej niż przestrzeń poprzednio zwolniona
//
void UTEST23(void)
{
    // informacje o teście
    test_start(23, "Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeń mniejszej niż przestrzeń poprzednio zwolniona", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr1 = heap_calloc(23, 1);
                char *ptr2 = heap_calloc(193, 1);
                char *ptr3 = heap_calloc(228, 1);

                test_error(ptr1 != NULL, "Funkcja heap_calloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(ptr2 != NULL, "Funkcja heap_calloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(ptr3 != NULL, "Funkcja heap_calloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");

                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_calloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));
                test_error(pointer_valid == get_pointer_type(ptr2), "Funkcja heap_calloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr2));
                test_error(pointer_valid == get_pointer_type(ptr3), "Funkcja heap_calloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr3));

                for (int i = 0; i < 23; ++i)
                    test_error(ptr1[i] == 0, "Funkcja heap_calloc() powinna zwrócić wskaźnik na obszar pamięci wypełniony wartościami 0");

                for (int i = 0; i < 193; ++i)
                    test_error(ptr2[i] == 0, "Funkcja heap_calloc() powinna zwrócić wskaźnik na obszar pamięci wypełniony wartościami 0");
     
                for (int i = 0; i < 228; ++i)
                    test_error(ptr3[i] == 0, "Funkcja heap_calloc() powinna zwrócić wskaźnik na obszar pamięci wypełniony wartościami 0");

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                heap_free(ptr2);

                test_error(pointer_unallocated == get_pointer_type(ptr2), "Funkcja heap_calloc() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr2));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr4 = heap_calloc(179, 1);
                test_error(ptr2 == ptr4, "Funkcja heap_calloc() powinna zwrócić adres równy adresowi ptr2 (jest możliwość wykorzystania wolnego bloku pamięci do zaalokowania), a zwróciła różny");

                for (int i = 0; i < 179; ++i)
                    test_error(ptr4[i] == 0, "Funkcja heap_calloc() powinna zwrócić wskaźnik na obszar pamięci wypełniony wartościami 0");

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 24: Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni równej przestrzeni poprzednio zwolnionej
//
void UTEST24(void)
{
    // informacje o teście
    test_start(24, "Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni równej przestrzeni poprzednio zwolnionej", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr1 = heap_calloc(48, 3);
                char *ptr2 = heap_calloc(174, 3);
                char *ptr3 = heap_calloc(283, 3);

                test_error(ptr1 != NULL, "Funkcja heap_calloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(ptr2 != NULL, "Funkcja heap_calloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(ptr3 != NULL, "Funkcja heap_calloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");

                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_calloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));
                test_error(pointer_valid == get_pointer_type(ptr2), "Funkcja heap_calloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr2));
                test_error(pointer_valid == get_pointer_type(ptr3), "Funkcja heap_calloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr3));

                for (int i = 0; i < 144; ++i)
                    test_error(ptr1[i] == 0, "Funkcja heap_calloc() powinna zwrócić wskaźnik na obszar pamięci wypełniony wartościami 0");

                for (int i = 0; i < 522; ++i)
                    test_error(ptr2[i] == 0, "Funkcja heap_calloc() powinna zwrócić wskaźnik na obszar pamięci wypełniony wartościami 0");
     
                for (int i = 0; i < 849; ++i)
                    test_error(ptr3[i] == 0, "Funkcja heap_calloc() powinna zwrócić wskaźnik na obszar pamięci wypełniony wartościami 0");

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                heap_free(ptr2);

                test_error(pointer_unallocated == get_pointer_type(ptr2), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr2));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr4 = heap_calloc(174, 3);
                test_error(ptr2 == ptr4, "Funkcja heap_calloc() powinna zwrócić adres równy adresowi ptr2 (jest możliwość wykorzystania wolnego bloku pamięci do zaalokowania), a zwróciła różny");

                for (int i = 0; i < 522; ++i)
                    test_error(ptr4[i] == 0, "Funkcja heap_calloc() powinna zwrócić wskaźnik na obszar pamięci wypełniony wartościami 0");

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 25: Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni mniejszej niż przestrzeń poprzednio zwolniona
//
void UTEST25(void)
{
    // informacje o teście
    test_start(25, "Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni mniejszej niż przestrzeń poprzednio zwolniona", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                size_t mem_sizes[] = {708, 774, 945, 725, 718, 782, 300, 323, 998, 307, 612, 860, 174, 124, 697, 181, 180, 689, 183, 861, 690};
                size_t mem_units[] = {1, 4, 1, 3, 5, 3, 2, 4, 2, 5, 1, 3, 1, 3, 3, 5, 5, 2, 2, 1, 4};
                char *ptrs[21];

                for (int i = 0; i < 21; ++i)
                {
                    ptrs[i] = heap_calloc(mem_sizes[i], mem_units[i]);
                    test_error(ptrs[i] != NULL, "Funkcja heap_calloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                    test_error(pointer_valid == get_pointer_type(ptrs[i]), "Funkcja heap_calloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptrs[i]));

                    for (size_t j = 0; j < mem_sizes[i] * mem_units[i]; ++j)
                        test_error(ptrs[i][j] == 0, "Funkcja heap_calloc() powinna zwrócić wskaźnik na obszar pamięci wypełniony wartościami 0");

                    status = heap_validate();
                    test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                }

                heap_free(ptrs[9]);

                test_error(pointer_unallocated == get_pointer_type(ptrs[9]), "Funkcja heap_calloc() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptrs[9]));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr = heap_calloc(287, 5);
                test_error(ptrs[9] == ptr, "Funkcja heap_calloc() powinna zwrócić adres równy adresowi ptrs[9] (jest możliwość wykorzystania wolnego bloku pamięci do zaalokowania), a zwróciła różny");

                for (size_t j = 0; j < 287 * 5; ++j)
                    test_error(ptrs[9][j] == 0, "Funkcja heap_calloc() powinna zwrócić wskaźnik na obszar pamięci wypełniony wartościami 0");


                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
             
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 26: Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni równej przestrzeni poprzednio zwolnionej
//
void UTEST26(void)
{
    // informacje o teście
    test_start(26, "Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni równej przestrzeni poprzednio zwolnionej", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                 int status = heap_setup();
                 test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 size_t mem_sizes[] = {365, 290, 715, 386, 745, 612, 354, 841, 426, 347, 337, 205, 530, 540, 799, 958, 422, 705, 684, 697, 718, 404, 307, 519, 755, 497, 665, 156, 430, 974, 328, 165, 254};
                 size_t mem_units[] = {4, 1, 4, 3, 2, 3, 4, 4, 1, 5, 5, 5, 3, 3, 1, 2, 3, 1, 2, 2, 5, 5, 4, 1, 1, 1, 1, 5, 3, 2, 3, 1, 4};
                 char *ptrs[33];

                 for (int i = 0; i < 33; ++i)
                 {
                     ptrs[i] = heap_calloc(mem_sizes[i], mem_units[i]);
                     test_error(ptrs[i] != NULL, "Funkcja heap_calloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                     test_error(pointer_valid == get_pointer_type(ptrs[i]), "Funkcja heap_calloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptrs[i]));

                     for (size_t j = 0; j < mem_sizes[i] * mem_units[i]; ++j)
                         test_error(ptrs[i][j] == 0, "Funkcja heap_calloc() powinna zwrócić wskaźnik na obszar pamięci wypełniony wartościami 0");

                     status = heap_validate();
                     test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                 }

                 heap_free(ptrs[25]);

                 test_error(pointer_unallocated == get_pointer_type(ptrs[25]), "Funkcja heap_calloc() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptrs[25]));

                 status = heap_validate();
                 test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 void *ptr = heap_calloc(497, 1);
                 test_error(ptrs[25] == ptr, "Funkcja heap_calloc() powinna zwrócić adres równy adresowi ptrs[25] (jest możliwość wykorzystania wolnego bloku pamięci do zaalokowania), a zwróciła różny");

                 for (size_t j = 0; j < 497 * 1; ++j)
                     test_error(ptrs[25][j] == 0, "Funkcja heap_calloc() powinna zwrócić wskaźnik na obszar pamięci wypełniony wartościami 0");


                 status = heap_validate();
                 test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
             
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 27: Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni większej niż przestrzeń poprzednio zwolnionej
//
void UTEST27(void)
{
    // informacje o teście
    test_start(27, "Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni większej niż przestrzeń poprzednio zwolnionej", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr1 = heap_calloc(53, 1);
                char *ptr2 = heap_calloc(184, 1);
                char *ptr3 = heap_calloc(276, 1);

                test_error(ptr1 != NULL, "Funkcja heap_calloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(ptr2 != NULL, "Funkcja heap_calloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(ptr3 != NULL, "Funkcja heap_calloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");

                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));
                test_error(pointer_valid == get_pointer_type(ptr2), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr2));
                test_error(pointer_valid == get_pointer_type(ptr3), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr3));

                for (int i = 0; i < 53; ++i)
                    test_error(ptr1[i] == 0, "Funkcja heap_calloc() powinna zwrócić wskaźnik na obszar pamięci wypełniony wartościami 0");

                for (int i = 0; i < 184; ++i)
                    test_error(ptr2[i] == 0, "Funkcja heap_calloc() powinna zwrócić wskaźnik na obszar pamięci wypełniony wartościami 0");
     
                for (int i = 0; i < 276; ++i)
                    test_error(ptr3[i] == 0, "Funkcja heap_calloc() powinna zwrócić wskaźnik na obszar pamięci wypełniony wartościami 0");

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                heap_free(ptr2);

                test_error(pointer_unallocated == get_pointer_type(ptr2), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr2));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr4 = heap_calloc(192, 1);

                test_error(ptr2 != ptr4, "Funkcja heap_calloc() powinna zwrócić adres równy adresowi ptr2 (jest możliwość wykorzystania wolnego bloku pamięci do zaalokowania), a zwróciła różny");
                test_error(ptr4 != NULL, "Funkcja heap_calloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr4), "Funkcja heap_calloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr4));

                for (int i = 0; i < 192; ++i)
                    test_error(ptr4[i] == 0, "Funkcja heap_calloc() powinna zwrócić wskaźnik na obszar pamięci wypełniony wartościami 0");

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
        
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 28: Sprawdzanie poprawności działania funkcji heap_get_largest_used_block_size
//
void UTEST28(void)
{
    // informacje o teście
    test_start(28, "Sprawdzanie poprawności działania funkcji heap_get_largest_used_block_size", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    

                size_t block_size = heap_get_largest_used_block_size();
                test_error(block_size == 0, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 0, a zwróciła na %lu", block_size);

                heap_clean();
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 29: Sprawdzanie poprawności działania funkcji heap_get_largest_used_block_size
//
void UTEST29(void)
{
    // informacje o teście
    test_start(29, "Sprawdzanie poprawności działania funkcji heap_get_largest_used_block_size", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                size_t block_size = heap_get_largest_used_block_size();
                test_error(block_size == 0, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 0, a zwróciła na %lu", block_size);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
        
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 30: Sprawdzanie poprawności działania funkcji heap_get_largest_used_block_size
//
void UTEST30(void)
{
    // informacje o teście
    test_start(30, "Sprawdzanie poprawności działania funkcji heap_get_largest_used_block_size", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                size_t mem_sizes[] = {291, 524, 701, 268, 524, 802, 928, 207, 598, 310, 101, 378, 934, 227, 396, 917, 399, 447, 306, 788, 635, 686, 414, 852, 218};
                void *ptrs[25];

                for (int i = 0; i < 25; ++i)
                {
                    ptrs[i] = heap_malloc(mem_sizes[i]);
                    test_error(pointer_valid == get_pointer_type(ptrs[i]), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptrs[i]));

                    status = heap_validate();
                    test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                }           


                size_t block_size = heap_get_largest_used_block_size();
                test_error(block_size == 934, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 934, a zwróciła na %lu", block_size);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                        
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 31: Sprawdzanie poprawności działania funkcji heap_get_largest_used_block_size
//
void UTEST31(void)
{
    // informacje o teście
    test_start(31, "Sprawdzanie poprawności działania funkcji heap_get_largest_used_block_size", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                 int status = heap_setup();
                 test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 size_t mem_sizes[] = {274, 568, 336, 551, 507, 244, 133, 853, 456, 126, 613, 780, 392, 351, 379, 995, 590, 517, 899, 929, 916, 358, 632};
                 size_t mem_units[] = {1, 2, 2, 2, 2, 2, 4, 1, 1, 4, 2, 5, 3, 2, 1, 2, 1, 2, 1, 1, 5, 3, 4};
                 char *ptrs[23];

                 for (int i = 0; i < 23; ++i)
                 {
                     ptrs[i] = heap_calloc(mem_sizes[i], mem_units[i]);
                     test_error(ptrs[i] != NULL, "Funkcja heap_calloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");

                     status = heap_validate();
                     test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                 }

                 size_t block_size = heap_get_largest_used_block_size();
                 test_error(block_size == 4580, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 4580, a zwróciła na %lu", block_size);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
             
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 32: Sprawdzanie poprawności działania funkcji heap_get_largest_used_block_size
//
void UTEST32(void)
{
    // informacje o teście
    test_start(32, "Sprawdzanie poprawności działania funkcji heap_get_largest_used_block_size", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                size_t mem_sizes[] = {890, 290, 723, 881, 743, 415, 728, 991, 525, 981, 919, 719, 163, 460, 514, 968, 881, 489, 736, 190, 847, 164, 223, 206, 588, 671, 141, 647, 533};
                char *ptrs[29];

                for (int i = 0; i < 29; ++i)
                {
                    ptrs[i] = heap_malloc(mem_sizes[i]);
                    test_error(pointer_valid == get_pointer_type(ptrs[i]), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptrs[i]));

                    status = heap_validate();
                    test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                }           

                ptrs[5][-1] += 10 ;

                status = heap_validate();
                test_error(status == 1, "Funkcja heap_validate() powinna zwrócić wartość 1, a zwróciła na %d", status);

                size_t block_size = heap_get_largest_used_block_size();
                test_error(block_size == 0, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 0, a zwróciła na %lu", block_size);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 33: Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku przekazania do niej niepoprawnego wskaźnika lub wskaźnika sprzed inicjalizacji sterty
//
void UTEST33(void)
{
    // informacje o teście
    test_start(33, "Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku przekazania do niej niepoprawnego wskaźnika lub wskaźnika sprzed inicjalizacji sterty", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                char *ptr = (char *)(intptr_t)rand();
                
                heap_free(ptr);
                
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);
            
                heap_free(NULL);
                
                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                heap_free(ptr);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                

            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 34: Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku przekazania do niej niepoprawnego wskaźnika
//
void UTEST34(void)
{
    // informacje o teście
    test_start(34, "Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku przekazania do niej niepoprawnego wskaźnika", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr = heap_malloc(106);
                test_error(ptr != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                heap_free(ptr + 1);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 106, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 106, a zwróciła na %llu", heap_get_largest_used_block_size());

                heap_free(ptr - 1);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 106, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 106, a zwróciła na %llu", heap_get_largest_used_block_size());

                heap_free(ptr + 98);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 106, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 106, a zwróciła na %llu", heap_get_largest_used_block_size());

                heap_free(ptr - 30);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 106, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 106, a zwróciła na %llu", heap_get_largest_used_block_size());

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 35: Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku przekazania do niej niepoprawnego wskaźnika
//
void UTEST35(void)
{
    // informacje o teście
    test_start(35, "Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku przekazania do niej niepoprawnego wskaźnika", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr1 = heap_malloc(337);
                test_error(ptr1 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));

                void *ptr2 = heap_malloc(75);
                test_error(ptr2 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr2), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr2));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 337, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 337, a zwróciła na %llu", heap_get_largest_used_block_size());

                heap_free(ptr1);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 75, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 75, a zwróciła na %llu", heap_get_largest_used_block_size());

                test_error(pointer_unallocated == get_pointer_type(ptr1), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr1));

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 36: Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku przekazania do niej niepoprawnego wskaźnika
//
void UTEST36(void)
{
    // informacje o teście
    test_start(36, "Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku przekazania do niej niepoprawnego wskaźnika", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr1 = heap_malloc(306);
                test_error(ptr1 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 306, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 306, a zwróciła na %llu", heap_get_largest_used_block_size());

                heap_free(ptr1);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %llu", heap_get_largest_used_block_size());

                test_error(pointer_unallocated == get_pointer_type(ptr1), "Funkcja get_pointer_type() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr1));

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 37: Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku przekazania do niej niepoprawnego wskaźnika
//
void UTEST37(void)
{
    // informacje o teście
    test_start(37, "Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku przekazania do niej niepoprawnego wskaźnika", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr1 = heap_malloc(398);
                test_error(ptr1 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));

                void *ptr2 = heap_malloc(92);
                test_error(ptr2 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr2), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr2));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 398, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 398, a zwróciła na %llu", heap_get_largest_used_block_size());

                heap_free(ptr1);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 92, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 92, a zwróciła na %llu", heap_get_largest_used_block_size());

                test_error(pointer_unallocated == get_pointer_type(ptr1), "Funkcja get_pointer_type() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr1));

                heap_free(ptr2);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %llu", heap_get_largest_used_block_size());

                test_error(pointer_unallocated == get_pointer_type(ptr2), "Funkcja get_pointer_type() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr2));


                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 38: Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia dwóch bloków pamięci sąsiadujących ze sobą
//
void UTEST38(void)
{
    // informacje o teście
    test_start(38, "Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia dwóch bloków pamięci sąsiadujących ze sobą", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr1 = heap_malloc(333);
                test_error(ptr1 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));

                void *ptr2 = heap_malloc(564);
                test_error(ptr2 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr2), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr2));

                void *ptr3 = heap_malloc(69);
                test_error(ptr3 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr3), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr3));


                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 564, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 564, a zwróciła na %llu", heap_get_largest_used_block_size());

                heap_free(ptr2);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 333, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 333, a zwróciła na %llu", heap_get_largest_used_block_size());

                test_error(pointer_unallocated == get_pointer_type(ptr2), "Funkcja get_pointer_type() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr2));

                heap_free(ptr1);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 69, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 69, a zwróciła na %llu", heap_get_largest_used_block_size());

                test_error(pointer_unallocated == get_pointer_type(ptr1), "Funkcja get_pointer_type() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr1));

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 39: Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia dwóch bloków pamięci sąsiadujących ze sobą i zaalokowanie w tym miejscu nowej pamięci o rozmiarze sumy dwóch zwolnionych bloków
//
void UTEST39(void)
{
    // informacje o teście
    test_start(39, "Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia dwóch bloków pamięci sąsiadujących ze sobą i zaalokowanie w tym miejscu nowej pamięci o rozmiarze sumy dwóch zwolnionych bloków", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr1 = heap_malloc(324);
                test_error(ptr1 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));

                void *ptr2 = heap_malloc(884);
                test_error(ptr2 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr2), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr2));

                void *ptr3 = heap_malloc(56);
                test_error(ptr3 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr3), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr3));


                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 884, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 884, a zwróciła na %llu", heap_get_largest_used_block_size());

                heap_free(ptr2);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 324, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 324, a zwróciła na %llu", heap_get_largest_used_block_size());

                test_error(pointer_unallocated == get_pointer_type(ptr2), "Funkcja get_pointer_type() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr2));

                heap_free(ptr1);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 56, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 56, a zwróciła na %llu", heap_get_largest_used_block_size());

                test_error(pointer_unallocated == get_pointer_type(ptr1), "Funkcja get_pointer_type() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr1));

                void *ptr4 = heap_malloc(1220);
                test_error(ptr4 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr4), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr4));
                test_error(ptr1 == ptr4, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego pierwszego bloku pamięci");
                test_error(heap_get_largest_used_block_size() == 1220, "Funkcja heap_validate() powinna zwrócić wartość 1220, a zwróciła na %llu", heap_get_largest_used_block_size());

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 40: Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia dwóch bloków pamięci sąsiadujących ze sobą
//
void UTEST40(void)
{
    // informacje o teście
    test_start(40, "Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia dwóch bloków pamięci sąsiadujących ze sobą", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr1 = heap_malloc(378);
                test_error(ptr1 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));

                void *ptr2 = heap_malloc(8262);
                test_error(ptr2 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr2), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr2));

                void *ptr3 = heap_malloc(809);
                test_error(ptr3 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr3), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr3));

                void *ptr4 = heap_malloc(54);
                test_error(ptr4 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr4), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr4));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 8262, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 8262, a zwróciła na %llu", heap_get_largest_used_block_size());

                heap_free(ptr2);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 809, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 809, a zwróciła na %llu", heap_get_largest_used_block_size());

                test_error(pointer_unallocated == get_pointer_type(ptr2), "Funkcja get_pointer_type() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr2));

                heap_free(ptr3);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 378, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 378, a zwróciła na %llu", heap_get_largest_used_block_size());

                test_error(pointer_unallocated == get_pointer_type(ptr3), "Funkcja get_pointer_type() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr3));

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 41: Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia dwóch bloków pamięci sąsiadujących ze sobą i zaalokowanie w tym miejscu nowej pamięci o rozmiarze sumy dwóch zwolnionych bloków
//
void UTEST41(void)
{
    // informacje o teście
    test_start(41, "Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia dwóch bloków pamięci sąsiadujących ze sobą i zaalokowanie w tym miejscu nowej pamięci o rozmiarze sumy dwóch zwolnionych bloków", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr1 = heap_malloc(257);
                test_error(ptr1 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));

                void *ptr2 = heap_malloc(5556);
                test_error(ptr2 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr2), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr2));

                void *ptr3 = heap_malloc(995);
                test_error(ptr3 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr3), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr3));

                void *ptr4 = heap_malloc(92);
                test_error(ptr4 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr4), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr4));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 5556, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 5556, a zwróciła na %llu", heap_get_largest_used_block_size());

                heap_free(ptr2);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 995, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 995, a zwróciła na %llu", heap_get_largest_used_block_size());

                test_error(pointer_unallocated == get_pointer_type(ptr2), "Funkcja get_pointer_type() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr2));

                heap_free(ptr3);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 257, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 257, a zwróciła na %llu", heap_get_largest_used_block_size());

                test_error(pointer_unallocated == get_pointer_type(ptr3), "Funkcja get_pointer_type() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr3));

                void *ptr = heap_malloc(6559);
                test_error(ptr != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr));
                test_error(ptr2 == ptr, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego pierwszego bloku pamięci");
                test_error(heap_get_largest_used_block_size() == 6559, "Funkcja heap_validate() powinna zwrócić wartość 6559, a zwróciła na %llu", heap_get_largest_used_block_size());


                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 42: Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia trzech bloków pamięci sąsiadujących ze sobą
//
void UTEST42(void)
{
    // informacje o teście
    test_start(42, "Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia trzech bloków pamięci sąsiadujących ze sobą", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr1 = heap_malloc(8301);
                test_error(ptr1 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));

                void *ptr2 = heap_malloc(303);
                test_error(ptr2 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr2), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr2));

                void *ptr3 = heap_malloc(850);
                test_error(ptr3 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr3), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr3));

                void *ptr4 = heap_malloc(77);
                test_error(ptr4 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr4), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr4));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 8301, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 8301, a zwróciła na %llu", heap_get_largest_used_block_size());

                heap_free(ptr1);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 850, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 850, a zwróciła na %llu", heap_get_largest_used_block_size());

                test_error(pointer_unallocated == get_pointer_type(ptr1), "Funkcja get_pointer_type() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr1));

                heap_free(ptr3);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 303, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 303, a zwróciła na %llu", heap_get_largest_used_block_size());
                test_error(pointer_unallocated == get_pointer_type(ptr3), "Funkcja get_pointer_type() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr3));

                heap_free(ptr2);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 77, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 77, a zwróciła na %llu", heap_get_largest_used_block_size());
                test_error(pointer_unallocated == get_pointer_type(ptr3), "Funkcja get_pointer_type() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr3));


                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 43: Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia trzech bloków pamięci sąsiadujących ze sobą i zaalokowanie w tym miejscu nowej pamięci o rozmiarze sumy dwóch zwolnionych bloków
//
void UTEST43(void)
{
    // informacje o teście
    test_start(43, "Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia trzech bloków pamięci sąsiadujących ze sobą i zaalokowanie w tym miejscu nowej pamięci o rozmiarze sumy dwóch zwolnionych bloków", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr1 = heap_malloc(6159);
                test_error(ptr1 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));

                void *ptr2 = heap_malloc(313);
                test_error(ptr2 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr2), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr2));

                void *ptr3 = heap_malloc(561);
                test_error(ptr3 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr3), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr3));

                void *ptr4 = heap_malloc(73);
                test_error(ptr4 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr4), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr4));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 6159, "Funkcja heap_validate() powinna zwrócić wartość 6159, a zwróciła na %llu", heap_get_largest_used_block_size());

                heap_free(ptr1);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 561, "Funkcja heap_validate() powinna zwrócić wartość 561, a zwróciła na %llu", heap_get_largest_used_block_size());

                test_error(pointer_unallocated == get_pointer_type(ptr1), "Funkcja get_pointer_type() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr1));

                heap_free(ptr3);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 313, "Funkcja heap_validate() powinna zwrócić wartość 313, a zwróciła na %llu", heap_get_largest_used_block_size());
                test_error(pointer_unallocated == get_pointer_type(ptr3), "Funkcja get_pointer_type() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr3));

                heap_free(ptr2);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 73, "Funkcja heap_validate() powinna zwrócić wartość 73, a zwróciła na %llu", heap_get_largest_used_block_size());
                test_error(pointer_unallocated == get_pointer_type(ptr3), "Funkcja get_pointer_type() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr3));

                void *ptr = heap_malloc(7044);
                test_error(ptr != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr));
                test_error(ptr1 == ptr, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego pierwszego bloku pamięci");
                test_error(heap_get_largest_used_block_size() == 7044, "Funkcja heap_validate() powinna zwrócić wartość 7044, a zwróciła na %llu", heap_get_largest_used_block_size());

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 44: Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia bloku pamięci, zaalokowania w to miejsce mniejszego bloku, zwolnienia go i zaalokowania ponownie większego bloku
//
void UTEST44(void)
{
    // informacje o teście
    test_start(44, "Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia bloku pamięci, zaalokowania w to miejsce mniejszego bloku, zwolnienia go i zaalokowania ponownie większego bloku", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr1 = heap_malloc(309);
                test_error(ptr1 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));

                void *ptr2 = heap_malloc(50);
                test_error(ptr2 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr2), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr2));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 309, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 309, a zwróciła na %llu", heap_get_largest_used_block_size());

                heap_free(ptr1);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 50, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 50, a zwróciła na %llu", heap_get_largest_used_block_size());

                test_error(pointer_unallocated == get_pointer_type(ptr1), "Funkcja get_pointer_type() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr1));

                void *ptr3 = heap_malloc(241);
                test_error(ptr3 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(ptr3 == ptr1, "Funkcja heap_malloc() powinna zwrócić adres pierwszego wolnego bloku pamięci");
                test_error(pointer_valid == get_pointer_type(ptr3), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr3));

                test_error(heap_get_largest_used_block_size() == 241, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 241, a zwróciła na %llu", heap_get_largest_used_block_size());

                heap_free(ptr3);

                void *ptr4 = heap_malloc(309);
                test_error(ptr4 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(ptr4 == ptr1, "Funkcja heap_malloc() powinna zwrócić adres pierwszego wolnego bloku pamięci");
                test_error(pointer_valid == get_pointer_type(ptr4), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr4));

                test_error(heap_get_largest_used_block_size() == 309, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 309, a zwróciła na %llu", heap_get_largest_used_block_size());


                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 45: Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia trzech bloków pamięci sąsiadujących ze sobą i zaalokowanie w tym miejscu nowej pamięci o rozmiarze sumy dwóch zwolnionych bloków
//
void UTEST45(void)
{
    // informacje o teście
    test_start(45, "Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia trzech bloków pamięci sąsiadujących ze sobą i zaalokowanie w tym miejscu nowej pamięci o rozmiarze sumy dwóch zwolnionych bloków", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr1 = heap_malloc(775);
                test_error(ptr1 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));

                void *ptr2 = heap_malloc(291);
                test_error(ptr2 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr2), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr2));

                void *ptr3 = heap_malloc(6949);
                test_error(ptr3 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr3), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr3));

                void *ptr4 = heap_malloc(92);
                test_error(ptr4 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr4), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr4));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 6949, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 6949, a zwróciła na %llu", heap_get_largest_used_block_size());

                heap_free(ptr3);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 775, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 775, a zwróciła na %llu", heap_get_largest_used_block_size());

                test_error(pointer_unallocated == get_pointer_type(ptr3), "Funkcja get_pointer_type() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr3));

                void *ptr5 = heap_malloc(4560);
                test_error(ptr5 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(ptr5 == ptr3, "Funkcja heap_malloc() powinna zwrócić adres pierwszego wolnego bloku pamięci");
                test_error(pointer_valid == get_pointer_type(ptr5), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr5));

                test_error(heap_get_largest_used_block_size() == 4560, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 4560, a zwróciła na %llu", heap_get_largest_used_block_size());

                heap_free(ptr5);
                heap_free(ptr2);
                
                void *ptr6 = heap_malloc(1075);
                test_error(ptr6 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(ptr6 == ptr2, "Funkcja heap_malloc() powinna zwrócić adres pierwszego wolnego bloku pamięci");
                test_error(pointer_valid == get_pointer_type(ptr6), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr6));

                test_error(heap_get_largest_used_block_size() == 1075, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 1075, a zwróciła na %llu", heap_get_largest_used_block_size());

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                

            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 46: Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia trzech bloków pamięci sąsiadujących ze sobą i zaalokowanie w tym miejscu nowej pamięci o rozmiarze sumy dwóch zwolnionych bloków
//
void UTEST46(void)
{
    // informacje o teście
    test_start(46, "Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia trzech bloków pamięci sąsiadujących ze sobą i zaalokowanie w tym miejscu nowej pamięci o rozmiarze sumy dwóch zwolnionych bloków", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr1 = heap_malloc(929);
                test_error(ptr1 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));

                void *ptr2 = heap_malloc(336);
                test_error(ptr2 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr2), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr2));

                void *ptr3 = heap_malloc(8319);
                test_error(ptr3 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr3), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr3));

                void *ptr4 = heap_malloc(95);
                test_error(ptr4 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr4), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr4));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 8319, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 8319, a zwróciła na %llu", heap_get_largest_used_block_size());

                heap_free(ptr3);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 929, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 929, a zwróciła na %llu", heap_get_largest_used_block_size());

                test_error(pointer_unallocated == get_pointer_type(ptr3), "Funkcja get_pointer_type() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr3));

                void *ptr5 = heap_malloc(6154);
                test_error(ptr5 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(ptr5 == ptr3, "Funkcja heap_malloc() powinna zwrócić adres pierwszego wolnego bloku pamięci");
                test_error(pointer_valid == get_pointer_type(ptr5), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr5));

                test_error(heap_get_largest_used_block_size() == 6154, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 6154, a zwróciła na %llu", heap_get_largest_used_block_size());

                heap_free(ptr2);
                heap_free(ptr5);

                void *ptr6 = heap_malloc(1279);
                test_error(ptr6 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(ptr6 == ptr2, "Funkcja heap_malloc() powinna zwrócić adres pierwszego wolnego bloku pamięci");
                test_error(pointer_valid == get_pointer_type(ptr6), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr6));

                test_error(heap_get_largest_used_block_size() == 1279, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 1279, a zwróciła na %llu", heap_get_largest_used_block_size());

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 47: Sprawdzanie poprawności działania funkcji heap_malloc, heap_calloc i heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia trzech bloków pamięci sąsiadujących ze sobą i zaalokowanie w tym miejscu nowej pamięci o rozmiarze sumy dwóch zwolnionych bloków
//
void UTEST47(void)
{
    // informacje o teście
    test_start(47, "Sprawdzanie poprawności działania funkcji heap_malloc, heap_calloc i heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia trzech bloków pamięci sąsiadujących ze sobą i zaalokowanie w tym miejscu nowej pamięci o rozmiarze sumy dwóch zwolnionych bloków", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr1 = heap_malloc(9266);
                test_error(ptr1 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr1), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr1));

                void *ptr2 = heap_malloc(366);
                test_error(ptr2 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr2), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr2));

                void *ptr3 = heap_calloc(768, 1);
                test_error(ptr3 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr3), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr3));

                void *ptr4 = heap_malloc(55);
                test_error(ptr4 != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr4), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr4));

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 9266, "Funkcja heap_validate() powinna zwrócić wartość 9266, a zwróciła na %llu", heap_get_largest_used_block_size());

                heap_free(ptr1);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 768, "Funkcja heap_validate() powinna zwrócić wartość 768, a zwróciła na %llu", heap_get_largest_used_block_size());

                test_error(pointer_unallocated == get_pointer_type(ptr1), "Funkcja get_pointer_type() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr1));

                heap_free(ptr3);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 366, "Funkcja heap_validate() powinna zwrócić wartość 366, a zwróciła na %llu", heap_get_largest_used_block_size());
                test_error(pointer_unallocated == get_pointer_type(ptr3), "Funkcja get_pointer_type() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr3));

                heap_free(ptr2);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
                test_error(heap_get_largest_used_block_size() == 55, "Funkcja heap_validate() powinna zwrócić wartość 55, a zwróciła na %llu", heap_get_largest_used_block_size());
                test_error(pointer_unallocated == get_pointer_type(ptr3), "Funkcja get_pointer_type() powinna zwrócić wskaźnik typu pointer_unallocated, a zwróciła %d", get_pointer_type(ptr3));

                void *ptr = heap_malloc(10408);
                test_error(ptr != NULL, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego bloku pamięci, a zwróciła NULL");
                test_error(pointer_valid == get_pointer_type(ptr), "Funkcja heap_malloc() powinna zwrócić wskaźnik typu pointer_valid, a zwróciła %d", get_pointer_type(ptr));
                test_error(ptr1 == ptr, "Funkcja heap_malloc() powinna zwrócić adres przydzielonego pierwszego bloku pamięci");
                test_error(heap_get_largest_used_block_size() == 10408, "Funkcja heap_validate() powinna zwrócić wartość 10408, a zwróciła na %llu", heap_get_largest_used_block_size());

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 48: Sprawdzanie poprawności działania funkcji heap_malloc i heap_free
//
void UTEST48(void)
{
    // informacje o teście
    test_start(48, "Sprawdzanie poprawności działania funkcji heap_malloc i heap_free", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                  srand (time(NULL));

                  int status = heap_setup();
                  test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                  char *ptr[2299];
                  int ptr_state[2299] = {0};

                  int is_allocated = 0;

                  for (int i = 0; i < 2299; ++i)
                  {
                    if (rand() % 100 < 66)
                    {
                      for (int j = 0; j < 2299; ++j)
                        if (ptr_state[j] == 0)
                        {
                          ptr_state[j] = 1;
                          ptr[j] = heap_malloc(rand() % 100 + 50);
                          is_allocated++;
                          break;
                        }
                    }
                    else
                    {
                      if (is_allocated)
                      {
                        int to_free = rand() % is_allocated;
                         for (int j = 0; j < 2299; ++j)
                          {
                              if (ptr_state[j] == 1 && !to_free)
                              {
                                ptr_state[j] = 0;
                                is_allocated--;
                                heap_free(ptr[j]);
                                break;
                              }        
                              to_free--;
                          }
                      }
                    }
                  }

                  for (int j = 0; j < 2299; ++j)
                    if (ptr_state[j] == 1)
                      heap_free(ptr[j]);

                  test_error(heap_get_largest_used_block_size() == 0, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 0, a zwróciła na %llu", heap_get_largest_used_block_size());

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 49: Sprawdzanie poprawności działania funkcji get_pointer_type
//
void UTEST49(void)
{
    // informacje o teście
    test_start(49, "Sprawdzanie poprawności działania funkcji get_pointer_type", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    

                test_error(get_pointer_type(NULL) == pointer_null, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_null, a zwróciła %d", get_pointer_type(NULL));

            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 50: Sprawdzanie poprawności działania funkcji get_pointer_type
//
void UTEST50(void)
{
    // informacje o teście
    test_start(50, "Sprawdzanie poprawności działania funkcji get_pointer_type", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                test_error(get_pointer_type(NULL) == pointer_null, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_null, a zwróciła %d", get_pointer_type(NULL));

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
        
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 51: Sprawdzanie poprawności działania funkcji get_pointer_type
//
void UTEST51(void)
{
    // informacje o teście
    test_start(51, "Sprawdzanie poprawności działania funkcji get_pointer_type", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr = heap_malloc(114);

                test_error(get_pointer_type(ptr) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr));

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 52: Sprawdzanie poprawności działania funkcji get_pointer_type
//
void UTEST52(void)
{
    // informacje o teście
    test_start(52, "Sprawdzanie poprawności działania funkcji get_pointer_type", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr = heap_malloc(118);
                char *ptr1 = heap_malloc(1152);

                test_error(get_pointer_type(ptr) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr));
                test_error(get_pointer_type(ptr1) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr1));

                heap_free(ptr);

                test_error(get_pointer_type(ptr) == pointer_unallocated, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_unallocated, a zwróciła %d", get_pointer_type(ptr));
                test_error(get_pointer_type(ptr + 30) == pointer_unallocated, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_unallocated, a zwróciła %d", get_pointer_type(ptr + 30));
                test_error(get_pointer_type(ptr + 118) == pointer_unallocated, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_unallocated, a zwróciła %d", get_pointer_type(ptr + 118));
                test_error(get_pointer_type(ptr - 1) == pointer_unallocated, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_unallocated, a zwróciła %d", get_pointer_type(ptr - 1));

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 53: Sprawdzanie poprawności działania funkcji get_pointer_type
//
void UTEST53(void)
{
    // informacje o teście
    test_start(53, "Sprawdzanie poprawności działania funkcji get_pointer_type", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr = heap_malloc(1304);
                char *ptr1 = heap_malloc(1312);

                test_error(get_pointer_type(ptr) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr));
                test_error(get_pointer_type(ptr1) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr1));

                test_error(get_pointer_type(ptr + 1304) == pointer_inside_fences, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_inside_fences, a zwróciła %d", get_pointer_type(ptr + 1304));
                test_error(get_pointer_type(ptr - 1) == pointer_inside_fences, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_inside_fences, a zwróciła %d", get_pointer_type(ptr - 1));

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 54: Sprawdzanie poprawności działania funkcji get_pointer_type
//
void UTEST54(void)
{
    // informacje o teście
    test_start(54, "Sprawdzanie poprawności działania funkcji get_pointer_type", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr = heap_malloc(1368);

                test_error(get_pointer_type(ptr) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr));

                for (int i = 1; i < 1368; ++i)
                    test_error(get_pointer_type(ptr + i) == pointer_inside_data_block, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_inside_data_block, a zwróciła %d", get_pointer_type(ptr + i));

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 55: Sprawdzanie poprawności działania funkcji get_pointer_type
//
void UTEST55(void)
{
    // informacje o teście
    test_start(55, "Sprawdzanie poprawności działania funkcji get_pointer_type", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr = heap_malloc(856);
                char *ptr1 = heap_malloc(824);

                test_error(get_pointer_type(ptr1) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr1));

                int i = -1;

                while (get_pointer_type(ptr1 + i) == pointer_inside_fences) --i;

                test_error(get_pointer_type(ptr1 + i) == pointer_control_block, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_control_block, a zwróciła %d", get_pointer_type(ptr + i));

                while (get_pointer_type(ptr1 + i) == pointer_control_block) --i;

                test_error(get_pointer_type(ptr1 + i) == pointer_inside_fences, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_inside_fences, a zwróciła %d", get_pointer_type(ptr + i));

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 56: Sprawdzanie poprawności działania funkcji get_pointer_type
//
void UTEST56(void)
{
    // informacje o teście
    test_start(56, "Sprawdzanie poprawności działania funkcji get_pointer_type", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr = heap_malloc(1544);
                char *ptr1 = heap_malloc(1104);

                test_error(get_pointer_type(ptr1) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr1));

                int i = 1544 + 1;

                while (get_pointer_type(ptr + i) == pointer_inside_fences) ++i;

                test_error(get_pointer_type(ptr + i) == pointer_control_block, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_control_block, a zwróciła %d", get_pointer_type(ptr + i));

                while (get_pointer_type(ptr + i) == pointer_control_block) ++i;

                test_error(get_pointer_type(ptr + i) == pointer_inside_fences, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_inside_fences, a zwróciła %d", get_pointer_type(ptr + i));

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 57: Sprawdzanie poprawności działania funkcji get_pointer_type
//
void UTEST57(void)
{
    // informacje o teście
    test_start(57, "Sprawdzanie poprawności działania funkcji get_pointer_type", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr = heap_malloc(1528);
                char *ptr1 = heap_malloc(1224);

                test_error(get_pointer_type(ptr) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr));
                test_error(get_pointer_type(ptr1) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr1));

                int i = -1, fences_size_before = 0, fences_size_after = 0;

                while (get_pointer_type(ptr + i) == pointer_inside_fences) 
                {
                    fences_size_before++;
                    --i;
                }

                i = 1528 + 1;

                while (get_pointer_type(ptr + i) == pointer_inside_fences)             
                {
                    fences_size_after++;
                    ++i;
                }

                heap_free(ptr);

                for (int i = -fences_size_before; i < 1528 + fences_size_after; ++i)
                    test_error(get_pointer_type(ptr + i) == pointer_unallocated, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_unallocated, a zwróciła %d", get_pointer_type(ptr + i));

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                

            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 58: Sprawdzanie poprawności działania funkcji get_pointer_type
//
void UTEST58(void)
{
    // informacje o teście
    test_start(58, "Sprawdzanie poprawności działania funkcji get_pointer_type", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr = heap_calloc(104, 139);

                test_error(get_pointer_type(ptr) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr));

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
        
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 59: Sprawdzanie poprawności działania funkcji get_pointer_type
//
void UTEST59(void)
{
    // informacje o teście
    test_start(59, "Sprawdzanie poprawności działania funkcji get_pointer_type", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr = heap_calloc(1408, 20);
                char *ptr1 = heap_calloc(1096, 20);

                test_error(get_pointer_type(ptr) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr));
                test_error(get_pointer_type(ptr1) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr1));

                heap_free(ptr);

                test_error(get_pointer_type(ptr) == pointer_unallocated, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_unallocated, a zwróciła %d", get_pointer_type(ptr));
                test_error(get_pointer_type(ptr + 23) == pointer_unallocated, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_unallocated, a zwróciła %d", get_pointer_type(ptr + 23));
                test_error(get_pointer_type(ptr + 28160) == pointer_unallocated, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_unallocated, a zwróciła %d", get_pointer_type(ptr + 1408));
                test_error(get_pointer_type(ptr - 1) == pointer_unallocated, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_unallocated, a zwróciła %d", get_pointer_type(ptr - 1));

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
                
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 60: Sprawdzanie poprawności działania funkcji get_pointer_type
//
void UTEST60(void)
{
    // informacje o teście
    test_start(60, "Sprawdzanie poprawności działania funkcji get_pointer_type", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr = heap_calloc(944, 20);
                char *ptr1 = heap_calloc(1000, 20);


                test_error(get_pointer_type(ptr) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr));
                test_error(get_pointer_type(ptr1) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr1));

                test_error(get_pointer_type(ptr + 18880) == pointer_inside_fences, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_inside_fences, a zwróciła %d", get_pointer_type(ptr + 944));
                test_error(get_pointer_type(ptr - 1) == pointer_inside_fences, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_inside_fences, a zwróciła %d", get_pointer_type(ptr - 1));

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);

            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 61: Sprawdzanie poprawności działania funkcji get_pointer_type
//
void UTEST61(void)
{
    // informacje o teście
    test_start(61, "Sprawdzanie poprawności działania funkcji get_pointer_type", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr = heap_calloc(816, 10);

                test_error(get_pointer_type(ptr) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr));

                for (int i = 1; i < 8160; ++i)
                    test_error(get_pointer_type(ptr + i) == pointer_inside_data_block, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_inside_data_block, a zwróciła %d", get_pointer_type(ptr + i));

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 62: Sprawdzanie poprawności działania funkcji get_pointer_type
//
void UTEST62(void)
{
    // informacje o teście
    test_start(62, "Sprawdzanie poprawności działania funkcji get_pointer_type", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr = heap_calloc(1256, 19);
                char *ptr1 = heap_calloc(1592, 19);

                test_error(get_pointer_type(ptr1) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr1));

                int i = -1;

                while (get_pointer_type(ptr1 + i) == pointer_inside_fences) --i;

                test_error(get_pointer_type(ptr1 + i) == pointer_control_block, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_control_block, a zwróciła %d", get_pointer_type(ptr + i));

                while (get_pointer_type(ptr1 + i) == pointer_control_block) --i;

                test_error(get_pointer_type(ptr1 + i) == pointer_inside_fences, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_inside_fences, a zwróciła %d", get_pointer_type(ptr + i));

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 63: Sprawdzanie poprawności działania funkcji heap_validate
//
void UTEST63(void)
{
    // informacje o teście
    test_start(63, "Sprawdzanie poprawności działania funkcji heap_validate", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_validate();
                test_error(status == 2, "Funkcja heap_validate() powinna zwrócić wartość 2, a zwróciła na %d", status);
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 64: Sprawdzanie poprawności działania funkcji heap_validate
//
void UTEST64(void)
{
    // informacje o teście
    test_start(64, "Sprawdzanie poprawności działania funkcji heap_validate", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr = heap_malloc(143);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                heap_free(ptr);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);
     
                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 65: Sprawdzanie poprawności działania funkcji heap_validate
//
void UTEST65(void)
{
    // informacje o teście
    test_start(65, "Sprawdzanie poprawności działania funkcji heap_validate", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr = heap_malloc(136);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                int i = -1;

                while (get_pointer_type(ptr + i) == pointer_inside_fences)
                {
                    char temp = ptr[i];
                    ptr[i] += 16;

                    status = heap_validate();
                    test_error(status == 1, "Funkcja heap_validate() powinna zwrócić wartość 1, a zwróciła na %d", status);

                    ptr[i] = temp;

                    status = heap_validate();
                    test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                    --i;
                }

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 66: Sprawdzanie poprawności działania funkcji heap_validate
//
void UTEST66(void)
{
    // informacje o teście
    test_start(66, "Sprawdzanie poprawności działania funkcji heap_validate", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr = heap_malloc(110);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                int i = 110;

                while (get_pointer_type(ptr + i) == pointer_inside_fences)
                {
                    char temp = ptr[i];
                    ptr[i] += 17;

                    status = heap_validate();
                    test_error(status == 1, "Funkcja heap_validate() powinna zwrócić wartość 1, a zwróciła na %d", status);

                    ptr[i] = temp;

                    status = heap_validate();
                    test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                    ++i;
                }

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 67: Sprawdzanie poprawności działania funkcji heap_validate
//
void UTEST67(void)
{
    // informacje o teście
    test_start(67, "Sprawdzanie poprawności działania funkcji heap_validate", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr = heap_malloc(140);
                char *ptr1 = heap_malloc(140);

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                test_error(get_pointer_type(ptr) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr));
                test_error(get_pointer_type(ptr1) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr1));

                int i = -1;

                while (get_pointer_type(ptr1 + i) == pointer_inside_fences) --i;

                while (get_pointer_type(ptr1 + i) == pointer_control_block)
                {
                    char temp = ptr1[i];
                    ptr1[i] += 10;

                    status = heap_validate();
                    test_error(status == 3, "Funkcja heap_validate() powinna zwrócić wartość 3, a zwróciła na %d", status);

                    ptr1[i] = temp;

                    status = heap_validate();
                    test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                    --i;
                }

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 68: Sprawdzanie poprawności działania funkcji heap_validate
//
void UTEST68(void)
{
    // informacje o teście
    test_start(68, "Sprawdzanie poprawności działania funkcji heap_validate", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                 int status = heap_setup();
                 test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 size_t mem_sizes[] = {994, 313, 779, 128, 854, 771, 165, 286, 402, 334, 722, 505, 800, 329, 974, 174, 649, 481, 501, 141, 668, 731, 223, 690};
                 size_t mem_units[] = {4, 1, 3, 2, 5, 3, 4, 5, 2, 5, 5, 5, 4, 1, 2, 5, 5, 1, 5, 3, 5, 3, 1, 4};
                 char *ptrs[24];

                 for (int i = 0; i < 24; ++i)
                     ptrs[i] = heap_calloc(mem_sizes[i], mem_units[i]);

                 ptrs[10][-1] += 7;
                 
                 status = heap_validate();
                 test_error(status == 1, "Funkcja heap_validate() powinna zwrócić wartość 1, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
             
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 69: Sprawdzanie poprawności działania funkcji heap_validate
//
void UTEST69(void)
{
    // informacje o teście
    test_start(69, "Sprawdzanie poprawności działania funkcji heap_validate", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                 int status = heap_setup();
                 test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 size_t mem_sizes[] = {736, 363, 221, 873, 675, 866, 796, 695, 825, 724, 264, 946, 425, 559, 489, 526, 355, 554, 927, 228, 852, 524, 490, 353, 671, 257, 792, 157, 615, 363, 438, 233, 993, 879, 583};
                 size_t mem_units[] = {3, 1, 5, 5, 3, 4, 5, 5, 1, 4, 5, 5, 3, 1, 3, 4, 4, 2, 2, 4, 3, 1, 3, 4, 1, 2, 4, 2, 4, 4, 2, 3, 2, 3, 2};
                 char *ptrs[35];

                 for (int i = 0; i < 35; ++i)
                     ptrs[i] = heap_calloc(mem_sizes[i], mem_units[i]);

                 ptrs[32][mem_sizes[32] * mem_units[32]] += 8;

                 status = heap_validate();
                 test_error(status == 1, "Funkcja heap_validate() powinna zwrócić wartość 1, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
             
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 70: Sprawdzanie poprawności działania funkcji heap_validate
//
void UTEST70(void)
{
    // informacje o teście
    test_start(70, "Sprawdzanie poprawności działania funkcji heap_validate", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    rldebug_heap_set_global_limit(0);
    
    //
    // -----------
    //
    
                 int status = heap_setup();
                 test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 size_t mem_sizes[] = {570, 227, 877, 187, 721, 227, 190, 410, 766, 832, 869, 844, 627, 298, 793, 439, 246, 100, 629, 578, 546, 654, 318};
                 size_t mem_units[] = {3, 2, 2, 1, 3, 4, 4, 4, 3, 1, 5, 1, 4, 1, 3, 4, 1, 1, 1, 3, 1, 2, 3};
                 char *ptrs[23];

                 for (int i = 0; i < 23; ++i)
                     ptrs[i] = heap_calloc(mem_sizes[i], mem_units[i]);

                 int i = -1;

                 while (get_pointer_type(ptrs[13] + i) == pointer_inside_fences) --i;

                 ptrs[13][i - 7] += 9;

                 status = heap_validate();
                 test_error(status == 3, "Funkcja heap_validate() powinna zwrócić wartość 3, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
             
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 71: Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku przekazania do niej wartości 0
//
void UTEST71(void)
{
    // informacje o teście
    test_start(71, "Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku przekazania do niej wartości 0", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr = heap_realloc(NULL, 0);

                test_error(ptr == NULL, "Funkcja heap_realloc() powinna zwrócić wartość NULL");

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 72: Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku niezainicjowania sterty
//
void UTEST72(void)
{
    // informacje o teście
    test_start(72, "Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku niezainicjowania sterty", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    

                void *ptr = heap_realloc(NULL, 111);

                test_error(ptr == NULL, "Funkcja heap_realloc() powinna zwrócić wartość NULL");
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 73: Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku przekazania do niej poprawnej wartości
//
void UTEST73(void)
{
    // informacje o teście
    test_start(73, "Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku przekazania do niej poprawnej wartości", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr = heap_realloc(NULL, 167);

                test_error(ptr != NULL, "Funkcja heap_realloc() powinna zwrócić adres pamięci przydzielonej użytkownikowi");
                test_error(get_pointer_type(ptr) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr));

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 74: Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku przekazania do niej wartości 0
//
void UTEST74(void)
{
    // informacje o teście
    test_start(74, "Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku przekazania do niej wartości 0", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                void *ptr = heap_realloc(NULL, 159);

                test_error(ptr != NULL, "Funkcja heap_realloc() powinna zwrócić adres pamięci przydzielonej użytkownikowi");
                test_error(get_pointer_type(ptr) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr));

                void *ptr1 = heap_realloc(ptr, 0);

                test_error(ptr1 == NULL, "Funkcja heap_realloc() powinna zwrócić wartość NULL");
                test_error(get_pointer_type(ptr) == pointer_unallocated, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_unallocated, a zwróciła %d", get_pointer_type(ptr));

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 75: Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku przekazania do niej niewłaściwego wskaźnika
//
void UTEST75(void)
{
    // informacje o teście
    test_start(75, "Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku przekazania do niej niewłaściwego wskaźnika", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                char *ptr = heap_realloc(NULL, 198);

                test_error(ptr != NULL, "Funkcja heap_realloc() powinna zwrócić adres pamięci przydzielonej użytkownikowi");
                test_error(get_pointer_type(ptr) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr));

                void *ptr1 = heap_realloc(ptr + 33, 198);

                test_error(ptr1 == NULL, "Funkcja heap_realloc() powinna zwrócić wartość NULL");
                test_error(get_pointer_type(ptr) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr));
        
                ptr1 = heap_realloc(ptr - 42, 198);

                test_error(ptr1 == NULL, "Funkcja heap_realloc() powinna zwrócić wartość NULL");
                test_error(get_pointer_type(ptr) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr));

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 76: Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania zmniejszenia rozmiaru pamięci
//
void UTEST76(void)
{
    // informacje o teście
    test_start(76, "Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania zmniejszenia rozmiaru pamięci", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                int status = heap_setup();
                test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                unsigned char values[117];
                
                for (int i = 0; i < 117; ++i)
                    values[i] = rand() % 255;

                char *ptr = heap_realloc(NULL, 117);
                char *ptr2 = heap_realloc(NULL, 117);

                test_error(ptr != NULL, "Funkcja heap_realloc() powinna zwrócić adres pamięci przydzielonej użytkownikowi");
                test_error(get_pointer_type(ptr) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr));

                test_error(ptr2 != NULL, "Funkcja heap_realloc() powinna zwrócić adres pamięci przydzielonej użytkownikowi");
                test_error(get_pointer_type(ptr2) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr2));

                memcpy(ptr, values, 117);

                void *ptr1 = heap_realloc(ptr, 52);

                test_error(ptr1 == ptr, "Funkcja heap_realloc() powinna zwrócić ten sam adres, który został do niej przekazany");
                test_error(memcmp(ptr, values, 52) == 0, "Funkcja heap_realloc() nie powinna modyfikować obszaru pamięci użytkownika");

                test_error(get_pointer_type(ptr + 52) == pointer_inside_fences, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_inside_fences, a zwróciła %d", get_pointer_type(ptr + 52));

                 int i = 52;

                char temp = ptr[i];
                ptr[i] += 10;

                status = heap_validate();
                test_error(status == 1, "Funkcja heap_validate() powinna zwrócić wartość 1, a zwróciła na %d", status);

                ptr[i] = temp;

                status = heap_validate();
                test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                while (get_pointer_type(ptr + i) == pointer_inside_fences) ++i;

                test_error(get_pointer_type(ptr + i) == pointer_unallocated, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_unallocated, a zwróciła %d", get_pointer_type(ptr + i));

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
            
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 77: Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania tego samego rozmiaru pamięci, który zajmował poprzednio
//
void UTEST77(void)
{
    // informacje o teście
    test_start(77, "Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania tego samego rozmiaru pamięci, który zajmował poprzednio", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                 int status = heap_setup();
                 test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 unsigned char values[179];

                 for (int i = 0; i < 179; ++i)
                     values[i] = rand() % 255;

                 char *ptr = heap_realloc(NULL, 179);
                 char *ptr2 = heap_realloc(NULL, 179);

                 test_error(ptr != NULL, "Funkcja heap_realloc() powinna zwrócić adres pamięci przydzielonej użytkownikowi");
                 test_error(get_pointer_type(ptr) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr));

                 test_error(ptr2 != NULL, "Funkcja heap_realloc() powinna zwrócić adres pamięci przydzielonej użytkownikowi");
                 test_error(get_pointer_type(ptr2) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr2));

                 memcpy(ptr, values, 179);

                 void *ptr1 = heap_realloc(ptr, 179);

                 test_error(ptr1 == ptr, "Funkcja heap_realloc() powinna zwrócić ten sam adres, który został do niej przekazany");
                 test_error(memcmp(ptr, values, 179) == 0, "Funkcja heap_realloc() nie powinna modyfikować obszaru pamięci użytkownika");

                 test_error(get_pointer_type(ptr + 179) == pointer_inside_fences, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_inside_fences, a zwróciła %d", get_pointer_type(ptr + 179));

                  int i = 179;

                 char temp = ptr[i];
                 ptr[i] += 10;

                 status = heap_validate();
                 test_error(status == 1, "Funkcja heap_validate() powinna zwrócić wartość 1, a zwróciła na %d", status);

                 ptr[i] = temp;

                 status = heap_validate();
                 test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);

             
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 78: Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania większego rozmiaru pamięci, który zajmował poprzednio
//
void UTEST78(void)
{
    // informacje o teście
    test_start(78, "Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania większego rozmiaru pamięci, który zajmował poprzednio", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                 int status = heap_setup();
                 test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 unsigned char values[191];

                 for (int i = 0; i < 191; ++i)
                     values[i] = rand() % 255;

                 char *ptr = heap_realloc(NULL, 191);

                 test_error(ptr != NULL, "Funkcja heap_realloc() powinna zwrócić adres pamięci przydzielonej użytkownikowi");
                 test_error(get_pointer_type(ptr) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr));

                 memcpy(ptr, values, 191);

                 void *ptr1 = heap_realloc(ptr, 860);

                 test_error(ptr1 == ptr, "Funkcja heap_realloc() powinna zwrócić ten sam adres, który został do niej przekazany");
                 test_error(memcmp(ptr, values, 191) == 0, "Funkcja heap_realloc() nie powinna modyfikować obszaru pamięci użytkownika");

                 test_error(get_pointer_type(ptr + 860) == pointer_inside_fences, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_inside_fences, a zwróciła %d", get_pointer_type(ptr + 179));

                  int i = 860;

                 char temp = ptr[i];
                 ptr[i] += 10;

                 status = heap_validate();
                 test_error(status == 1, "Funkcja heap_validate() powinna zwrócić wartość 1, a zwróciła na %d", status);

                 ptr[i] = temp;

                 status = heap_validate();
                 test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
             
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 79: Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania większego rozmiaru pamięci, który zajmował poprzednio
//
void UTEST79(void)
{
    // informacje o teście
    test_start(79, "Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania większego rozmiaru pamięci, który zajmował poprzednio", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                 int status = heap_setup();
                 test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 unsigned char values[195];

                 for (int i = 0; i < 195; ++i)
                     values[i] = rand() % 255;

                 char *ptr = heap_realloc(NULL, 195);

                 test_error(ptr != NULL, "Funkcja heap_realloc() powinna zwrócić adres pamięci przydzielonej użytkownikowi");
                 test_error(get_pointer_type(ptr) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr));

                 memcpy(ptr, values, 195);

                 void *ptr1 = heap_realloc(ptr, 11367135);

                 test_error(ptr1 == ptr, "Funkcja heap_realloc() powinna zwrócić ten sam adres, który został do niej przekazany");
                 test_error(memcmp(ptr, values, 195) == 0, "Funkcja heap_realloc() nie powinna modyfikować obszaru pamięci użytkownika");

                 test_error(get_pointer_type(ptr + 11367135) == pointer_inside_fences, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_inside_fences, a zwróciła %d", get_pointer_type(ptr + 179));

                  int i = 11367135;

                 char temp = ptr[i];
                 ptr[i] += 10;

                 status = heap_validate();
                 test_error(status == 1, "Funkcja heap_validate() powinna zwrócić wartość 1, a zwróciła na %d", status);

                 ptr[i] = temp;

                 status = heap_validate();
                 test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
             
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 80: Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania większego rozmiaru pamięci, który zajmował poprzednio
//
void UTEST80(void)
{
    // informacje o teście
    test_start(80, "Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania większego rozmiaru pamięci, który zajmował poprzednio", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                  int status = heap_setup();
                  test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                  unsigned char values[146];

                  for (int i = 0; i < 146; ++i)
                      values[i] = rand() % 255;

                  char *ptr1 = heap_realloc(NULL, 146);
                  char *ptr2 = heap_realloc(NULL, 1590);
                  char *ptr3 = heap_realloc(NULL, 140);
                  char *ptr4 = heap_realloc(NULL, 158);

                  test_error(get_pointer_type(ptr1) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr1));
                  test_error(get_pointer_type(ptr2) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr2));
                  test_error(get_pointer_type(ptr3) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr3));
                  test_error(get_pointer_type(ptr4) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr4));

                  heap_free(ptr2);
                
                  ptr2 = heap_realloc(NULL, 146);
                
                  heap_free(ptr3);

                  memcpy(ptr2, values, 146);

                  char *ptr5 = heap_realloc(ptr2, 1751);

                  test_error(ptr5 == ptr2, "Funkcja heap_realloc() powinna zwrócić ten sam adres, który został do niej przekazany");
                  test_error(memcmp(ptr2, values, 146) == 0, "Funkcja heap_realloc() nie powinna modyfikować obszaru pamięci użytkownika");

                  test_error(get_pointer_type(ptr2 + 1751) == pointer_inside_fences, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_inside_fences, a zwróciła %d", get_pointer_type(ptr2 + 1751));

                  status = heap_validate();
                  test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
              
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 81: Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania większego rozmiaru pamięci, który zajmował poprzednio
//
void UTEST81(void)
{
    // informacje o teście
    test_start(81, "Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania większego rozmiaru pamięci, który zajmował poprzednio", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                  int status = heap_setup();
                  test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                  unsigned char values[101];

                  for (int i = 0; i < 101; ++i)
                      values[i] = rand() % 255;

                  char *ptr1 = heap_realloc(NULL, 101);
                  char *ptr2 = heap_realloc(NULL, 1685);
                  char *ptr3 = heap_realloc(NULL, 137);
                  char *ptr4 = heap_realloc(NULL, 134);

                  test_error(get_pointer_type(ptr1) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr1));
                  test_error(get_pointer_type(ptr2) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr2));
                  test_error(get_pointer_type(ptr3) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr3));
                  test_error(get_pointer_type(ptr4) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr4));

                  heap_free(ptr2);

                  ptr2 = heap_realloc(NULL, 101);

                  heap_free(ptr3);

                  memcpy(ptr2, values, 101);

                  char *ptr5 = heap_realloc(ptr2, 1714);

                  test_error(ptr5 == ptr2, "Funkcja heap_realloc() powinna zwrócić ten sam adres, który został do niej przekazany");
                  test_error(memcmp(ptr2, values, 101) == 0, "Funkcja heap_realloc() nie powinna modyfikować obszaru pamięci użytkownika");

                  test_error(get_pointer_type(ptr2 + 1714) == pointer_inside_fences, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_inside_fences, a zwróciła %d", get_pointer_type(ptr2 + 1714));

                  status = heap_validate();
                  test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
              
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 82: Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania większego rozmiaru pamięci, który zajmował poprzednio
//
void UTEST82(void)
{
    // informacje o teście
    test_start(82, "Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania większego rozmiaru pamięci, który zajmował poprzednio", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                  int status = heap_setup();
                  test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                  unsigned char values[109];

                  for (int i = 0; i < 109; ++i)
                      values[i] = rand() % 255;

                  char *ptr1 = heap_realloc(NULL, 109);
                  char *ptr2 = heap_realloc(NULL, 1197);
                  char *ptr3 = heap_realloc(NULL, 138);
                  char *ptr4 = heap_realloc(NULL, 129);

                  test_error(get_pointer_type(ptr1) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr1));
                  test_error(get_pointer_type(ptr2) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr2));
                  test_error(get_pointer_type(ptr3) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr3));
                  test_error(get_pointer_type(ptr4) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr4));

                  heap_free(ptr2);

                  ptr2 = heap_realloc(NULL, 109);

                  heap_free(ptr3);

                  memcpy(ptr2, values, 109);

                  char *ptr5 = heap_realloc(ptr2, 4209);

                  test_error(ptr5 != ptr2, "Funkcja heap_realloc() powinna zwrócić ten sam adres, który został do niej przekazany");
                  test_error(memcmp(ptr5, values, 109) == 0, "Funkcja heap_realloc() nie powinna modyfikować obszaru pamięci użytkownika");

                  test_error(get_pointer_type(ptr5 + 4209) == pointer_inside_fences, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_inside_fences, a zwróciła %d", get_pointer_type(ptr5 + 4209));
                  test_error(get_pointer_type(ptr2) == pointer_unallocated, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_unallocated, a zwróciła %d", get_pointer_type(ptr2));

                  status = heap_validate();
                  test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
              
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 83: Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania większego rozmiaru pamięci, który zajmował poprzednio
//
void UTEST83(void)
{
    // informacje o teście
    test_start(83, "Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania większego rozmiaru pamięci, który zajmował poprzednio", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                  int status = heap_setup();
                  test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                  unsigned char values[200];

                  for (int i = 0; i < 200; ++i)
                      values[i] = rand() % 255;

                  char *ptr1 = heap_realloc(NULL, 200);
                  char *ptr2 = heap_realloc(NULL, 1558);
                  char *ptr3 = heap_realloc(NULL, 145);
                  char *ptr4 = heap_realloc(NULL, 152);

                  test_error(get_pointer_type(ptr1) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr1));
                  test_error(get_pointer_type(ptr2) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr2));
                  test_error(get_pointer_type(ptr3) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr3));
                  test_error(get_pointer_type(ptr4) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr4));

                  heap_free(ptr2);

                  ptr2 = heap_realloc(NULL, 200);

                  heap_free(ptr3);

                  memcpy(ptr2, values, 200);

                  char *ptr5 = heap_realloc(ptr2, 67800947);

                  test_error(ptr5 != ptr2, "Funkcja heap_realloc() powinna zwrócić ten sam adres, który został do niej przekazany");
                  test_error(memcmp(ptr2, values, 200) == 0, "Funkcja heap_realloc() nie powinna modyfikować obszaru pamięci użytkownika");

                  test_error(get_pointer_type(ptr2) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr2));
                  test_error(get_pointer_type(ptr5) == pointer_null, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_null, a zwróciła %d", get_pointer_type(ptr5));

                  status = heap_validate();
                  test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
              
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 84: Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania większego rozmiaru pamięci, który zajmował poprzednio
//
void UTEST84(void)
{
    // informacje o teście
    test_start(84, "Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania większego rozmiaru pamięci, który zajmował poprzednio", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                  int status = heap_setup();
                  test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                  unsigned char values[132];

                  for (int i = 0; i < 132; ++i)
                      values[i] = rand() % 255;

                  char *ptr1 = heap_realloc(NULL, 159);
                  char *ptr2 = heap_realloc(NULL, 1623);
                  char *ptr3 = heap_realloc(NULL, 176);
                  char *ptr4 = heap_realloc(NULL, 132);

                  test_error(get_pointer_type(ptr1) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr1));
                  test_error(get_pointer_type(ptr2) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr2));
                  test_error(get_pointer_type(ptr3) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr3));
                  test_error(get_pointer_type(ptr4) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr4));

                  memcpy(ptr4, values, 132);

                  char *ptr5 = heap_realloc(ptr4, 67987788);

                  test_error(ptr5 == NULL, "Funkcja heap_realloc() powinna zwrócić NULL");
                  test_error(memcmp(ptr4, values, 132) == 0, "Funkcja heap_realloc() nie powinna modyfikować obszaru pamięci użytkownika");

                  test_error(get_pointer_type(ptr4) == pointer_valid, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_valid, a zwróciła %d", get_pointer_type(ptr2));
                  test_error(get_pointer_type(ptr5) == pointer_null, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_null, a zwróciła %d", get_pointer_type(ptr5));

                  test_error(get_pointer_type(ptr4 + 132) == pointer_inside_fences, "Funkcja get_pointer_type() powinna zwrócić wartość pointer_inside_fences, a zwróciła %d", get_pointer_type(ptr4 + 132));

                  status = heap_validate();
                  test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);
              
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 85: Sprawdzanie poprawności działania funkcji heap_realloc
//
void UTEST85(void)
{
    // informacje o teście
    test_start(85, "Sprawdzanie poprawności działania funkcji heap_realloc", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                   srand (time(NULL));

                   int status = heap_setup();
                   test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                   char *ptr[3573];
                   int ptr_state[3573] = {0};

                   int is_allocated = 0;

                   for (int i = 0; i < 3573; ++i)
                   {
                     int rand_value = rand() % 100;
                     if (rand_value < 33)
                     {
                       for (int j = 0; j < 3573; ++j)
                         if (ptr_state[j] == 0)
                         {
                           ptr_state[j] = 1;
                           ptr[j] = heap_realloc(NULL, rand() % 1000 + 500);  
                           is_allocated++;
                           break;
                         }
                     }
                     else if (rand_value < 66)
                     {
                       if (is_allocated)
                       {
                          int to_reallocate = rand() % is_allocated;
                          for (int j = 0; j < 3573; ++j)
                           {
                               if (ptr_state[j] == 1 && !to_reallocate)
                               {
                                   if (rand() % 100 < 50)
                                       ptr[j] = heap_realloc(ptr[j], rand() % 1000 + 500);
                                   else
                                       ptr[j] = heap_realloc(ptr[j], rand() % 10000 + 5000);    
                                   break;
                               }
                               to_reallocate--;
                           }
                       }
                     }
                     else
                     {
                       if (is_allocated)
                       {
                         int to_free = rand() % is_allocated;
                          for (int j = 0; j < 3573; ++j)
                           {
                               if (ptr_state[j] == 1 && !to_free)
                               {
                                 ptr_state[j] = 0;
                                 is_allocated--;
                                 heap_realloc(ptr[j], 0);
                                 break;
                               }
                               to_free--;
                           }
                       }
                     }
                     status = heap_validate();
                     test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła na %d", status);           
                   }

                   for (int j = 0; j < 3573; ++j)
                     if (ptr_state[j] == 1)
                         heap_realloc(ptr[j], 0);

                   test_error(heap_get_largest_used_block_size() == 0, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 0, a zwróciła na %llu", heap_get_largest_used_block_size());

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);

             
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}

//
//  Test 86: Sprawdzanie poprawności działania funkcji heap_realloc, heap_malloc, heap_calloc i heap_free
//
void UTEST86(void)
{
    // informacje o teście
    test_start(86, "Sprawdzanie poprawności działania funkcji heap_realloc, heap_malloc, heap_calloc i heap_free", __LINE__);

    // uwarunkowanie zasobów - pamięci, itd...
    test_file_write_limit_setup(33554432);
    rldebug_reset_limits();
    
    //
    // -----------
    //
    
                   srand (time(NULL));

                   int status = heap_setup();
                   test_error(status == 0, "Funkcja heap_setup() powinna zwrócić wartość 0, a zwróciła na %d", status);

                   char *ptr[706];
                   int ptr_state[706] = {0};

                   int is_allocated = 0;

                   for (int i = 0; i < 706; ++i)
                   {
                     int rand_value = rand() % 100;
                     if (rand_value < 16)
                     {
                       for (int j = 0; j < 706; ++j)
                         if (ptr_state[j] == 0)
                         {
                           ptr_state[j] = 1;
                           ptr[j] = heap_realloc(NULL, rand() % 100 + 50);  
                           is_allocated++;
                           break;
                         }
                     }
                     else if (rand_value < 33)
                     {
                       if (is_allocated)
                       {
                          int to_reallocate = rand() % is_allocated;
                          for (int j = 0; j < 706; ++j)
                           {
                               if (ptr_state[j] == 1 && !to_reallocate)
                               {
                                   if (rand() % 100 < 50)
                                       ptr[j] = heap_realloc(ptr[j], rand() % 100 + 50);
                                   else
                                       ptr[j] = heap_realloc(ptr[j], rand() % 1000 + 500);    
                                   break;
                               }
                               to_reallocate--;
                           }
                       }
                     }
                     else if (rand_value < 50)
                     {
                       if (is_allocated)
                       {
                         int to_free = rand() % is_allocated;
                          for (int j = 0; j < 706; ++j)
                           {
                               if (ptr_state[j] == 1 && !to_free)
                               {
                                 ptr_state[j] = 0;
                                 is_allocated--;
                                 heap_realloc(ptr[j], 0);
                                 break;
                               }
                               to_free--;
                           }
                       }
                     }
                     else if (rand_value < 66)
                     {
                       for (int j = 0; j < 706; ++j)
                         if (ptr_state[j] == 0)
                         {
                           ptr_state[j] = 1;
                           ptr[j] = heap_malloc(rand() % 100 + 50);  
                           is_allocated++;
                           break;
                         }
                     }
                     else if (rand_value < 83)
                     {
                       for (int j = 0; j < 706; ++j)
                         if (ptr_state[j] == 0)
                         {
                           ptr_state[j] = 1;
                           ptr[j] = heap_calloc(rand() % 100 + 50, rand() % 10 + 1);  
                           is_allocated++;
                           break;
                         }
                     }     
                     else
                     {
                       if (is_allocated)
                       {
                         int to_free = rand() % is_allocated;
                          for (int j = 0; j < 706; ++j)
                           {
                               if (ptr_state[j] == 1 && !to_free)
                               {
                                 ptr_state[j] = 0;
                                 is_allocated--;
                                 heap_free(ptr[j]);
                                 break;
                               }
                               to_free--;
                           }
                       }
                     }
                     status = heap_validate();
                     test_error(status == 0, "Funkcja heap_validate() powinna zwrócić wartość 0, a zwróciła %d", status);           
                   }

                   for (int j = 0; j < 706; ++j)
                     if (ptr_state[j] == 1)
                         heap_realloc(ptr[j], 0);

                   test_error(heap_get_largest_used_block_size() == 0, "Funkcja heap_get_largest_used_block_size() powinna zwrócić wartość 0, a zwróciła na %llu", heap_get_largest_used_block_size());

                 status = custom_sbrk_check_fences_integrity();
                 test_error(status == 0, "Funkcja custom_sbrk_check_fences_integrity() powinna zwrócić wartość 0, a zwróciła na %d. Oznacza to, że alokator nadpisał pamięć, która nie została przydzielona przez system", status);

                 heap_clean();

                 uint64_t reserved_memory = custom_sbrk_get_reserved_memory();
                 test_error(reserved_memory == 0, "Funkcja custom_sbrk_get_reserved_memory() powinna zwrócić wartość 0, a zwróciła na %llu. Po wywołaniu funkcji heap_clean cała pamięć zarezerwowana przez alokator powinna być zwrócona do systemu", reserved_memory);

             
    //
    // -----------
    //

    // przywrócenie podstawowych parametów przydzielania zasobów (jeśli to tylko możliwe)
    rldebug_reset_limits();
    test_file_write_limit_restore();
    
    test_ok();
}




enum run_mode_t { rm_normal_with_rld = 0, rm_unit_test = 1, rm_main_test = 2 };

int __wrap_main(volatile int _argc, char** _argv, char** _envp)
{
    int volatile vargc = _argc;
    char ** volatile vargv = _argv, ** volatile venvp = _envp;
	volatile enum run_mode_t run_mode = rm_unit_test; // -1
	volatile int selected_test = -1;

    if (vargc > 1)
	{
	    char* smode = strtok(vargv[1], ",");
	    char* stest = strtok(NULL, "");
		char *errptr = NULL;
		run_mode = (enum run_mode_t)strtol(smode, &errptr, 10);
		if (*errptr == '\x0')
		{
			memmove(vargv + 1, vargv + 2, sizeof(char*) * (vargc - 1));
			vargc--;

			if (stest != NULL)
			{
			    int val = (int)strtol(stest, &errptr, 10);
			    if (*errptr == '\x0')
			        selected_test = val;
			}
		}
	}

    // printf("runmode=%d; selected_test=%d\n", run_mode, selected_test);

    // inicjuj testy jednostkowe
    unit_test_init(run_mode, "unit_test_v2.c");
    test_limit_init();
    rldebug_set_reported_severity_level(MSL_FAILURE);

    if (run_mode == rm_normal_with_rld)
    {
        // konfiguracja ograniczników
        rldebug_reset_limits();
        

        // uruchom funkcję main Studenta a potem wyświetl podsumowanie sterty i zasobów
        volatile int ret_code = rdebug_call_main(tested_main, vargc, vargv, venvp);

        rldebug_reset_limits();
        

        int leaks_detected = rldebug_show_leaked_resources(0);
        if (leaks_detected)
            raise(SIGHEAP);

        return ret_code;
    }

    
    if (run_mode == rm_unit_test)
    {
        test_title("Testy jednostkowe");

        void (*pfcn[])(void) =
        { 
            UTEST1, // Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku przekazania do niej wartości 0
            UTEST2, // Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni większej niż przestrzeń obecna na stercie
            UTEST3, // Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku pierwszej próby zaalokowania przestrzeni mniejszej niż przestrzeń obecna na stercie
            UTEST4, // Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni mniejszej niż przestrzeń obecna na stercie, w przypadku przydzielenia wcześniej pamięci użytkownikowi
            UTEST5, // Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni mniejszej niż przestrzeń obecna na stercie (żądana pamięć wymaga zwiększenia limitu pamięci przydzielonego przez system), w przypadku przydzielenia wcześniej pamięci użytkownikowi
            UTEST6, // Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni większej niż przestrzeń, która może być przydzielona przez system (żądana pamięć wymaga zwiększenia limitu pamięci przydzielonego przez system), w przypadku przydzielenia wcześniej pamięci użytkownikowi
            UTEST7, // Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeń mniejszej niż przestrzeń poprzednio zwolniona
            UTEST8, // Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni równej przestrzeni poprzednio zwolnionej
            UTEST9, // Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni mniejszej niż przestrzeń poprzednio zwolniona
            UTEST10, // Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni równej przestrzeń poprzednio zwolnionej
            UTEST11, // Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni większej niż przestrzeń poprzednio zwolnionej
            UTEST12, // Sprawdzanie poprawności działania funkcji heap_malloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni większej niż przestrzeń poprzednio zwolnionej
            UTEST13, // Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku przekazania do niej wartości 0
            UTEST14, // Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku przekazania do niej wartości 0
            UTEST15, // Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku przekazania do niej wartości 0
            UTEST16, // Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni większej niż przestrzeń obecna na stercie
            UTEST17, // Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni większej niż przestrzeń obecna na stercie
            UTEST18, // Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku pierwszej próby zaalokowania przestrzeni mniejszej niż przestrzeń obecna na stercie
            UTEST19, // Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku pierwszej próby zaalokowania przestrzeni mniejszej niż przestrzeń obecna na stercie
            UTEST20, // Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni mniejszej niż przestrzeń obecna na stercie, w przypadku przydzielenia wcześniej pamięci użytkownikowi
            UTEST21, // Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni mniejszej niż przestrzeń obecna na stercie (żądana pamięć wymaga zwiększenia limitu pamięci przydzielonego przez system), w przypadku przydzielenia wcześniej pamięci użytkownikowi
            UTEST22, // Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni większej niż przestrzeń, która może być przydzielona przez system (żądana pamięć wymaga zwiększenia limitu pamięci przydzielonego przez system), w przypadku przydzielenia wcześniej pamięci użytkownikowi
            UTEST23, // Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeń mniejszej niż przestrzeń poprzednio zwolniona
            UTEST24, // Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni równej przestrzeni poprzednio zwolnionej
            UTEST25, // Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni mniejszej niż przestrzeń poprzednio zwolniona
            UTEST26, // Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni równej przestrzeni poprzednio zwolnionej
            UTEST27, // Sprawdzanie poprawności działania funkcji heap_calloc - test sprawdza poprawność działania funkcji w przypadku próby zaalokowania przestrzeni większej niż przestrzeń poprzednio zwolnionej
            UTEST28, // Sprawdzanie poprawności działania funkcji heap_get_largest_used_block_size
            UTEST29, // Sprawdzanie poprawności działania funkcji heap_get_largest_used_block_size
            UTEST30, // Sprawdzanie poprawności działania funkcji heap_get_largest_used_block_size
            UTEST31, // Sprawdzanie poprawności działania funkcji heap_get_largest_used_block_size
            UTEST32, // Sprawdzanie poprawności działania funkcji heap_get_largest_used_block_size
            UTEST33, // Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku przekazania do niej niepoprawnego wskaźnika lub wskaźnika sprzed inicjalizacji sterty
            UTEST34, // Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku przekazania do niej niepoprawnego wskaźnika
            UTEST35, // Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku przekazania do niej niepoprawnego wskaźnika
            UTEST36, // Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku przekazania do niej niepoprawnego wskaźnika
            UTEST37, // Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku przekazania do niej niepoprawnego wskaźnika
            UTEST38, // Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia dwóch bloków pamięci sąsiadujących ze sobą
            UTEST39, // Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia dwóch bloków pamięci sąsiadujących ze sobą i zaalokowanie w tym miejscu nowej pamięci o rozmiarze sumy dwóch zwolnionych bloków
            UTEST40, // Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia dwóch bloków pamięci sąsiadujących ze sobą
            UTEST41, // Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia dwóch bloków pamięci sąsiadujących ze sobą i zaalokowanie w tym miejscu nowej pamięci o rozmiarze sumy dwóch zwolnionych bloków
            UTEST42, // Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia trzech bloków pamięci sąsiadujących ze sobą
            UTEST43, // Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia trzech bloków pamięci sąsiadujących ze sobą i zaalokowanie w tym miejscu nowej pamięci o rozmiarze sumy dwóch zwolnionych bloków
            UTEST44, // Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia bloku pamięci, zaalokowania w to miejsce mniejszego bloku, zwolnienia go i zaalokowania ponownie większego bloku
            UTEST45, // Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia trzech bloków pamięci sąsiadujących ze sobą i zaalokowanie w tym miejscu nowej pamięci o rozmiarze sumy dwóch zwolnionych bloków
            UTEST46, // Sprawdzanie poprawności działania funkcji heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia trzech bloków pamięci sąsiadujących ze sobą i zaalokowanie w tym miejscu nowej pamięci o rozmiarze sumy dwóch zwolnionych bloków
            UTEST47, // Sprawdzanie poprawności działania funkcji heap_malloc, heap_calloc i heap_free - test sprawdza poprawność działania funkcji w przypadku zwolnienia trzech bloków pamięci sąsiadujących ze sobą i zaalokowanie w tym miejscu nowej pamięci o rozmiarze sumy dwóch zwolnionych bloków
            UTEST48, // Sprawdzanie poprawności działania funkcji heap_malloc i heap_free
            UTEST49, // Sprawdzanie poprawności działania funkcji get_pointer_type
            UTEST50, // Sprawdzanie poprawności działania funkcji get_pointer_type
            UTEST51, // Sprawdzanie poprawności działania funkcji get_pointer_type
            UTEST52, // Sprawdzanie poprawności działania funkcji get_pointer_type
            UTEST53, // Sprawdzanie poprawności działania funkcji get_pointer_type
            UTEST54, // Sprawdzanie poprawności działania funkcji get_pointer_type
            UTEST55, // Sprawdzanie poprawności działania funkcji get_pointer_type
            UTEST56, // Sprawdzanie poprawności działania funkcji get_pointer_type
            UTEST57, // Sprawdzanie poprawności działania funkcji get_pointer_type
            UTEST58, // Sprawdzanie poprawności działania funkcji get_pointer_type
            UTEST59, // Sprawdzanie poprawności działania funkcji get_pointer_type
            UTEST60, // Sprawdzanie poprawności działania funkcji get_pointer_type
            UTEST61, // Sprawdzanie poprawności działania funkcji get_pointer_type
            UTEST62, // Sprawdzanie poprawności działania funkcji get_pointer_type
            UTEST63, // Sprawdzanie poprawności działania funkcji heap_validate
            UTEST64, // Sprawdzanie poprawności działania funkcji heap_validate
            UTEST65, // Sprawdzanie poprawności działania funkcji heap_validate
            UTEST66, // Sprawdzanie poprawności działania funkcji heap_validate
            UTEST67, // Sprawdzanie poprawności działania funkcji heap_validate
            UTEST68, // Sprawdzanie poprawności działania funkcji heap_validate
            UTEST69, // Sprawdzanie poprawności działania funkcji heap_validate
            UTEST70, // Sprawdzanie poprawności działania funkcji heap_validate
            UTEST71, // Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku przekazania do niej wartości 0
            UTEST72, // Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku niezainicjowania sterty
            UTEST73, // Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku przekazania do niej poprawnej wartości
            UTEST74, // Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku przekazania do niej wartości 0
            UTEST75, // Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku przekazania do niej niewłaściwego wskaźnika
            UTEST76, // Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania zmniejszenia rozmiaru pamięci
            UTEST77, // Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania tego samego rozmiaru pamięci, który zajmował poprzednio
            UTEST78, // Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania większego rozmiaru pamięci, który zajmował poprzednio
            UTEST79, // Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania większego rozmiaru pamięci, który zajmował poprzednio
            UTEST80, // Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania większego rozmiaru pamięci, który zajmował poprzednio
            UTEST81, // Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania większego rozmiaru pamięci, który zajmował poprzednio
            UTEST82, // Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania większego rozmiaru pamięci, który zajmował poprzednio
            UTEST83, // Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania większego rozmiaru pamięci, który zajmował poprzednio
            UTEST84, // Sprawdzanie poprawności działania funkcji heap_realloc - test sprawdza poprawność działania funkcji w przypadku żądania większego rozmiaru pamięci, który zajmował poprzednio
            UTEST85, // Sprawdzanie poprawności działania funkcji heap_realloc
            UTEST86, // Sprawdzanie poprawności działania funkcji heap_realloc, heap_malloc, heap_calloc i heap_free
            NULL
        };

        for (int idx = 0; pfcn[idx] != NULL && !test_get_session_termination_flag(); idx++)
        {
            if (selected_test == -1 || selected_test == idx + 1)
                pfcn[idx]();

            // limit niezaliczonych testów, po jakim testy jednostkowe zostaną przerwane
            if (test_session_get_fail_count() >= 1000)
                test_terminate_session();
        }


        test_title("RLDebug :: Analiza wycieku zasobów");
        // sprawdź wycieki pamięci
        int leaks_detected = rldebug_show_leaked_resources(1);
        test_set_session_leaks(leaks_detected);

        // poinformuj serwer Mrówka o wyniku testu - podsumowanie
        test_title("Podsumowanie");
        if (selected_test == -1)
            test_summary(86); // wszystkie testy muszą zakończyć się sukcesem
        else
            test_summary(1); // tylko jeden (selected_test) test musi zakończyć się  sukcesem
        return EXIT_SUCCESS;
    }
    

    if (run_mode == rm_main_test)
    {
        test_title("Testy funkcji main()");

        void (*pfcn[])(int, char**, char**) =
        { 
            NULL
        };

        for (volatile int idx = 0; pfcn[idx] != NULL && !test_get_session_termination_flag(); idx++)
        {
            if (selected_test == -1 || selected_test == idx + 1)
                pfcn[idx](vargc, vargv, venvp);

            // limit niezaliczonych testów, po jakim testy jednostkowe zostaną przerwane
            if (test_session_get_fail_count() >= 1000)
                test_terminate_session();
        }


        test_title("RLDebug :: Analiza wycieku zasobów");
        // sprawdź wycieki pamięci
        int leaks_detected = rldebug_show_leaked_resources(1);
        test_set_session_leaks(leaks_detected);

        // poinformuj serwer Mrówka o wyniku testu - podsumowanie
        test_title("Podsumowanie");
        if (selected_test == -1)
            test_summary(0); // wszystkie testy muszą zakończyć się sukcesem
        else
            test_summary(1); // tylko jeden (selected_test) test musi zakończyć się  sukcesem

        return EXIT_SUCCESS;
    }

    printf("*** Nieznana wartość RunMode: %d", (int)run_mode);
    abort();
}