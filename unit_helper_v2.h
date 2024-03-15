/*
 * Biblioteka pomocnicza dla testów jednostkowych systemu DANTE
 * Autorzy: Tomasz Jaworski, Piotr Duch, 2017-2020
 */

#if !defined(_UNIT_TEST_HELPER_H_)
#define _UNIT_TEST_HELPER_H_

//
// Funkcje testów jednostkowych
//

// Start wszystkich testów jednostkowych w ramach danego testu maszynowego (start sesji)
void unit_test_init(int mode, const char* unit_test_source);

// Start indywidualnego testu jednostkowego bądź ich niewielkiej grupy (start pojedynczy)
void test_start(int test_index, const char* title, int line);

// Nagłówek w raporcie
void test_title(const char* str);

// Podsumowanie testów
void test_summary(int expected_positives);


// Raportuj BŁĄD i PRZERWIJ dany test
// Jeżeli warunek __cond nie jest spełniony to przerwij dany test z komunikatem __message
#define test_error(__cond, ...) do { \
    if (!(__cond)) \
        test_result_internal(TEST_FAILED, __LINE__, __VA_ARGS__); \
    else \
        test_result_internal(TEST_NONE, __LINE__, ""); \
} while(0)

// Raportuj OSTRZEŻENIE i KONTYNUUJ dany test
// Jeżeli warunek __cond nie jest spełniony to przerwij dany test z komunikatem __message
#define test_warning(__cond, ...) do { \
    if (!(__cond)) \
        test_result_internal(TEST_WARNING, __LINE__, __VA_ARGS__); \
    else \
        test_result_internal(TEST_NONE, __LINE__, ""); \
} while(0)

// Raportuj SUKCES i ZAKOŃCZ dany test
#define test_ok(void) do { \
    if (!test_single_has_failed()) \
        test_result_internal(TEST_PASSED, __LINE__, ""); \
    return; \
} while(0)

// Jeśli w trakcie wykonywania danego testu pojawił się błąd, to zakończ ten test.
// Następne testy, jeżeli istnieją, to będą one uruchomione
#define onerror_return() do { \
    if (test_single_has_failed()) \
        return; \
    } while (0)

// Jeśli w trakcie wykonywania danego testu pojawił się błąd, to zakończ ten test
// oraz przerwij cały proces testowania.
//
#define onerror_terminate() do { \
    if (test_single_has_failed()) { \
        test_terminate_session(); \
        return; \
        } \
    } while (0)


// Jeśli w trakcie wykonywania danego testu pojawił się błąd, to porównaj dane o wielkości __size
// pod dwoma wskaźnikami. Wskaźnik __reference_pointer to dane referencyjne (takie, jakich oczekuje test)
// a __compared_pointer to wskaźnik z daynmi wygenerowanymi przez program Studenta.
#define onerror_compare_memory(__reference_pointer, __compared_pointer, __size) do { \
    if (test_single_has_failed()) \
        mem_compare(__reference_pointer, __compared_pointer, __size); \
    } while (0)


/*
#define TEST(__expr) do { \
    test_show_call(#__expr, __LINE__); \
    __expr; \
} while (0)
*/

//
// Narzędzia
//

// Zwróc pozycję pierwszej różnicy między danymi pods wskaźnkami ptr1 i ptr2. Funkcja sprawdza size bajtów.
int mem_find_first_difference(const void* ptr1, const void* ptr2, ssize_t size);

// Pobierz bajt z pozycji pos bufora danego wskaźnikiem ptr.
unsigned char mem_get_byte(const void* ptr, int pos);



// ***************************************************************
// Wewnętrzne struktury testów jednostkowych
//

enum TEST_RESULT {
    TEST_NONE = 0,
    TEST_PASSED = 1,
    TEST_WARNING = 2,
    TEST_FAILED = 3,
};

void test_result_internal(enum TEST_RESULT result, int line, const char* message, ...);
int test_single_has_failed(void);
int test_session_get_fail_count(void);
void test_terminate_session(void);
int test_get_session_termination_flag(void);
void test_set_session_leaks(int leaks);
void test_show_call(const char* call, int line);
void mem_compare(const void* reference, const void* tested, size_t size);

int __real_main(int argc, char** argv, char** envp);
#define tested_main __real_main

// --------

void test_limit_init(void);

void test_memory_limit_setup(int soft_limit, int hard_limit);
void test_memory_limit_restore(void);

void test_file_write_limit_setup(int write_limit);
void test_file_write_limit_restore(void);



#endif // _UNIT_TEST_HELPER_H_

