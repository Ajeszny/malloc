/*
 * Biblioteka pomocnicza dla testów jednostkowych systemu DANTE
 * Autorzy: Tomasz Jaworski, Piotr Duch, 2017-2020
 *
 * Wersja   Opis
 *
 * 1.01     Dodanie ograniczników pamięci
 * 1.00     Init
 */

#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <ctype.h>

#include <sys/resource.h>
#include <sys/errno.h>
#include <signal.h>



#include "unit_helper_v2.h"

#if !defined(_GNU_SOURCE)
    #error Biblioteka wymaga _GNU_SOURCE
#endif

#if defined(_HTML_OUTPUT)
    #define BOLD(x) "<b>" x "</b>"
    #define UPASSED(x) "<span class=\"upassed\">" x "</span>"
    #define UFAILED(x) "<span class=\"ufailed\">" x "</span>"
    #define UWARNING(x) "<span class=\"uwarning\">" x "</span>"
    #define ULEAK(x) "<span class=\"uleak\">" x "</span>"
#endif

#if defined(_ANSI_OUTPUT)
    #define BOLD(x) "\x1b[1m" x "\x1b[0m"
    #define UPASSED(x) "\x1b[32m" x "\x1b[0m"
    #define UFAILED(x) "\x1b[31m" x "\x1b[0m"
    #define UWARNING(x) "\x1b[33m" x "\x1b[0m"
    #define ULEAK(x) "\x1b[35m" x "\x1b[0m"
#endif

#if !defined(_HTML_OUTPUT) && !defined(_ANSI_OUTPUT)
    #define BOLD(x) x
    #define UPASSED(x) x
    #define UFAILED(x) x
    #define UWARNING(x) x
    #define ULEAK(x) x
#endif



struct test_statistics_t {
    int group_id;
    int current_index;
    enum TEST_RESULT last_result;
    int error_detected;
    int terminate;
    const char* unit_test_source;

    struct {
        int passed;
        int failed;
        int warnings;
        int leaks;
    } session, single;
};

static struct test_statistics_t test_statistics;

void unit_test_init(int gid, const char* unit_test_source)
{
    test_statistics.group_id = gid;
    test_statistics.unit_test_source = unit_test_source;
    test_statistics.current_index = 0;
    test_statistics.terminate = 0;

    test_statistics.last_result = TEST_NONE;

    memset(&test_statistics.session, 0, sizeof(test_statistics.session));
    memset(&test_statistics.single, 0, sizeof(test_statistics.single));
}

void test_title(const char* str)
{
#if defined(_HTML_OUTPUT)
    printf("<hr/>");
    printf("<h3>### %s ### </h3>", str);
#else
    printf("--------------------------------------------------------\n");
    printf("### " BOLD( "\x1b[30m" "%s") " ###\n\n", str);
#endif
}


void test_start(int test_index, const char* title, int line)
{
    test_statistics.current_index = test_index;
    test_statistics.last_result = TEST_NONE;

    test_statistics.single.passed = 0;
    test_statistics.single.failed = 0;
    test_statistics.single.warnings = 0;

    if (test_statistics.current_index > 1)
        printf("\n");

#if defined(_HTML_OUTPUT)
    const char* header = "<b><a href=\"source\\%s.html#line-%d\" class=\"utlink\">TEST %d</a></b>: %s\n";
    printf(header, test_statistics.unit_test_source, line, test_statistics.current_index, title);
#else
    const char* header = "TEST %d: %s\n";
    printf(header, test_statistics.current_index, title);
    ((void)line);
#endif

}

void test_summary(int expected_positives)
{
    printf("   Testy dostępne: " BOLD    ("%4d") " (" BOLD    ("AVAIL"   ) ")\n", expected_positives);
    printf("         Wykonane: " BOLD    ("%4d") " (" BOLD    ("DONE"    ) ")\n", test_statistics.session.passed + test_statistics.session.failed);
    printf("   Testy poprawne: " UPASSED ("%4d") " (" UPASSED ("PASSED"  ) ")\n", test_statistics.session.passed);
    printf("Testy niepoprawne: " UFAILED ("%4d") " (" UFAILED ("FAILED"  ) ")\n", test_statistics.session.failed);
    printf("      Ostrzeżenia: " UWARNING("%4d") " (" UWARNING("WARNINGS") ")\n", test_statistics.session.warnings);
    printf("  Wycieki zasobów: " ULEAK   ("%4d") " (" ULEAK   ("LEAKS"   ) ")\n", test_statistics.session.leaks);

#if defined(_HTML_OUTPUT)
    printf("<div style=\"display: none;\">status avail=%d done=%d failed=%d passed=%d warns=%d gid=%d leaks=%d</div>",
        expected_positives,
        test_statistics.session.passed + test_statistics.session.failed, test_statistics.session.failed ,
        test_statistics.session.passed, test_statistics.session.warnings, test_statistics.group_id,
        test_statistics.session.leaks);
#endif
}



void test_result_internal(enum TEST_RESULT result, int line, const char* message, ...)
{
    assert(result == TEST_FAILED || result == TEST_PASSED || result == TEST_WARNING || result == TEST_NONE);
    test_statistics.last_result = result;

    if (result == TEST_NONE) {
        return;
    } else if (result == TEST_FAILED) {

        printf("Wynik: " UFAILED("PORAŻKA") ": ");

        va_list ap;
        va_start(ap, message);
        vprintf(message, ap);
        va_end(ap);

        printf("\n");
#if defined(_HTML_OUTPUT)
        printf("       Sprawdź funkcję testującą <b>TEST%d(void)</b> z pliku <a href=\"source\\%s.html#line-%d\">%s</a>, w linii %d\n",
                test_statistics.current_index, test_statistics.unit_test_source, line, test_statistics.unit_test_source, line);
#else
        printf("       Sprawdź funkcję testującą TEST%d(void) z pliku %s w linii %d\n",
               test_statistics.current_index, test_statistics.unit_test_source, line);
#endif

        test_statistics.single.failed++;
        test_statistics.session.failed++;
        return;

    } else if (result == TEST_PASSED) {

        printf("Wynik: " UPASSED("SUKCES") "\n");
        test_statistics.single.passed++;
        test_statistics.session.passed++;
        return;

    } else if (result == TEST_WARNING) {
        printf("Wynik: <span class=\"uwarning\">OSTRZEŻENIE</span>: ");

        va_list ap;
        va_start(ap, message);
        vprintf(message, ap);
        va_end(ap);

        printf("\n");
#if defined(_HTML_OUTPUT)
        printf("       Sprawdź funkcję testującą <b>TEST%d(void)</b> z pliku <a href=\"source\\%s.html#line-%d\">%s</a>, w linii %d\n",
                test_statistics.current_index, test_statistics.unit_test_source, line, test_statistics.unit_test_source, line);
#else
        printf("       Sprawdź funkcję testującą TEST%d(void) z pliku %s w linii %d\n",
                test_statistics.current_index, test_statistics.unit_test_source, line);
#endif
        test_statistics.single.warnings++;
        test_statistics.session.warnings++;
        return;
    }

    assert(0 && "???");
}

void test_terminate_session(void)
{
    printf(UFAILED("*** Test przerwany ***") "\n");
    test_statistics.terminate = 1;
}

//void test_show_call(const char* call, int line)
//{
//    printf("Wywołanie: <b>%s</b> z pliku <a href=\"source\\%s.html#line-%d\">%s</a>, w linii %d\n",
//        call, test_statistics.unit_test_source, line, test_statistics.unit_test_source, line);
//}


int test_get_session_termination_flag(void)
{
    return test_statistics.terminate;
}

void test_set_session_leaks(int leaks)
{
    test_statistics.session.leaks = leaks;
}

int test_single_has_failed(void)
{
    return test_statistics.single.failed > 0;
}

int test_session_get_fail_count(void)
{
    return test_statistics.session.failed;
}

//
//
//
//


int mem_find_first_difference(const void* ptr1, const void* ptr2, ssize_t size)
{
    if (memcmp(ptr1, ptr2, size) == 0)
        return -1; // nie ma różnicy

    const uint8_t* bptr1 = (const uint8_t*)ptr1;
    const uint8_t* bptr2 = (const uint8_t*)ptr2;

    for (int i = 0; i < size; i++)
        if (*bptr1++ != *bptr2++)
            return i;

    assert(0 && "???");
}

uint8_t mem_get_byte(const void* ptr, int pos)
{
    const uint8_t* bptr = (const uint8_t*)ptr;
    return bptr[pos];
}

void mem_dump_diff(const void* p1, const void* p2, size_t size) {

    const uint8_t* b1 = (const uint8_t*)p1;
    const uint8_t* b2 = (const uint8_t*)p2;
    const uint32_t ROW_LENGTH = 16;

    int diff = 0;
    for (uint32_t addr = 0; addr < size; addr += ROW_LENGTH) {
        printf("       %04x  ", addr);
        for (uint32_t pos = 0; pos < ROW_LENGTH; pos++)
            if (addr + pos < size) {
                if (b1[addr + pos] != b2[addr + pos] && !diff) {
                    diff = 1;
#if defined(_HTML_OUTPUT)
                    printf("<span style=\"color: red; font-weight:bold;\">");
#endif
                }

                if (b1[addr + pos] == b2[addr + pos] && diff) {
                    diff = 0;
#if defined(_HTML_OUTPUT)
                    printf("</span>");
#endif
                }

                printf("%02x ", b1[addr + pos]);
            } else
                printf("   ");

        if (diff) {
            diff = 0;
            printf("</span>");
        }

        printf(" ");

        for (uint32_t pos = 0; addr + pos < size && pos < ROW_LENGTH; pos++)
            putchar(isprint(b1[addr + pos]) ? b1[addr + pos] : '.');

        putchar('\n');
    }
}


void mem_compare(const void* reference, const void* tested, size_t size)
{
    printf("       Zrzut pamięci (dane oczekiwane):\n");
    mem_dump_diff(reference, tested, size);

    printf("       Zrzut pamięci (dane otrzymane):\n");
    mem_dump_diff(tested, reference, size);
}

/*
 ################################################################################################
 #
 # Narzędzia
 #
 ################################################################################################
 */

struct test_limit_t {
    struct rlimit memory_current, memory_old;
    struct rlimit file_write_current, file_write_old;

    int memory_set;
    int file_write_set;

    sighandler_t old_handler;
};

static struct test_limit_t test_limit;


void test_internal_error(int cond, const char* message)
{
    if (cond)
        return; // jest ok

    printf("\n<span style=\"font-weight:bold; color:red\">***\n"
           "   Bład %s\n"
           "   errno=%d; strerror(%d)=%s\n"
           "   Skontaktuj się z autorem testu!\n"
           "***</span>\n", message, errno, errno, strerror(errno));

    test_error(0, "Błąd wewnętrzny testu - skontaktuj się z autorem testu!");
    exit(42);
}


void test_limit_init(void)
{
    memset(&test_limit, 0, sizeof(struct test_limit_t));
}

void test_memory_limit_setup(int soft_limit, int hard_limit)
{
    struct rlimit current;

    //test_internal_error(!test_limit.memory_set, "Limit pamięci jest już aktywny!");


    if (getrlimit (RLIMIT_DATA, &current) != 0)
        test_internal_error(0, "getrlimit");

    memcpy(&test_limit.memory_current, &current, sizeof(struct rlimit));
    memcpy(&test_limit.memory_old, &current, sizeof(struct rlimit));

    test_limit.memory_current.rlim_cur = soft_limit;
    test_limit.memory_current.rlim_max = hard_limit;

    if (setrlimit(RLIMIT_DATA, &test_limit.memory_current) != 0)
        test_internal_error(0, "setrlimit(RLIMIT_DATA, &test_limit.memory_current)");

    test_limit.memory_set = 1;


    /*
    printf("# Limit pamięci: current[soft=%d; hard=%d]; old[soft=%d; hard=%d]\n",
        (int)test_limit.memory_current.rlim_cur, (int)test_limit.memory_current.rlim_max,
        (int)test_limit.memory_old.rlim_cur, (int)test_limit.memory_old.rlim_max);
    */
}

void test_memory_limit_restore(void)
{
/*
    test_internal_error(test_limit.memory_set, "Limit pamięci nie jest aktywny!");
    if (setrlimit(RLIMIT_DATA, &test_memory_limit.old) != 0)
        test_internal_error(0, "setrlimit(RLIMIT_DATA, &test_memory_limit.old)");

    test_limit.memory_set = 0;
*/
}


// ------------------------------

void test_signal_handler(int signo)
{
	switch (signo)
	{
		case SIGXFSZ:
			fprintf(stderr, "Przekroczono wielkość pliku podczas zapisywania (%d, %s)\n", signo, strsignal(signo));
			exit(128 + SIGXFSZ);
			break;
		default:
			assert(0);
	}
}


void test_file_write_limit_setup(int write_limit)
{

    if (getrlimit (RLIMIT_FSIZE, &test_limit.file_write_current) != 0)
        test_internal_error(0, "getrlimit (test_file_write_limit_setup)");

    test_limit.file_write_current.rlim_cur = write_limit;
    //test_limit.file_write_current.rlim_max = write_limit;

    if (setrlimit(RLIMIT_FSIZE, &test_limit.file_write_current) != 0)
        test_internal_error(0, "setrlimit(RLIMIT_FSIZE, &test_limit.file_write_current)");

    //test_internal_error(!test_limit.file_write_set, "Limit wielkości zapisywanego pliku jest już aktywny!");
    test_limit.file_write_set = 1;

    /*
    printf("# Limit pliku: current[soft=%d; hard=%d]; old[soft=%d; hard=%d]\n",
        (int)test_limit.file_write_current.rlim_cur, (int)test_limit.file_write_current.rlim_max,
        (int)test_limit.file_write_old.rlim_cur, (int)test_limit.file_write_old.rlim_max);
    */

    // ustawienie sygnałów
    sighandler_t old = signal(SIGXFSZ, test_signal_handler);
    test_internal_error(old != SIG_ERR, "signal(SIGXFSZ, test_signal_handler) ");
    test_limit.old_handler = old;
}

void test_file_write_limit_restore(void)
{

//    if (setrlimit(RLIMIT_FSIZE, &test_limit.file_write_old) != 0)
//        test_internal_error(0, "setrlimit(RLIMIT_FSIZE, &test_limit.file_write_old)");

    // przywracanie sygnałów
    sighandler_t old = signal(SIGXFSZ, test_limit.old_handler);
    test_internal_error(old != SIG_ERR, "signal(SIGXFSZ, test_limit.old_handler) ");
    test_limit.old_handler = NULL;

    //test_internal_error(test_limit.file_write_set, "Limit wielkości zapisywanego pliku nie jest aktywny!");
    test_limit.file_write_set = 0;
}
