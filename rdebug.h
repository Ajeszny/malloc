/*
 *
 *  RLDebugger - Resource Leakage Debugger
 *  Autor: Tomasz Jaworski, 2018-2020
 *
 * ==================================================
 *   Plik nagłówkowy rdebug.h _MUSI_ być dodany na końcu listy #include w kompilowanym pliku .C
 *   Przykład:
 * 		#include <stdio.h>
 * 		#include <string.h>
 * 		#include "rdebug.h"
 * 
 * 		...
 * 		int main(....)
 */

#if !defined(_RESOURCE_DEBUG_H_)
#define _RESOURCE_DEBUG_H_

enum heap_function_code_t {
	__hfc_min = 0,
	
	HFC_MALLOC = 1,
	HFC_FREE,
	HFC_CALLOC,
	HFC_REALLOC,

	HFC_STRDUP,
	HFC_STRNDUP,
	
	HFC_EXIT,

	HFC_FOPEN,	// wip
	HFC_FCLOSE,	// wip
	
	__hfc_max
};

#if !defined(_RLDEBUG_IMPLEMENTATION_)


	#if defined(__malloc_and_calloc_defined) || defined(_MALLOC_H) || defined(_STDLIB_H)
		void* _rldebug_heap_wrapper(
			enum heap_function_code_t call_type,	// Typ wywołania - identyfikator funkcji w imieniu której wywoływany jest wrapper
			void* ptr,								// ARGUMENT 0: Wskaźnik przekazany do fukncji
			size_t number,							// ARGUMENT 1: pierwsza liczba przekazana do funkcji (zaraz za wskaźnikiem)
			size_t size,							// ARGUMENT 2: druga liczba przekazana do funkcji (tylko calloc)
			const char* source_name,				// Nazwa pliku, w którym pojawiło się dane wywołanie
			int source_line							// Linia w pliku, z której pojawiło się wywołanie
		);		

		// przejęcie funkcji obsługi sterty dla biblioteki standardowej C
		#define malloc(n)             _rldebug_heap_wrapper(HFC_MALLOC,  NULL, n,    0,    __FILE__, __LINE__)
		#define free(p)	              _rldebug_heap_wrapper(HFC_FREE,    p,    0,    0,    __FILE__, __LINE__)
		#define calloc(n, s)          _rldebug_heap_wrapper(HFC_CALLOC,  NULL, n,    s,    __FILE__, __LINE__)
		#define realloc(p, n)         _rldebug_heap_wrapper(HFC_REALLOC, p,    n,    0,    __FILE__, __LINE__)

	#endif // __malloc_and_calloc_defined

	#if defined(_STRING_H)
		char* _rldebug_str_wrapper(
			enum heap_function_code_t call_type,	// Typ wywołania - identyfikator funkcji w imieniu której wywoływany jest wrapper
			const char* pstring,					// ARGUMENT 0: Wskaźnik przekazany do fukncji
			size_t number,							// ARGUMENT 1: pierwsza liczba przekazana do funkcji (zaraz za wskaźnikiem)
			const char* source_name,				// Nazwa pliku, w którym pojawiło się dane wywołanie
			int source_line							// Linia w pliku, z której pojawiło się wywołanie
		);

		// kolizja z GNU Features
		#if defined __USE_XOPEN2K8

			#undef strdup
			#undef strndup
	
		#endif

		#define strdup(p)             _rldebug_str_wrapper(HFC_STRDUP,  p,    0,    __FILE__, __LINE__)
		#define strndup(p, n)         _rldebug_str_wrapper(HFC_STRNDUP, p,    n,    __FILE__, __LINE__)
	#endif // _STRING_H

	#if defined(_STDIO_H)
	
		void* _rldebug_io_wrapper(enum heap_function_code_t call_type, FILE* stream, const char* stream_name,
								  const char* stream_mode, const char* source_file, int source_line);
	
	
		#define fopen(f, m)   ((FILE*)_rldebug_io_wrapper (HFC_FOPEN,   NULL, f,    m,    __FILE__, __LINE__))

		#if __WORDSIZE == 64
		    // x64
			#define fclose(stream)     ((unsigned long int) \
									  _rldebug_io_wrapper (HFC_FCLOSE,  stream,    NULL, NULL, __FILE__, __LINE__))
		#else // if __WORDSIZE == 64
		    // x32
			#define fclose(stream)  ((int)_rldebug_io_wrapper (HFC_FCLOSE,  stream,    NULL, NULL, __FILE__, __LINE__))
		#endif // if __WORDSIZE == 64
	#endif // _STDIO_H
	
	#if defined(_STDLIB_H)
		
		void* __attribute__((noreturn)) _rldebug_stdlib_noreturn_wrapper(enum heap_function_code_t call_type, int iarg, const char* source_file, int source_line);
		
		#define exit(__status)		 _rldebug_stdlib_noreturn_wrapper(HFC_EXIT, __status, __FILE__, __LINE__)
		
	#endif // _STDLIB_H

//	#if defined(__malloc_and_calloc_defined) || defined(_STRING_H)
//	#endif // defined(__malloc_and_calloc_defined) || defined(_STRING_H)

#endif // _RLDEBUG_IMPLEMENTATION_

#define SIGHEAP SIGUSR2 // redefinicja sygnału USR2 na potrzeby debuggera zasobów

int rdebug_call_main(int (*pmain)(int, char**, char**), int argc, char** argv, char** envp);


//
//  #############################################################################
//  ## 
//	## Interfejs publiczny
//  ## 
//  #############################################################################

#if defined _RLDEBUG_API_

#define RLD_HEAP_UNLIMITED		(SIZE_MAX >> 1)
#define RLD_UNKNOWN_POINTER     (SIZE_MAX >> 1)

enum message_severity_level_t {
	MSL_QUIET = 0,
	MSL_INFORMATION,
	MSL_WARNING,
	MSL_FAILURE
};


int rldebug_show_leaked_resources(int force_empty_summary);

//
//	## ustwienia, ograniczniki, płotki, etc..
// 

// Uchyl wszystkie ograniczniki sterty i zasobów
void rldebug_reset_limits(void);

// Ustaw poziom szczegółowości raportowania akcji testowanego programu
void rldebug_set_reported_severity_level(enum message_severity_level_t lowest_level);

// Ustaw globalny limit sterty
void rldebug_heap_set_global_limit(size_t heap_limit /* = RLD_HEAP_UNLIMITED */);

// Ustal jednostkowy limit alokacji pamięci (nie więcej niz `limit` bajtów na jedno wywołanie funkcji `call_type`
void rldebug_heap_set_function_singleshot_limit(enum heap_function_code_t call_type, size_t limit);

// Włacz/wyłacz wszystkie funkcje obsługi sterty
void rldebug_heap_disable_all_functions(int disabled);

// Wyznacz wielkość wycieku
size_t rldebug_heap_get_leak_size(void);

// Funkcja rldebug_get_block_size(ptr) zwraca wielkość bloku reprezentowanego przez wskaźnik ptr albo RLD_UNKNOWN_POINTER jeśli wskaźnik nie reprezentuje bloku
// Reprezentacja bloku przez wskaźnik - wskaźnik wskazuje na pierwszy bajt bloku a nie w dowolne jego miejsce
size_t rldebug_get_block_size(const void* ptr);

// Funkcja ustawia skumulowany limit alokacji pamięci dla funkcji call_type.
void rldebug_heap_set_function_cumulative_limit(enum heap_function_code_t call_type, size_t limit);


// Funkcja ustawia liczbę wywołań danej funkcji call_type, które zakończą się sukcesem
void rldebug_set_function_success_limit(enum heap_function_code_t call_type, size_t limit);

//
// Podłączenie do testów jednostkowych
//

// Raportuj BŁĄD i PRZERWIJ dany test jeśli wykryto wycieki pamięci
#define test_no_heap_leakage() do { \
    if (rldebug_heap_get_leak_size() > 0) \
        test_result_internal(TEST_FAILED, __LINE__, "Wykryto wyciek pamięci."); \
    else \
        test_result_internal(TEST_NONE, __LINE__, ""); \
} while(0)

#endif // _RLDEBUG_API_
#endif // _RESOURCE_DEBUG_H_