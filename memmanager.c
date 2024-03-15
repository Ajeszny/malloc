/*
 * Biblioteka pomocnicza dla testów jednostkowych systemu DANTE
 * Emulacja funkcji systemowej sbrk() na potrzeby projektu alokatora pamięci przedmiotu Systemy Operacyjne 2.
 * Autor: Tomasz Jaworski, 2020
 *
 * Wersja   Opis
 * 1.01     Dodanie dodatkowego płotka brk + zewnętrzna walidacja płotków
 * 1.00     Init
 */

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>

#if !defined(__clang__) && !defined(__GNUC__)
// Zakomentuj poniższy błąd, jeżeli chcesz przetestować testy na swoim kompilatorze C.
#error System testow jednostkowych jest przeznaczony dla kompilatorów GCC/Clang.
#endif

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
// Zakomentuj poniższy błąd, jeżeli chcesz przetestować testy na platformie Windows.
#error System testow jednostkowych NIE jest przeznaczony dla testów uruchamianych na platformach Windows.
#endif

#if defined(_HTML_OUTPUT)
#define BOLD(x) "<b>" x "</b>"
#endif

#if defined(_ANSI_OUTPUT)
#define BOLD(x) "\x1b[1m" x "\x1b[0m"
#endif

#if !defined(_HTML_OUTPUT) && !defined(_ANSI_OUTPUT)
#define BOLD(x) x
#endif

// --------

#include "custom_unistd.h"

#define PAGE_SIZE       4096    // Długość strony w bajtach
#define PAGE_FENCE      1       // Liczba stron na jeden płotek
#define PAGES_AVAILABLE 16384   // Liczba stron dostępnych dla sterty
#define PAGES_TOTAL     (PAGES_AVAILABLE + 2 * PAGE_FENCE)

// Makro zaokrągla adres bajta __addr do adresu bazowego następnej strony
#define ROUND_TO_NEXT_PAGE(__addr) (((__addr) & ~(PAGE_SIZE - 1)) + PAGE_SIZE * !!((__addr) & (PAGE_SIZE - 1)))


uint8_t memory[PAGE_SIZE * PAGES_TOTAL] __attribute__((aligned(PAGE_SIZE)));

struct memory_fence_t {
    uint8_t first_page[PAGE_SIZE];
    uint8_t last_page[PAGE_SIZE];
};

struct mm_struct {
    intptr_t start_brk;
    intptr_t brk;

    pthread_mutex_t mutex;

    // Poniższe pola nie należą do standardowej struktury mm_struct
    struct memory_fence_t fence;
    intptr_t start_mmap;

    // statystyki
    struct timespec init_timestamp;
    uint64_t sbrk_executions;
} mm;


void __attribute__((constructor)) memory_init(void) {
    //
    // Inicjuj testy
    setvbuf(stdout, NULL, _IONBF, 0);
    srand(time(NULL));
    assert(sizeof(intptr_t) == sizeof(void *));

    /*
     * Architektura przestrzeni dynamicznej dla sterty, z płotkami pamięci:
     *
     *  |<-   PAGES_AVAILABLE            ->|
     * ......................................
     * FppppppppppppppppppppppppppppppppppppL
     *
     * F - płotek początku
     * L - płotek końca
     * p - strona do użycia (liczba stron nie jest znana)
     *
     * W większości przypadków, gdy alokator nie będzie korzystał ze wszystkich stron sterty,
     * układ poszczególnych elementów będzie następujący:
     *
     *  |<-   PAGES_AVAILABLE            ->|
     * ......................................
     * FpppppppppppppppLxxxxxxxxxxxxxxxxxxxxL
     * ||              |+-- obszar wolny --+|
     * ||              |                    |
     * ||              +brk                 +start_mmap
     * |+start_brk
     * +memory
     *
     * Płotek L w pozycji brk jest przesuwany automatycznie, przy każdym uruchomieniu custom_sbr().
     */

    //
    // Inicjuj płotki
    for (int i = 0; i < PAGE_SIZE; i++) {
        mm.fence.first_page[i] = rand();
        mm.fence.last_page[i] = rand();
    }

    //
    // Inicjuj strukturę opisującą pamięć procesu (symulację tej struktury)
    mm.start_brk = (intptr_t) (memory + PAGE_SIZE);
    mm.brk = (intptr_t) (memory + PAGE_SIZE);
    mm.start_mmap = (intptr_t) (memory + (PAGE_FENCE + PAGES_AVAILABLE) * PAGE_SIZE);

    assert(mm.start_mmap - mm.start_brk == PAGES_AVAILABLE * PAGE_SIZE);

    //
    // Ustaw płotki
    memcpy(memory, mm.fence.first_page, PAGE_SIZE); // płotek przed pierwszą stroną CAŁEJ przestrzeni
    memcpy((void *) mm.start_mmap, mm.fence.last_page, PAGE_SIZE); // płotek za ostatnią stroną CAŁEJ przestrzeni
    memcpy((void *) mm.brk, mm.fence.last_page, PAGE_SIZE); // płotek za ostatnią stroną PRZYDZIELONEGO obszaru sterty


    //
    // Przygotuj sekcję krytyczną dla funkcji sbrk()
    pthread_mutexattr_t attributes;
    pthread_mutexattr_init(&attributes);
    pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE);
    int result = pthread_mutex_init(&mm.mutex, &attributes);
    if (result != 0) {
        perror("memory_init: pthread_mutex_init");
        exit(-1);
    }

    //
    // Zapamiętaj czas uruchomienia testów
    clock_gettime(CLOCK_REALTIME, &mm.init_timestamp);
    mm.sbrk_executions = 0;
}

static void memory_validate_fences(int *ok_first, int *ok_brk, int *ok_last) {

    if (ok_first != NULL) // Sprawdź płotek PRZED stertą alokatora
        *ok_first = memcmp(memory, mm.fence.first_page, PAGE_SIZE) == 0;

    if (ok_brk != NULL) // Sprawdź płotek ruchomy (w pozycji brk)
        *ok_brk = memcmp((const void *) ROUND_TO_NEXT_PAGE(mm.brk), mm.fence.last_page, PAGE_SIZE) == 0;

    if (ok_last != NULL) // Sprawdź płotek PO stercie alokatora
        *ok_last = memcmp((const void *) mm.start_mmap, mm.fence.last_page, PAGE_SIZE) == 0;
}

void __attribute__((destructor)) memory_check(void) {
    pthread_mutex_lock(&mm.mutex);

    //
    // Sprawdź płotki
    int ok_first, ok_brk, ok_last;
    memory_validate_fences(&ok_first, &ok_brk, &ok_last);

    //
    // Wyznacz czas testów
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    uint64_t delta_ms = now.tv_sec * 1000 + now.tv_nsec / 1e6;
    delta_ms -= mm.init_timestamp.tv_sec * 1000 + mm.init_timestamp.tv_nsec / 1e6;


    printf("\n### Stan płotków przestrzeni sterty:\n");
    printf("    Płotek początku ....: [%s]\n", ok_first ? "poprawny" : "<span style=\"color:red;\">USZKODZONY</span>");
    printf("    Płotek w pozycji brk: [%s]\n", ok_brk ? "poprawny" : "<span style=\"color:red;\">USZKODZONY</span>");
    printf("    Płotek końca........: [%s]\n", ok_last ? "poprawny" : "<span style=\"color:red;\">USZKODZONY</span>");

    printf("### Podsumowanie: \n");
    printf("    Całkowita przestrzeni dostępnej pamięci: %lu bajtów\n", mm.start_mmap - mm.start_brk);
    printf("    Pamięć zarezerwowana przez sbrk() .....: %lu bajtów\n", custom_sbrk_get_reserved_memory());
    printf("    Czas wykonywania wszystkich testów ....: %.3lf sekund\n", delta_ms / 1.0e3);
    printf("    Liczba wywołań " BOLD("custom_sbrk()") " ..........: %lu\n", mm.sbrk_executions);

    pthread_mutex_unlock(&mm.mutex);
    pthread_mutex_destroy(&mm.mutex);
}

//
//
//

int custom_sbrk_check_fences_integrity(void) {
    pthread_mutex_lock(&mm.mutex);

    int ok_first, ok_brk, ok_last;
    memory_validate_fences(&ok_first, &ok_brk, &ok_last);
    int status =
            !ok_first << 0 |
            !ok_brk << 1 |
            !ok_last << 2;

    /* Wartości statusu:
     * 0x00 - wszystko jest w porządku
     * 0x01 - tylko płotek początku jest uszkodzony; pozostałe płotki są całe
     * 0x05 - płotek początku i końca jest uszkodzony; płotek brk jest cały
     * 0x07 - wszystkie płotki padły.
     * itd...
    */
    pthread_mutex_unlock(&mm.mutex);
    return status; //
}

uint64_t custom_sbrk_get_reserved_memory(void) {

    pthread_mutex_lock(&mm.mutex);
    uint64_t return_value = mm.brk - mm.start_brk;
    pthread_mutex_unlock(&mm.mutex);

    return return_value;
}

void *custom_sbrk(intptr_t delta) {
    int ok_first, ok_brk, ok_last;
    memory_validate_fences(&ok_first, &ok_brk, &ok_last);
    if (!ok_first || !ok_brk || !ok_last) {
        printf("-----------------------------------------------\n");
        printf("<strong style=\"color:red;\">custom_sbrk:</strong> Wykryto uszkodzenie płotków sterty");
        exit(-1);
    }

    pthread_mutex_lock(&mm.mutex);
    void *return_value;

    intptr_t current_brk = mm.brk;
    if (mm.brk + delta < mm.start_brk) {
        errno = 0;
        return_value = (void *) current_brk;
        goto _exit; // :P
    }

    if (mm.brk + delta >= mm.start_mmap) {
        errno = ENOMEM;
        return_value = (void *) -1;
        goto _exit;
    }

    // Przesuń
    mm.brk += delta;
    return_value = (void *) current_brk;

    // płotek za ostatnią stroną PRZYDZIELONEGO obszaru sterty
    memcpy((void *) ROUND_TO_NEXT_PAGE(mm.brk), mm.fence.last_page, PAGE_SIZE);

    //
    //

    _exit:;
    mm.sbrk_executions++;
    pthread_mutex_unlock(&mm.mutex);
    return return_value;
}

