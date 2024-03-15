/*
 *
 *  RLDebugger - Resource Leakage Debugger
 *  Autor: Tomasz Jaworski, 2018-2020
 *
 */

#define _RLDEBUG_IMPLEMENTATION_
#define _RLDEBUG_API_

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include <errno.h>
#include <time.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>


#include "rdebug.h"


#define RLD_MAGIC1			0xc8fccdb505c6ac13	
#define RLD_MAGIC2			0xece706d3d0533953

#define ALIGN(n, __type)	(n & ~(sizeof(__type)-1)) + sizeof(__type)*!!(n & (sizeof(__type)-1))
#define MIN(__x, __y) ((__x) < (__y) ? (__x) : (__y))
#define MAX(__x, __y) ((__x) > (__y) ? (__x) : (__y))

#define IS_ALLOCATION_FUNCTION_CODE(__code) ((__code) == HFC_CALLOC || (__code) == HFC_MALLOC || (__code) == HFC_REALLOC || (__code) == HFC_STRDUP || (__code) == HFC_STRNDUP)

#if defined(_HTML_OUTPUT)
    #define BOLD(str) "<b>" str "</b>"
    #define BYELLOW(str) "<span style=\"background-color:yellow\">" str "</span>"
    #define BOLDGREEN(str) "<span style=\"color:green; font-weight:bold\">" str "</span>"
    #define BOLDRED(str) "<span style=\"color:red;font-weight:bold\">" str "</span>"
    #define BOLDPINK(str) "<span style=\"color:magenta;font-weight:bold\">" str "</span>"
#endif

#if defined(_ANSI_OUTPUT)
    #define BOLD(str) "\x1b[1m" str "\x1b[0m"
    #define YELLOW(str) "\x1b[33m" str "\x1b[0m"
    #define BOLDGREEN(str) "\x1b[32m" BOLD(str) "\x1b[0m"
    #define BOLDRED(str) "\x1b[31m" BOLD(str) "\x1b[0m"
    #define BOLDPINK(str) "\x1b[35m" BOLD(str) "\x1b[0m"
#endif

#if !defined(_HTML_OUTPUT) && !defined(_ANSI_OUTPUT)
    #define BOLD(str) str
    #define YELLOW(str) str
    #define BOLDGREEN(str) str
    #define BOLDRED(str) str
    #define BOLDPINK(str) str
#endif

enum resource_type_t {
	RT_MEMORY,
	RT_STREAM
};

struct block_fence_t {
	uint8_t pattern[32];
};

struct rld_settings_t {

	//rldebug_callback_t callback;
	enum message_severity_level_t lowest_reported_severity;
	
	// ogranicznik wielkości sterty
	struct {
		int global_limit_active;
		size_t global_limit_value;
		int global_disable;
	} heap;
};


enum rld_message_t {
	RLD_UNDEFINED = 0,
	RLD_HEAP_FUNCTIONS_DISABLED,
	
	RLD_MALLOC_NO_MEMORY,
	RLD_MALLOC_NO_MEMORY_DUE_LIMIT,
	RLD_MALLOC_NO_MEMORY_DUE_SINGLESHOT_LIMIT,
	RLD_MALLOC_NO_MEMORY_DUE_CUMULATIVE_LIMIT,
	RLD_MALLOC_FAILED_DUE_SUCCESS_LIMIT,
	RLD_MALLOC_SUCCESSFUL,
	
	RLD_FREE_NULL,
	RLD_FREE_INVALID_POINTER,
	RLD_FREE_SUCCESSFUL,

	RLD_CALLOC_NO_MEMORY,
	RLD_CALLOC_NO_MEMORY_DUE_LIMIT,
	RLD_CALLOC_NO_MEMORY_DUE_SINGLESHOT_LIMIT,
	RLD_CALLOC_NO_MEMORY_DUE_CUMULATIVE_LIMIT,
	RLD_CALLOC_FAILED_DUE_SUCCESS_LIMIT,
	RLD_CALLOC_SUCCESSFUL,
	
	RLD_REALLOC_NO_MEMORY,
	RLD_REALLOC_NO_MEMORY_DUE_LIMIT,
	RLD_REALLOC_NO_MEMORY_DUE_SINGLESHOT_LIMIT,
	RLD_REALLOC_NO_MEMORY_DUE_CUMULATIVE_LIMIT,
	RLD_REALLOC_FAILED_DUE_SUCCESS_LIMIT,
	RLD_REALLOC_INVALID_POINTER,
	RLD_REALLOC_SUCCESSFUL,

	//
	
	RLD_STRDUP_NULL,
	RLD_STRDUP_NO_MEMORY,
	RLD_STRDUP_NO_MEMORY_DUE_LIMIT,
	RLD_STRDUP_NO_MEMORY_DUE_SINGLESHOT_LIMIT,
	RLD_STRDUP_NO_MEMORY_DUE_CUMULATIVE_LIMIT,
	RLD_STRDUP_FAILED_DUE_SUCCESS_LIMIT,
	RLD_STRDUP_SUCCESSFUL,
	
	RLD_STRNDUP_NULL,
	RLD_STRNDUP_NO_MEMORY,
	RLD_STRNDUP_NO_MEMORY_DUE_LIMIT,
	RLD_STRNDUP_NO_MEMORY_DUE_SINGLESHOT_LIMIT,
	RLD_STRNDUP_NO_MEMORY_DUE_CUMULATIVE_LIMIT,
	RLD_STRNDUP_FAILED_DUE_SUCCESS_LIMIT,
	RLD_STRNDUP_SUCCESSFUL,

	//

	RLD_HEAP_BROKEN,
	RLD_HEAP_DATA_OUT_OF_BOUNDS,

	//
	
	RLD_FOPEN_SUCCESSFUL,
	RLD_FOPEN_FAILED,
	RLD_FOPEN_FAILED_DUE_SUCCESS_LIMIT,

	RLD_FCLOSE_NULL_STREAM,
	RLD_FCLOSE_INVALID_STREAM,
	RLD_FCLOSE_SUCCESSFUL,
	
};


struct resource_t {
	uint64_t magic1;
	
	enum resource_type_t type; // typ zasobu
	union {
		
		// blok pamieci
		struct {
			size_t size;
			void* base_pointer;
			enum heap_function_code_t allocated_by;
			
			struct block_fence_t head_fence;
			struct block_fence_t tail_fence;
		} memory;

		// strumien
		struct {
			char* name;
			char* mode;
			FILE* stream;
		} stream;
	};

	// wspolne dane
	const char* source_file;
	int source_line;
	
	struct resource_t *pnext, *pprev; // następny i poprzedni zasób
	
	uint32_t checksum;
	uint64_t magic2;
};


static struct resource_base_t {
	struct resource_t *phead, *ptail;
	
	size_t current_heap_size;
	size_t top_heap_size;
	
	struct rld_settings_t settings;

	FILE* debug_output_stream;

	jmp_buf exit_hook;
    int exit_hooked;
	int exit_allowed;
} rbase;

#define RLD_STREAM rbase.debug_output_stream


enum resource_validate_error_t {
	RVE_SUCCESS = 0,
	
	// blok zasobów
	
	RVE_INVALID_MAGIC1,
	RVE_INVALID_MAGIC2,
	RVE_INVALID_CHECKSUM,
	
	// RT_MEMORY - blok pamięci
	
	RVE_INVALID_HEAD_FENCE,
	RVE_INVALID_TAIL_FENCE
	
	// RT_STREAM
	// todo?
};

static struct limit_descriptor_t {
    enum heap_function_code_t call_type;

    // funkcje sterty
    struct {
        size_t singleshot;

        struct {
			size_t limit;
			size_t sum;
		} cumulative;
    } heap;

    // limit sukcesów
    struct {
        size_t limit;
        size_t counter;
    } success;

} rld_limit[__hfc_max];

static char* rld_severity_text[] = {
	[MSL_QUIET] = "<cisza>",
	[MSL_INFORMATION] = "Informacja",
	[MSL_WARNING] = "Ostrzeżenie",
	[MSL_FAILURE] = BOLDRED("PORAŻKA")
};

static char* rld_resource_validate_error_message[] = {
    [RVE_SUCCESS] = "Ok (RVE_SUCCESS)",
    [RVE_INVALID_MAGIC1] = "Zamazany początek bloku opisującego zasób (RVE_INVALID_MAGIC1)",
    [RVE_INVALID_MAGIC2] = "Zamazany koniec bloku opisującego zasób (RVE_INVALID_MAGIC2)",
    [RVE_INVALID_CHECKSUM] = "Zamazana suma kontrolna bloku opisującego zasób (RVE_INVALID_CHECKSUM)",
    [RVE_INVALID_HEAD_FENCE] = "Wykryto modyfikację obszaru pamięci przed zaalokowanym blokiem (RVE_INVALID_HEAD_FENCE)",
    [RVE_INVALID_TAIL_FENCE] = "Wykryto modyfikację obszaru pamięci za zaalokowanym blokiem (RVE_INVALID_TAIL_FENCE)"
};

static char* rld_message_text[] = {
	[RLD_UNDEFINED] = "Błąd nieokreślony; skontaktuj się z autorem",
	[RLD_HEAP_FUNCTIONS_DISABLED] = "Funkcje operacji na stercie są zablokowane; proszę ich nie używać",
	
	[RLD_MALLOC_NO_MEMORY] = "Brak wolnej pamięci dla funkcji malloc",
	[RLD_MALLOC_NO_MEMORY_DUE_LIMIT] = "Brak wolnej pamięci dla funkcji malloc (heap limit)",
	[RLD_MALLOC_NO_MEMORY_DUE_SINGLESHOT_LIMIT] = "Przekroczono limit alokowanego bloku w jednym wywołaniu funkcji malloc()",
	[RLD_MALLOC_NO_MEMORY_DUE_CUMULATIVE_LIMIT] = "Przekroczono skumulowany limit alokowanej pamięci dla funkcji malloc()",
	[RLD_MALLOC_FAILED_DUE_SUCCESS_LIMIT] = "Funkcja malloc() zakończyła się porażką ze wględu na wyczerpany limit wywołań, które mogą zakończyć  się sukcesem",
	[RLD_MALLOC_SUCCESSFUL] = "Blok pamięci o żądanej wielkości został pomyślnie przydzielony",

	[RLD_FREE_NULL] = "Próba zwolnienia wskaźnika NULL",
	[RLD_FREE_INVALID_POINTER] = "Próba zwolnienia niezaalokowanego wcześniej bloku pamięci (nieznany wskaźnik)",
	[RLD_FREE_SUCCESSFUL] = "Blok pamięci został pomyślnie zwolniony",
	
	[RLD_CALLOC_NO_MEMORY] = "Brak wolnej pamięci dla funkcji calloc",
	[RLD_CALLOC_NO_MEMORY_DUE_LIMIT] = "Brak wolnej pamięci dla funkcji calloc (heap limit)",
	[RLD_CALLOC_NO_MEMORY_DUE_SINGLESHOT_LIMIT] = "Przekroczono limit alokowanego bloku w jednym wywołaniu funkcji calloc()",
	[RLD_CALLOC_NO_MEMORY_DUE_CUMULATIVE_LIMIT] = "Przekroczono skumulowany limit alokowanej pamięci dla funkcji calloc()",
	[RLD_CALLOC_FAILED_DUE_SUCCESS_LIMIT] = "Funkcja calloc() zakończyła się porażką ze wględu na wyczerpany limit wywołań, które mogą zakończyć  się sukcesem",
	[RLD_CALLOC_SUCCESSFUL] = "Blok pamięci o żądanej wielkości został pomyślnie przydzielony",

	[RLD_REALLOC_NO_MEMORY] = "Brak wolnej pamięci dla funkcji realloc; wielkość bloku nie uległa zmianie",
	[RLD_REALLOC_NO_MEMORY_DUE_LIMIT] = "Brak wolnej pamięci dla funkcji realloc; wielkość bloku nie uległa zmianie (heap limit)",
	[RLD_REALLOC_NO_MEMORY_DUE_SINGLESHOT_LIMIT] = "Przekroczono limit alokowanego bloku w jednym wywołaniu funkcji realloc()",
	[RLD_REALLOC_NO_MEMORY_DUE_CUMULATIVE_LIMIT] = "Przekroczono skumulowany limit alokowanej pamięci dla funkcji realloc()",
	[RLD_REALLOC_FAILED_DUE_SUCCESS_LIMIT] = "Funkcja realloc() zakończyła się porażką ze wględu na wyczerpany limit wywołań, które mogą zakończyć  się sukcesem",
    [RLD_REALLOC_INVALID_POINTER] = "Próba zmiany rozmiaru niezaalokowanego wcześniej bloku pamięci (nieznany wskaźnik)",
	[RLD_REALLOC_SUCCESSFUL] = "Rozmiar bloku pamięci został zmieniony pomyślnie",

	//
	
	[RLD_STRDUP_NULL] = "Próba duplikowania tekstu o wskaźniku NULL",
	[RLD_STRDUP_NO_MEMORY] = "Brak wolnej pamięci dla funkcji strdup",
	[RLD_STRDUP_NO_MEMORY_DUE_LIMIT] = "Brak wolnej pamięci dla funkcji strdup (heap limit)",
	[RLD_STRDUP_NO_MEMORY_DUE_SINGLESHOT_LIMIT] = "Przekroczono limit alokowanego bloku w jednym wywołaniu funkcji strdup()",
	[RLD_STRDUP_NO_MEMORY_DUE_CUMULATIVE_LIMIT] = "Przekroczono skumulowany limit alokowanej pamięci dla funkcji strdup()",
	[RLD_STRDUP_FAILED_DUE_SUCCESS_LIMIT] = "Funkcja strdup() zakończyła się porażką ze wględu na wyczerpany limit wywołań, które mogą zakończyć  się sukcesem",
	[RLD_STRDUP_SUCCESSFUL] = "Pamięć dla kopii tekstu została pomyślnie przydzielona",
	
	[RLD_STRNDUP_NULL] = "Próba duplikowania tekstu o wskaźniku NULL",
	[RLD_STRNDUP_NO_MEMORY] = "Brak wolnej pamięci dla funkcji strndup",
	[RLD_STRNDUP_NO_MEMORY_DUE_LIMIT] = "Brak wolnej pamięci dla funkcji strndup (heap limit)",
	[RLD_STRNDUP_NO_MEMORY_DUE_SINGLESHOT_LIMIT] = "Przekroczono limit alokowanego bloku w jednym wywołaniu funkcji strndup()",
	[RLD_STRNDUP_NO_MEMORY_DUE_CUMULATIVE_LIMIT] = "Przekroczono skumulowany limit alokowanej pamięci dla funkcji strndup()",
	[RLD_STRNDUP_FAILED_DUE_SUCCESS_LIMIT] = "Funkcja strndup() zakończyła się porażką ze wględu na wyczerpany limit wywołań, które mogą zakończyć  się sukcesem",
	[RLD_STRNDUP_SUCCESSFUL] = "Pamięć dla ograniczonej kopii tekstu została pomyślnie przydzielona",


	[RLD_HEAP_BROKEN] = "Wykryto uszkodzenie pamięci starty. Któraś z poprzednich operacji wyszła po za swój zakres pamięci.",
	[RLD_HEAP_DATA_OUT_OF_BOUNDS] = "Wykryto naruszenie granic bloku pamięci.",


	//
	
	
	[RLD_FOPEN_SUCCESSFUL] = "Plik został pomyślnie otwarty",
	[RLD_FOPEN_FAILED] = "Nie udało się otworzyć pliku",
	[RLD_FOPEN_FAILED_DUE_SUCCESS_LIMIT] = "Funkcja fopen() zakończyła się porażką ze wględu na wyczerpany limit wywołań, które mogą zakończyć  się sukcesem",
	
	[RLD_FCLOSE_NULL_STREAM] = "Próba zamknięcia pliku reprezentowanego wartością NULL",
	[RLD_FCLOSE_INVALID_STREAM] = "Próba zamknięcia nieotwartego wcześniej pliku (nieznany uchwyt pliku)",
	[RLD_FCLOSE_SUCCESSFUL] = "Plik został pomyślnie zamknięty",
	
};


//
//
//
//
//

static const char* only_name(const char* full_path);
static uint32_t calc_checksum(const void* restrict buffer, size_t size);
static void update_checksum(struct resource_t* pres);
static enum resource_validate_error_t validate_resource(const struct resource_t* pres);
static int validate_heap(const char* caller_source_file, int caller_source_line);

static void rldebug_init(void)
{
	memset(&rbase, 0, sizeof(struct resource_base_t));
	srand(time(NULL));
	
	rbase.settings.lowest_reported_severity = MSL_INFORMATION;
	rbase.exit_allowed = 0;

	rbase.debug_output_stream = stdout;
//	rbase.debug_output_stream = stderr;
}


static struct resource_t* create_resource(enum resource_type_t res_type, const char* source_file, int source_line)
{
	struct resource_t* pres = (struct resource_t*)malloc(sizeof(struct resource_t));
	memset(pres, 0, sizeof(struct resource_t));
	assert(pres != NULL);

	pres->type = res_type;
	pres->pnext = NULL;
	pres->pprev = NULL;

	pres->source_file = source_file;
	pres->source_line = source_line;
	
	pres->magic1 = RLD_MAGIC1;
	pres->magic2 = RLD_MAGIC2;
	
	if (res_type == RT_MEMORY)
	{
		//pres->memory.head_pattern = ((uint64_t)rand() << 32) | (uint64_t)rand();
		//pres->memory.tail_pattern = ((uint64_t)rand() << 32) | (uint64_t)rand();
		
		for (int i = 0; i < 32; i++)
		{
			pres->memory.head_fence.pattern[i] = i + 1;
			pres->memory.tail_fence.pattern[i] = 32 - i;
		}
	}

	return pres;	
}

static void remove_resource(struct resource_t** ppres)
{
    assert(ppres != NULL && "remove_resource: pprese == NULL");
	struct resource_t* pres = *ppres;
	
	if (pres->type == RT_STREAM)
	{
		// nic do roboty
	} else if (pres->type == RT_MEMORY)
	{
		// nic do roboty
	}
	
	if (pres->pnext == NULL && pres->pprev == NULL && rbase.phead == pres && rbase.ptail == pres)
	{
		// tylko jeden element
		rbase.phead = NULL;
		rbase.ptail = NULL;
	} else if (pres->pprev == NULL && rbase.phead == pres)
	{
		// pierwszy
		rbase.phead = rbase.phead->pnext;
		rbase.phead->pprev = NULL;
		update_checksum(rbase.phead);
	} else if (pres->pnext == NULL && rbase.ptail == pres)
	{
		// ostatni
		rbase.ptail = rbase.ptail->pprev;
		rbase.ptail->pnext = NULL;
		update_checksum(rbase.ptail);
	} else if (pres->pnext != NULL && pres->pprev != NULL)
	{
		// środeczek
		struct resource_t *p1, *p2;
		p1 = pres->pprev;
		p2 = pres->pnext;
		pres->pprev->pnext = pres->pnext;
		pres->pnext->pprev = pres->pprev;
		update_checksum(p1);
		update_checksum(p2);
	} else
		assert(0 && "Naruszona spójność sterty");

	free(pres);
	*ppres = NULL;
}

static void add_resource(struct resource_t *pres)
{
    assert(pres != NULL && "add_resource: pres == NULL");

	if (rbase.phead == NULL)
	{
		rbase.phead = pres;
		rbase.ptail = pres;
	} else
	{
		pres->pprev = rbase.ptail;
		rbase.ptail->pnext = pres;
		rbase.ptail = pres;
	}
	
	update_checksum(pres);
	if (pres->pprev)
		update_checksum(pres->pprev);
		

	// Jeśli dodawany zasób jest blokiem pamięci, to zwiększ globalne statystyki zajęcia sterty
	if (pres->type == RT_MEMORY)
	{
		rbase.current_heap_size += pres->memory.size;
		rbase.top_heap_size = MAX(rbase.current_heap_size, rbase.top_heap_size);
	}
	//pres->checksum = 0;
	//pres->checksum = calc_checksum(pres, sizeof(struct resource_t));
}


static struct resource_t* find_resource(enum resource_type_t res_type, const void* handle)
{
	struct resource_t *presource = rbase.phead;
	for( ;presource != NULL; presource = presource->pnext)
	{
		if (presource->type != res_type)
			continue;
		
		if (res_type == RT_MEMORY && presource->memory.base_pointer == handle)
			return presource;
		if (res_type == RT_STREAM && presource->stream.stream == handle)
			return presource;
	}
	
	return NULL;
}

char* print_source_location(char* buffer, size_t buffer_size, const char* source_name, int source_line)
{
    if (source_name == NULL || source_line == -1)
    {
        if (buffer_size)
            buffer[0] = '\x0';
        return buffer;
    }

#if defined(_HTML_OUTPUT)
    snprintf(buffer, buffer_size, "<a href=\"source/%s.html#line-%d\">%s:%d</a>",
        only_name(source_name), source_line, only_name(source_name), source_line);
#else
    snprintf(buffer, buffer_size, "%s:%d", only_name(source_name), source_line);
#endif
    return buffer;
}

static void report(enum message_severity_level_t severity, enum rld_message_t msg_id, const char* source_name, int source_line, const char* message)
{
	if (severity >= rbase.settings.lowest_reported_severity)
	{

        char location[128];
        print_source_location(location, sizeof(location), source_name, source_line);

		if (message == NULL)
		    if (source_name == NULL || source_line == -1)
	    		fprintf(RLD_STREAM, BOLDPINK("Analiza zasobów")": %s: " BOLD("%s") "\n",
		    		rld_severity_text[severity], rld_message_text[msg_id]);
			else
				fprintf(RLD_STREAM, BOLDPINK("Analiza zasobów")": %s dla %s: " BOLD("%s") "\n",
    				rld_severity_text[severity], location, rld_message_text[msg_id]);

		else
		    if (source_name == NULL || source_line == -1)
                fprintf(RLD_STREAM, BOLDPINK("Analiza zasobów")": %s: " BOLD("%s") " [%s]\n",
                    rld_severity_text[severity], rld_message_text[msg_id], message);
            else
                fprintf(RLD_STREAM, BOLDPINK("Analiza zasobów")": %s dla %s: " BOLD("%s") " [%s]\n",
    				rld_severity_text[severity], location, rld_message_text[msg_id], message);
	}

	fflush(RLD_STREAM);
	
	if (severity == MSL_FAILURE)
		raise(SIGHEAP);
}

static void* setup_base_pointer(struct resource_t* pres)
{
	assert(pres->type == RT_MEMORY);
	
	uint8_t* base = pres->memory.base_pointer;
	*(struct block_fence_t*)base = pres->memory.head_fence;
	*(struct block_fence_t*)(base + sizeof(struct block_fence_t) + pres->memory.size) = pres->memory.tail_fence;
	
	void* user_space_pointer = base + sizeof(struct block_fence_t);
	return user_space_pointer;
}

static void* get_base_pointer(void* user_space_pointer)
{
	if (user_space_pointer == NULL)
		return NULL;
	return (uint8_t*)user_space_pointer - sizeof(struct block_fence_t);
}

void* _rldebug_heap_wrapper(enum heap_function_code_t call_type, void* user_pointer, size_t number, size_t size, const char* source_name, int source_line)
{
    char msg[128];
	validate_heap(source_name, source_line);

    //
    //
    //
	if (call_type == HFC_MALLOC)
	{
		// przekazanie liczby ujemnej daje liczbę dodatnią ponad SIZE_MAX/2
		if (number > SIZE_MAX >> 1)
			return NULL;
		
        if (rbase.settings.heap.global_disable)
			report(MSL_FAILURE, RLD_HEAP_FUNCTIONS_DISABLED, source_name, source_line, NULL);

        // limit wywołań mogących zakończyć się sukcesem
		if (++rld_limit[call_type].success.counter > rld_limit[call_type].success.limit)
		{
			report(MSL_INFORMATION, RLD_MALLOC_FAILED_DUE_SUCCESS_LIMIT, source_name, source_line, NULL);
			return NULL;
		}

        // limit pojedynczej alokacji
		if (number > rld_limit[call_type].heap.singleshot)
		{
		    snprintf(msg, sizeof(msg), "wynosi on %lu bajtów", rld_limit[call_type].heap.singleshot);
		    report(MSL_INFORMATION, RLD_MALLOC_NO_MEMORY_DUE_SINGLESHOT_LIMIT, source_name, source_line, msg);
			return NULL;
		}

        // skumulowany limit alokacji
		if (number + rld_limit[call_type].heap.cumulative.sum > rld_limit[call_type].heap.cumulative.limit)
		{
		    snprintf(msg, sizeof(msg), "wynosi on %lu bajtów", rld_limit[call_type].heap.cumulative.limit);
		    report(MSL_INFORMATION, RLD_MALLOC_NO_MEMORY_DUE_CUMULATIVE_LIMIT, source_name, source_line, msg);
			return NULL;
		}

		// ogranicznik sterty
		if (rbase.settings.heap.global_limit_active)
			if (rbase.current_heap_size + number > rbase.settings.heap.global_limit_value)
			{
				report(MSL_INFORMATION, RLD_MALLOC_NO_MEMORY_DUE_LIMIT, source_name, source_line, NULL);
				return NULL;
			}

		// normalna alokacja
		uint8_t* new_base_pointer = (uint8_t*)malloc(number + sizeof(struct block_fence_t) * 2);
		if (new_base_pointer == NULL)
		{
			report(MSL_INFORMATION, RLD_MALLOC_NO_MEMORY, source_name, source_line, NULL);
			return NULL;
		}

		struct resource_t *pres = create_resource(RT_MEMORY, source_name, source_line);
		pres->memory.size = number;
		pres->memory.base_pointer = new_base_pointer;
		pres->memory.allocated_by = call_type;

		add_resource(pres);
		rld_limit[call_type].heap.cumulative.sum += pres->memory.size;
		report(MSL_INFORMATION, RLD_MALLOC_SUCCESSFUL, source_name, source_line, NULL);

		return setup_base_pointer(pres);
	}

    //
    //
    //
	if (call_type == HFC_FREE)
	{
        if (rbase.settings.heap.global_disable)
			report(MSL_FAILURE, RLD_HEAP_FUNCTIONS_DISABLED, source_name, source_line, NULL);

		// libc - brak akcji
		if (user_pointer == NULL)
		{
			report(MSL_WARNING, RLD_FREE_NULL, source_name, source_line, NULL);
			return NULL;
		}

		void* base_pointer = get_base_pointer(user_pointer);

		struct resource_t *pres = find_resource(RT_MEMORY, base_pointer);
		if (pres == NULL)
			report(MSL_FAILURE, RLD_FREE_INVALID_POINTER, source_name, source_line, NULL);

        assert(IS_ALLOCATION_FUNCTION_CODE(pres->memory.allocated_by));
		rbase.current_heap_size -= pres->memory.size;
		rld_limit[pres->memory.allocated_by].heap.cumulative.sum -= pres->memory.size;
		assert(rld_limit[pres->memory.allocated_by].heap.cumulative.sum <= (SIZE_MAX >> 1));

		remove_resource(&pres);
		free(base_pointer);
		report(MSL_INFORMATION, RLD_FREE_SUCCESSFUL, source_name, source_line, NULL);

		return NULL;
	}
	
    //
    //
    //
	if (call_type == HFC_CALLOC)
	{
		// przekazanie liczby ujemnej daje liczbę dodatnią ponad SIZE_MAX/2
		size_t bytes = number * size;
		if (bytes > SIZE_MAX >> 1)
			return NULL;
		
        if (rbase.settings.heap.global_disable)
			report(MSL_FAILURE, RLD_HEAP_FUNCTIONS_DISABLED, source_name, source_line, NULL);

        // limit wywołań mogących zakończyć się sukcesem
		if (++rld_limit[call_type].success.counter > rld_limit[call_type].success.limit)
		{
			report(MSL_INFORMATION, RLD_CALLOC_FAILED_DUE_SUCCESS_LIMIT, source_name, source_line, NULL);
			return NULL;
		}


        // limit pojedynczej alokacji
		if (bytes > rld_limit[call_type].heap.singleshot)
		{
		    snprintf(msg, sizeof(msg), "wynosi on %lu bajtów", rld_limit[call_type].heap.singleshot);
		    report(MSL_INFORMATION, RLD_CALLOC_NO_MEMORY_DUE_SINGLESHOT_LIMIT, source_name, source_line, msg);
			return NULL;
		}

        // skumulowany limit alokacji
		if (number + rld_limit[call_type].heap.cumulative.sum > rld_limit[call_type].heap.cumulative.limit)
		{
		    snprintf(msg, sizeof(msg), "wynosi on %lu bajtów", rld_limit[call_type].heap.cumulative.limit);
		    report(MSL_INFORMATION, RLD_CALLOC_NO_MEMORY_DUE_CUMULATIVE_LIMIT, source_name, source_line, msg);
			return NULL;
		}

		// ogranicznik sterty
		if (rbase.settings.heap.global_limit_active)
			if (rbase.current_heap_size + bytes > rbase.settings.heap.global_limit_value)
			{
				report(MSL_INFORMATION, RLD_CALLOC_NO_MEMORY_DUE_LIMIT, source_name, source_line, NULL);
				return NULL;
			}
			
		// normalna alokacja
		uint8_t* new_base_pointer = (uint8_t*)malloc(bytes + sizeof(struct block_fence_t) * 2);
		if (new_base_pointer == NULL)
		{
			report(MSL_INFORMATION, RLD_CALLOC_NO_MEMORY, source_name, source_line, NULL);
			return NULL;
		} else
			memset(new_base_pointer, 0x00, bytes + sizeof(struct block_fence_t) * 2);

		
		struct resource_t *pres = create_resource(RT_MEMORY, source_name, source_line);
		pres->memory.size = bytes;
		pres->memory.base_pointer = new_base_pointer;
		pres->memory.allocated_by = call_type;

		add_resource(pres);
		rld_limit[call_type].heap.cumulative.sum += pres->memory.size;
		report(MSL_INFORMATION, RLD_CALLOC_SUCCESSFUL, source_name, source_line, NULL);

		return setup_base_pointer(pres);
	}
	
    //
    // void* realloc(void*, size_t)
    //
	if (call_type == HFC_REALLOC)
	{
		// przekazanie liczby ujemnej daje liczbę dodatnią ponad SIZE_MAX/2
		if (number > SIZE_MAX >> 1)
			return NULL;
		
        if (rbase.settings.heap.global_disable)
			report(MSL_FAILURE, RLD_HEAP_FUNCTIONS_DISABLED, source_name, source_line, NULL);

        // limit wywołań mogących zakończyć się sukcesem
		if (++rld_limit[call_type].success.counter > rld_limit[call_type].success.limit)
		{
			report(MSL_INFORMATION, RLD_REALLOC_FAILED_DUE_SUCCESS_LIMIT, source_name, source_line, NULL);
			return NULL;
		}

		// normalna alokacja		
		void* base_pointer = get_base_pointer(user_pointer);
		struct resource_t *pres = find_resource(RT_MEMORY, base_pointer);
		if (base_pointer != NULL && pres == NULL)
			report(MSL_FAILURE, RLD_REALLOC_INVALID_POINTER, source_name, source_line, NULL);

		// Jeżeli wywołanie zmienia rozmiar bufora, to analizuj tylko przyrost
		int64_t allocation_delta = (int64_t)number;
		if (pres != NULL)
			allocation_delta = (int64_t)number - (int64_t)pres->memory.size;

		// nie ma żadnej zmiany?
		if (allocation_delta == 0)
			return user_pointer;
			
		// limit pojedynczej alokacji
		if ((pres == NULL) && (number > rld_limit[call_type].heap.singleshot) ||
			(pres != NULL) && (number > pres->memory.size)
						   && (allocation_delta > (int64_t)rld_limit[call_type].heap.singleshot))
		{
		    snprintf(msg, sizeof(msg), "wynosi on %lu bajtów", rld_limit[call_type].heap.singleshot);
		    report(MSL_INFORMATION, RLD_REALLOC_NO_MEMORY_DUE_SINGLESHOT_LIMIT, source_name, source_line, msg);
			return NULL;
		}

        // skumulowany limit alokacji
		if ((pres == NULL && (number + rld_limit[call_type].heap.cumulative.sum > rld_limit[call_type].heap.cumulative.limit)) ||
			(pres != NULL && (number > pres->memory.size)
						  && (allocation_delta + (int64_t)rld_limit[call_type].heap.cumulative.sum > (int64_t)rld_limit[call_type].heap.cumulative.limit)))
			{
				snprintf(msg, sizeof(msg), "wynosi on %lu bajtów", rld_limit[call_type].heap.cumulative.limit);
				report(MSL_INFORMATION, RLD_REALLOC_NO_MEMORY_DUE_CUMULATIVE_LIMIT, source_name, source_line, msg);
				return NULL;
			}

		// ogranicznik sterty
		if (rbase.settings.heap.global_limit_active)
			if ((int64_t)rbase.current_heap_size + allocation_delta > (int64_t)rbase.settings.heap.global_limit_value)
			{
				report(MSL_INFORMATION, RLD_REALLOC_NO_MEMORY_DUE_LIMIT, source_name, source_line, NULL);
				return NULL;
			}
				
		void* new_base_pointer = realloc(base_pointer, number + sizeof(struct block_fence_t) * 2);
		if (new_base_pointer == NULL)
		{
			// Teraz to już naprawdę brakuje pamięci :)
			report(MSL_INFORMATION, RLD_REALLOC_NO_MEMORY, source_name, source_line, NULL);
			return NULL;
		}

		if (base_pointer == NULL)
		{
			//
			// Nie przekazano wskaźnika do realloc() - funkcja działa jak malloc 
			//
			pres = create_resource(RT_MEMORY, source_name, source_line);
			pres->memory.size = number;
			pres->memory.base_pointer = new_base_pointer;
			pres->memory.allocated_by = call_type;

			add_resource(pres);
			rld_limit[call_type].heap.cumulative.sum += pres->memory.size;
			report(MSL_INFORMATION, RLD_REALLOC_SUCCESSFUL, source_name, source_line, NULL);

			return setup_base_pointer(pres);
		} else
		{

			size_t old_size = pres->memory.size;
			enum heap_function_code_t old_code = pres->memory.allocated_by;
            assert(IS_ALLOCATION_FUNCTION_CODE(old_code));

			pres->memory.allocated_by = call_type;
			pres->memory.size = number;
			pres->memory.base_pointer = new_base_pointer;
			pres->source_line = source_line;
			pres->source_file = source_name;
			
			update_checksum(pres);
			if (pres->pprev)
				update_checksum(pres->pprev);
			if (pres->pnext)
				update_checksum(pres->pnext);

			// wycofaj liczbę bajtów zaalokowaną w tym bloku poprzednim wywołaniem (old_code)
			rbase.current_heap_size -= old_size;
			rld_limit[old_code].heap.cumulative.sum -= old_size;
    		assert(rld_limit[old_code].heap.cumulative.sum < (SIZE_MAX >> 1));

			// uzupełnij liczniki o nową wielkość bloku
			rbase.current_heap_size += pres->memory.size;
			rld_limit[call_type].heap.cumulative.sum += pres->memory.size;

			rbase.top_heap_size = MAX(rbase.current_heap_size, rbase.top_heap_size);
		}

		report(MSL_INFORMATION, RLD_REALLOC_SUCCESSFUL, source_name, source_line, NULL);
		return setup_base_pointer(pres);
	}	
	
	assert(0 && "_rldebug_heap_wrapper: niezaimplementowana funkcja");
}

char* _rldebug_str_wrapper(enum heap_function_code_t call_type, const char* pstring, size_t number, const char* source_file, int source_line)
{
    char msg[128];
	validate_heap(source_file, source_line);

    //
    //
    //
	if (call_type == HFC_STRDUP)
	{
        if (rbase.settings.heap.global_disable)
			report(MSL_FAILURE, RLD_HEAP_FUNCTIONS_DISABLED, source_file, source_line, NULL);

		if (pstring == NULL)
			report(MSL_FAILURE, RLD_STRDUP_NULL, source_file, source_line, NULL);

        // limit wywołań mogących zakończyć się sukcesem
		if (++rld_limit[call_type].success.counter > rld_limit[call_type].success.limit)
		{
			report(MSL_INFORMATION, RLD_STRDUP_FAILED_DUE_SUCCESS_LIMIT, source_file, source_line, NULL);
			return NULL;
		}

		size_t bytes = strlen(pstring) + 1;

        // limit pojedynczej alokacji
		if (bytes > rld_limit[call_type].heap.singleshot)
		{
		    snprintf(msg, sizeof(msg), "wynosi on %lu bajtów", rld_limit[call_type].heap.singleshot);
		    report(MSL_INFORMATION, RLD_STRDUP_NO_MEMORY_DUE_SINGLESHOT_LIMIT, source_file, source_line, msg);
			return NULL;
		}

        // skumulowany limit alokacji
		if (number + rld_limit[call_type].heap.cumulative.sum > rld_limit[call_type].heap.cumulative.limit)
		{
		    snprintf(msg, sizeof(msg), "wynosi on %lu bajtów", rld_limit[call_type].heap.cumulative.limit);
		    report(MSL_INFORMATION, RLD_STRDUP_NO_MEMORY_DUE_CUMULATIVE_LIMIT, source_file, source_line, msg);
			return NULL;
		}

		// ogranicznik sterty
		if (rbase.settings.heap.global_limit_active)
			if (rbase.current_heap_size + bytes > rbase.settings.heap.global_limit_value)
			{
				report(MSL_INFORMATION, RLD_STRDUP_NO_MEMORY_DUE_LIMIT, source_file, source_line, NULL);
				return NULL;
			}

		// normalna alokacja		
		uint8_t* new_base_pointer = (uint8_t*)malloc(bytes + sizeof(struct block_fence_t) * 2);
		if (new_base_pointer == NULL)
		{
			report(MSL_INFORMATION, RLD_STRDUP_NO_MEMORY, source_file, source_line, NULL);
			return NULL;
		}

		struct resource_t *pres = create_resource(RT_MEMORY, source_file, source_line);
		pres->memory.size = bytes;
		pres->memory.base_pointer = new_base_pointer;
		pres->memory.allocated_by = call_type;

		add_resource(pres);
		rld_limit[call_type].heap.cumulative.sum += pres->memory.size;
		report(MSL_INFORMATION, RLD_STRDUP_SUCCESSFUL, source_file, source_line, NULL);

		char* user_pointer = setup_base_pointer(pres);
		strcpy(user_pointer, pstring);

		return user_pointer;
	}
	
    //
    //
    //
	if (call_type == HFC_STRNDUP)
	{
        if (rbase.settings.heap.global_disable)
			report(MSL_FAILURE, RLD_HEAP_FUNCTIONS_DISABLED, source_file, source_line, NULL);

		if (pstring == NULL)
			report(MSL_FAILURE, RLD_STRNDUP_NULL, source_file, source_line, NULL);

        // limit wywołań mogących zakończyć się sukcesem
		if (++rld_limit[call_type].success.counter > rld_limit[call_type].success.limit)
		{
			report(MSL_INFORMATION, RLD_STRNDUP_FAILED_DUE_SUCCESS_LIMIT, source_file, source_line, NULL);
			return NULL;
		}

		size_t bytes = strlen(pstring);
		bytes = MIN(bytes, number);

        // limit pojedynczej alokacji
		if (bytes > rld_limit[call_type].heap.singleshot)
		{
		    char msg[128];
		    snprintf(msg, sizeof(msg), "wynosi on %lu bajtów", rld_limit[call_type].heap.singleshot);
		    report(MSL_INFORMATION, RLD_STRNDUP_NO_MEMORY_DUE_SINGLESHOT_LIMIT, source_file, source_line, msg);
			return NULL;
		}

        // skumulowany limit alokacji
		if (number + rld_limit[call_type].heap.cumulative.sum > rld_limit[call_type].heap.cumulative.limit)
		{
		    snprintf(msg, sizeof(msg), "wynosi on %lu bajtów", rld_limit[call_type].heap.cumulative.limit);
		    report(MSL_INFORMATION, RLD_STRNDUP_NO_MEMORY_DUE_CUMULATIVE_LIMIT, source_file, source_line, msg);
			return NULL;
		}

		// ogranicznik sterty
		if (rbase.settings.heap.global_limit_active)
			if (rbase.current_heap_size + bytes > rbase.settings.heap.global_limit_value)
			{
				report(MSL_INFORMATION, RLD_STRNDUP_NO_MEMORY_DUE_LIMIT, source_file, source_line, NULL);
				return NULL;
			}

		// normalna alokacja		
		uint8_t* new_base_pointer = (uint8_t*)malloc(bytes + 1 + sizeof(struct block_fence_t) * 2);
		if (new_base_pointer == NULL)
		{
			report(MSL_INFORMATION, RLD_STRNDUP_NO_MEMORY, source_file, source_line, NULL);
			return NULL;
		}

		struct resource_t *pres = create_resource(RT_MEMORY, source_file, source_line);
		pres->memory.size = bytes + 1;
		pres->memory.base_pointer = new_base_pointer;
		pres->memory.allocated_by = call_type;

		add_resource(pres);
		rld_limit[call_type].heap.cumulative.sum += pres->memory.size;
		report(MSL_INFORMATION, RLD_STRNDUP_SUCCESSFUL, source_file, source_line, NULL);

		char* user_pointer = setup_base_pointer(pres);
		memcpy(user_pointer, pstring, bytes);
		user_pointer[bytes] = '\x0';

		return user_pointer;			
	}

	assert(0 && "_rldebug_str_wrapper: niezaimplementowana funkcja");
}


void* _rldebug_io_wrapper(enum heap_function_code_t call_type, FILE* stream, const char* stream_name, const char* stream_mode, const char* source_file, int source_line)
{
    char sys_message[256];
	validate_heap(source_file, source_line);

    //
    //
    //
	if (call_type == HFC_FOPEN)
	{
        // limit wywołań mogących zakończyć się sukcesem
		if (++rld_limit[call_type].success.counter > rld_limit[call_type].success.limit)
		{
			report(MSL_INFORMATION, RLD_FOPEN_FAILED_DUE_SUCCESS_LIMIT, source_file, source_line, NULL);
			return NULL;
		}

		FILE* fhandle = fopen(stream_name, stream_mode);
		if (fhandle == NULL)
		{
			snprintf(sys_message, sizeof(sys_message), "%s; errno=%d", strerror(errno), errno);
			report(MSL_INFORMATION, RLD_FOPEN_FAILED, source_file, source_line, sys_message);
			return NULL;
		}

		struct resource_t *pres = create_resource(RT_STREAM, source_file, source_line);
		pres->stream.name = strdup(stream_name);
		pres->stream.mode = strdup(stream_mode);
		pres->stream.stream = fhandle;

		assert(pres->stream.name != NULL && pres->stream.mode != NULL);

		add_resource(pres);
		report(MSL_INFORMATION, RLD_FOPEN_SUCCESSFUL, source_file, source_line, NULL);

		return fhandle;
	}
	
    //
    //
    //
	if (call_type == HFC_FCLOSE)
	{
		if (stream == NULL)
			report(MSL_FAILURE, RLD_FCLOSE_NULL_STREAM, source_file, source_line, NULL);

		struct resource_t *pres = find_resource(RT_STREAM, (void*)stream);

		if (pres == NULL)
			report(MSL_FAILURE, RLD_FCLOSE_INVALID_STREAM, source_file, source_line, NULL);

		intptr_t result = fclose(stream);
		remove_resource(&pres);
		report(MSL_INFORMATION, RLD_FCLOSE_SUCCESSFUL, source_file, source_line, NULL);
		return (void*)result;
	}

	assert(0 && "_rldebug_mem_wrapper: niezaimplementowana funkcja");
}

void __attribute__((noreturn)) _rldebug_stdlib_noreturn_wrapper(enum heap_function_code_t call_type, int iarg, const char* source_file, int source_line)
{
	validate_heap(source_file, source_line);

    //
    //
    //
    if (call_type == HFC_EXIT)
    {
		if (rbase.exit_hooked)
			longjmp(rbase.exit_hook, 0x0100 | (iarg) & 0xFF);

		if (rbase.exit_allowed)
			exit(iarg);

		// nie używać exit()
        char location[128];
        print_source_location(location, sizeof(location), source_file, source_line);
		fprintf(RLD_STREAM, "\n" BOLDRED("*** Użyto funkcji exit(int) w %s. Pozwala on na natychmiastowe wyjście z programu, co uniemożliwia finalizację testów.\n"), location);
		fprintf(RLD_STREAM, "*** Proszę poprawić swój kod tak aby niewykorzystywać funkcji exit()\n");
		fprintf(RLD_STREAM, "*** W przypadku wątpliwości proszę skontaktować się z autorem testu.\n");
        raise(SIGTERM);
    }

	assert(0 && "_rldebug_stdlib_noreturn_wrapper: niezaimplementowany call_type");
}


int rldebug_show_leaked_resources(int force_empty_summary)
{
	uint32_t blocks = 0;
	uint32_t streams = 0;
	uint64_t memory_leaked = 0;
	
	// policz elementy
	for(struct resource_t *presource = rbase.phead; presource != NULL; presource = presource->pnext)
	{
		blocks += presource->type == RT_MEMORY;
		streams += presource->type == RT_STREAM;
	}

	if (blocks || streams || force_empty_summary)
	{
		//fprintf(RLD_STREAM, BYELLOW("** "BOLD("RLDebug")" :: Analizator wycieku zasobów ***")"\n");
		fflush(RLD_STREAM);
	}

	// pamięć - lista wycieków
	if (blocks > 0)
	{
		fprintf(RLD_STREAM, "\n" BOLDRED("Wycieki pamięci") ":\n");
		fprintf(RLD_STREAM, "--------------------------------------------\n");
		fprintf(RLD_STREAM, " ID                Adres       Plik źródłowy\n");
		fprintf(RLD_STREAM, "           Liczba bajtów       Numer linii  \n");
		fprintf(RLD_STREAM, "--------------------------------------------\n");
		fflush(RLD_STREAM);
		
		int i = 1; 
		for(struct resource_t *presource = rbase.phead; presource != NULL; presource = presource->pnext, i++)
		{
			if (presource->type != RT_MEMORY)
				continue;

#if defined(_HTML_OUTPUT)
			fprintf(RLD_STREAM, " %-3d  %18p       <a href=\"source/%s.html#line-%d\">%s</a>\n", i, presource->memory.base_pointer,
			    only_name(presource->source_file), presource->source_line, only_name(presource->source_file));
#else
			fprintf(RLD_STREAM, " %-3d  %18p       %s\n", i, presource->memory.base_pointer, only_name(presource->source_file));
#endif
			fprintf(RLD_STREAM, "      %18lu       %d\n",    presource->memory.size, presource->source_line);
			fflush(RLD_STREAM);
			memory_leaked += presource->memory.size;
		}
		
		fprintf(RLD_STREAM, "--------------------------------------------\n");
	}
	
	// pamięć - podsumowanie wycieków
	if (blocks > 0 || force_empty_summary)
	{
	    if (blocks > 0)
	    {
            fprintf(RLD_STREAM, "Liczba niezwolnionych bloków pamięci: " BOLDRED("%d") " blok(ów)\n", blocks);
            fprintf(RLD_STREAM, "Sumaryczna wielkość wycieku pamięci: " BOLDRED("%lu") " bajt(ów)\n", memory_leaked);
        } else
            fprintf(RLD_STREAM, BOLDGREEN("Wszystkie bloki pamięci zostały pomyślnie zwolnione - brak wycieków.") "\n");
	}
	
	// pliki - lista niezamkniętych plików
	if (streams > 0)
	{
//		if (blocks)
//			fprintf(RLD_STREAM, "\n");
		fprintf(RLD_STREAM, "\n"BOLDRED("Niezamknięte pliki")":\n");
		fprintf(RLD_STREAM, "--------------------------------------------\n");
		fprintf(RLD_STREAM, " ID  Nazwa                     Plik źródłowy\n");
		fprintf(RLD_STREAM, "     Tryb                      Numer linii  \n");
		fprintf(RLD_STREAM, "--------------------------------------------\n");
		fflush(RLD_STREAM);
		
		int i = 1; 
		for(struct resource_t *presource = rbase.phead; presource != NULL; presource = presource->pnext, i++)
		{
			if (presource->type != RT_STREAM)
				continue;
				
			char fname[32] = {0};
			
			if (strlen(presource->stream.name) > 25)
			{
				strncpy(fname, presource->stream.name, 25-5);
				strcat(fname, "(...)");
			} else
				strncpy(fname, presource->stream.name, 25);
			

#if defined(_HTML_OUTPUT)
			fprintf(RLD_STREAM, " %-3d %-25s <a href=\"source/%s.html#line-%d\">%s</a>\n", i, fname,
			    only_name(presource->source_file), presource->source_line, only_name(presource->source_file));
#else
			fprintf(RLD_STREAM, " %-3d %-25s %s\n", i, fname, only_name(presource->source_file));
#endif

			fprintf(RLD_STREAM, "     %-25s %d\n",    presource->stream.mode, presource->source_line);
			fflush(RLD_STREAM);
		}
		
		fprintf(RLD_STREAM, "--------------------------------------------\n");

	}
	
	// pliki - podsumowanie
	if (streams > 0 || force_empty_summary)
	{
	    if (streams > 0)
		    fprintf(RLD_STREAM, "Liczba niezamkniętych plików: "BOLDRED("%d")"\n", streams);
		else
		    fprintf(RLD_STREAM, BOLDGREEN("Wszystkie pliki zostały zamknięte.\n"));
	}

	assert(memory_leaked == rbase.current_heap_size);

    if (force_empty_summary)
    {
        fprintf(RLD_STREAM, BOLDGREEN("Nie wykryto uszkodzenia sterty.\n"));
        fprintf(RLD_STREAM, "\n");
    }

	return streams + blocks;
}


size_t rldebug_heap_get_leak_size(void)
{
	size_t leak = 0;

	// policz elementy
	for(struct resource_t *presource = rbase.phead; presource != NULL; presource = presource->pnext)
	{
	    if (presource->type != RT_MEMORY)
	        continue;

	    leak += presource->memory.size;
	}

	return leak;
}

size_t rldebug_get_block_size(const void* ptr)
{
    for(struct resource_t *presource = rbase.phead; presource != NULL; presource = presource->pnext)
    {
        if (presource->type != RT_MEMORY)
            continue;
        if (ptr == (struct block_fence_t*)presource->memory.base_pointer + 1)
            return presource->memory.size;

    }
    return RLD_UNKNOWN_POINTER;
}

static const char* only_name(const char* full_path)
{
	char* p = strrchr(full_path, '/');
	return p ? p + 1 : full_path;
}

static uint32_t calc_checksum(const void* restrict buffer, size_t size)
{
	uint32_t chk = 0;
	const uint8_t* restrict ptr = (const uint8_t* restrict)buffer;
	while(size--)
		chk = ((chk ^ *ptr++) << 1) ^ !(chk & 1 << 31);
	return chk;
}

static void update_checksum(struct resource_t* pres)
{
	pres->checksum = 0;
	pres->checksum = calc_checksum(pres, sizeof(struct resource_t));
}


static enum resource_validate_error_t validate_resource(const struct resource_t* pres)
{
	// wstępne sprawdzenie
	if (pres->magic1 != RLD_MAGIC1)
		return RVE_INVALID_MAGIC1; // uszkodzona magiczna liczba na początku deskryptora zasobu
	if (pres->magic2 != RLD_MAGIC2)
		return RVE_INVALID_MAGIC2; // uszkodzona magiczna liczba na końcu deskryptora zasobu
		
	// i test właściwy
	struct resource_t temp = *pres;
	temp.checksum = 0;
	uint32_t chk = calc_checksum(&temp, sizeof(struct resource_t));
	if (chk != pres->checksum)
		return RVE_INVALID_CHECKSUM; // suma kontrolna zasobu jest błędna
		
	if (pres->type == RT_MEMORY)
	{
		struct block_fence_t* head_fence = (struct block_fence_t*)pres->memory.base_pointer;
		struct block_fence_t* tail_fence = (struct block_fence_t*)((uint8_t*)pres->memory.base_pointer + sizeof(struct block_fence_t) + pres->memory.size);
		if (memcmp(head_fence, &pres->memory.head_fence, sizeof(struct block_fence_t)) != 0)
			return RVE_INVALID_HEAD_FENCE; // uszkodzony płotek na początku bloku danych
		if (memcmp(tail_fence, &pres->memory.tail_fence, sizeof(struct block_fence_t)) != 0)
			return RVE_INVALID_TAIL_FENCE; // uszkodzony płotek na końcu bloku danych
	}
	
	return RVE_SUCCESS; // wszystko wydaje się być w porządku
}

static int validate_heap(const char* caller_source_file, int caller_source_line)
{
	for(struct resource_t *presource = rbase.phead; presource != NULL; presource = presource->pnext)
	{
		int vderror;
		if ((vderror = validate_resource(presource)) != RVE_SUCCESS)
		{
			char msg[512];
			//sprintf(msg, "vderror=%d", vderror);
			
			if (vderror == RVE_INVALID_MAGIC1 || vderror == RVE_INVALID_MAGIC2 || vderror == RVE_INVALID_CHECKSUM)
			{
				char loc1[128];
                print_source_location(loc1, sizeof(loc1), caller_source_file, caller_source_line);
                snprintf(msg, sizeof(msg), "Sterta zawiera uszkodzony blok pamięci. Problem zauważono w trakcie wykonywania %s. Opis: %s", loc1, rld_resource_validate_error_message[vderror]);

				report(MSL_FAILURE, RLD_HEAP_BROKEN, NULL, -1, msg);
			}

/*
			sprintf(msg, "vderror=%d; blok utworzony w %s:%d", vderror, only_name(presource->source_file), presource->source_line);

			if (vderror == RVE_INVALID_HEAD_FENCE || RVE_INVALID_TAIL_FENCE)
				report(MSL_FAILURE, RLD_HEAP_DATA_OUT_OF_BOUNDS, caller_source_file, caller_source_line, msg);
*/

			if (vderror == RVE_INVALID_HEAD_FENCE || RVE_INVALID_TAIL_FENCE)
			{
                char loc1[128], loc2[128];
                print_source_location(loc1, sizeof(loc1), presource->source_file, presource->source_line);
                print_source_location(loc2, sizeof(loc2), caller_source_file, caller_source_line);

                snprintf(msg, sizeof(msg), "Uszkodzony został blok zaalokowany w %s a samo uszkodzenie zauważono w trakcie wykonywania %s. Opis: %s", loc1, loc2, rld_resource_validate_error_message[vderror]);

                report(MSL_FAILURE, RLD_HEAP_DATA_OUT_OF_BOUNDS, NULL, -1, msg);
            }

			assert(!vderror);
			return 0;
		
		}
	}
	//printf("ok");
	return 1;
}


static void __attribute__ ((constructor)) __rldebug_startup()
{
    rldebug_init();
	rldebug_reset_limits();
}

//
//
// RLDebugger 
//
//

void rldebug_reset_limits(void)
{
	//
	rbase.settings.heap.global_limit_active = 0;
	rbase.settings.heap.global_disable = 0;

	for (int command_id = __hfc_min; command_id < __hfc_max; command_id++)
	{
	    struct limit_descriptor_t *plim = &rld_limit[command_id];
	    plim->call_type = command_id;

	    // limity pamieci
//	    plim->heap.general = RLD_HEAP_UNLIMITED;
	    plim->heap.singleshot = RLD_HEAP_UNLIMITED;
	    plim->heap.cumulative.limit = RLD_HEAP_UNLIMITED;
	    plim->heap.cumulative.sum = 0;


	    // limity sukcesów
	    plim->success.limit = RLD_HEAP_UNLIMITED;
	    plim->success.counter = 0;

	}
}

void rldebug_heap_set_global_limit(size_t heap_limit)
{
	if (heap_limit == RLD_HEAP_UNLIMITED)
	{
		// wyłącz ogranicznik
		rbase.settings.heap.global_limit_active = 0;
	} else
	{
		// włącz 
		rbase.settings.heap.global_limit_value = heap_limit;
		rbase.settings.heap.global_limit_active = 1;
	}
}


void rldebug_heap_disable_all_functions(int disable)
{
	rbase.settings.heap.global_disable = disable;
}


void rldebug_heap_set_function_singleshot_limit(enum heap_function_code_t call_type, size_t limit)
{
	assert(call_type > __hfc_min && call_type < __hfc_max);
	rld_limit[call_type].heap.singleshot = limit;
}

void rldebug_heap_set_function_cumulative_limit(enum heap_function_code_t call_type, size_t limit)
{
	assert(call_type > __hfc_min && call_type < __hfc_max);
	rld_limit[call_type].heap.cumulative.limit = limit;
}


void rldebug_set_function_success_limit(enum heap_function_code_t call_type, size_t limit)
{
	assert(call_type > __hfc_min && call_type < __hfc_max);
	rld_limit[call_type].success.limit = limit;
}


void rldebug_set_reported_severity_level(enum message_severity_level_t lowest_level)
{
	rbase.settings.lowest_reported_severity = lowest_level;
}


//
//
//
//
//

void remove_single_newline(void)
{

	int old_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, old_flags | O_NONBLOCK);

	int ch = getchar();

	fcntl(STDIN_FILENO, F_SETFL, old_flags);

	if(ch != EOF && ch != '\n')
		ungetc(ch, stdin);
}


int rdebug_call_main(int (*pmain)(int, char**, char**), int argc, char** argv, char** envp)
{
	assert(pmain != NULL);
	//jmp_buf main_return_hook;

	volatile int jstatus = setjmp(rbase.exit_hook);
	int ret_code = 0;
	if (!jstatus)
	{
	    rbase.exit_hooked = 1;

		ret_code = (int8_t)pmain(argc, argv, envp);


		rbase.exit_hooked = 0;
	} else {
	    rbase.exit_hooked = 0;
		assert((jstatus & 0xFF00) == 0x0100);
		ret_code = (int8_t)(jstatus & 0xFF);
	}

    // jeśli w buforze klawiatury zaplątał się znak nowej linii, to go skasuj
    remove_single_newline();
	return ret_code;
}

