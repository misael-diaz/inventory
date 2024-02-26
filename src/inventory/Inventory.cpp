#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cctype>
#include <cmath>

#define MAX_STRING_LEN (0x03ff)
#define MAX_BUFFER_SIZE (MAX_STRING_LEN + 1)
#define HASH ((size_t) 0xffff20240feb0025)

typedef struct m_chain_s {
	struct m_chain_s *prev;
	struct m_chain_s *next;
	void *data;
	size_t hash;
	size_t size;
} m_chain_t;

typedef enum {
	A,
	B,
	C,
} kind_t;

struct Kind
{
	kind_t kind;
	Kind(kind_t const kind);
	kind_t k(void) const;
	static const char *stringify(const Kind *kind);
	static kind_t enumerator(const char *kind);
	void *operator new(size_t size);
	void operator delete(void *p);
};

static m_chain_t m_chain;
static size_t m_size = 0;
static size_t m_count = 0;

static size_t _sz_ = 0;		// size of temporary placeholder
static char *_temp_[] = {NULL};	// temporary placeholder for fetching the entire line
static char *_code_[] = {NULL};	// shoe reference code could be alpha numeric
static char *_info_[] = {NULL};	// shoe information might be a phrase
static double _size_ = 0;	// shoe size
static char _avail_ = 0;	// shoe availability (Y/N)
static double _cost_ = 0;	// shoe cost
static double _sale_ = 0;	// shoe sale value
static double _number_ = 0;	// placeholder for real numbers
static double _count_ = 0;	// shoe count

// headers:
void head(void);
void header(void);
// getters:
void gcode(void);
void ginfo(void);
void gsize(void);
void gavail(void);
void gcost(void);
void gsale(void);
void gcount(void);
// loggers:
void code(void);
void info(void);
void size(void);
void avail(void);
void cost(void);
void sale(void);
void count(void);
void total(void);
void profit(void);
void greet(void);
// memory handling utilities:
void init(void);
void cleanup(void);
void *Util_Malloc(size_t const sz);
void *Util_Free(void *p);
void Util_Clear(void);
// console manipulators:
void clear(void);
void pause(void);

int main ()
{
	init();
	head();

	gcode();
	ginfo();
	gsize();
	gavail();
	gcost();
	gsale();
	gcount();

	clear();

	header();
	code();
	info();
	size();
	avail();
	cost();
	sale();
	count();
	total();
	profit();
	greet();

	cleanup();
	pause();
	return EXIT_SUCCESS;
}

static m_chain_t *Util_Chain (m_chain_t *node)
{
	m_chain_t *next = (m_chain.next)? m_chain.next : NULL;
	if (next) {
		next->prev = node;
	}

	node->next = next;
	node->prev = &m_chain;
	m_chain.next = node;
	return node;
}

static m_chain_t *Util_Remove (m_chain_t *node)
{
	m_chain_t *prev = node->prev;
	m_chain_t *next = node->next;
	if (next) {
		next->prev = prev;
	}

	prev->next = next;
	node->next = NULL;
	node->prev = NULL;
	node->data = NULL;
	free(node);
	node = NULL;
	return node;
}

static bool isValidSymbol (char const c)
{
	if (c == '+' || c == '-' || c == ',' || c == '.' || c == 'e' || c == 'E') {
		return true;
	} else {
		return false;
	}
}

static bool isNumber (char const c)
{
	if (c >= '0' && c <= '9') {
		return true;
	} else {
		return false;
	}
}

static void skipWhiteSpace (char **txt)
{
	while (**txt && **txt != '\n' && **txt <= ' ') {
		++*txt;
	}
}

static void nullTrailWhiteSpace (char **text)
{
	size_t const len = strlen(*text);
	char *txt = *text;
	char *iter = &txt[len - 1];
	while (*iter <= ' ') {
		*iter = 0;
		--iter;
	}
}

static bool is_numeric (char **text)
{
	char *start = *text;
	if (!**text || **text == '\n') {
		return false;
	}

	skipWhiteSpace(text);
	if (!**text || **text == '\n') {
		*text = start;
		return false;
	}

	char *iter = *text;
	bool space = false;
	while (*iter && *iter != '\n') {

		if (*iter <= ' ' && !space) {
			space = true;
			++iter;
			continue;
		}

		// ignores trailing whitespace
		if (*iter <= ' ' && space) {
			++iter;
			continue;
		}

		if (*iter > ' ' && space) {
			*text = start;
			return false;
		}

		if (!isValidSymbol(*iter) && !isNumber(*iter)) {
			*text = start;
			return false;
		}

		++iter;
	}

	*text = start;
	return true;
}

static bool toNumber (char **text)
{
	bool invalid = true;
	errno = 0;
	char *endptr[] = {NULL};
	_number_ = strtod(*text, endptr);
	if (errno == ERANGE) {
		invalid = true;
	} else if (!isspace(**endptr)) {
		invalid = true;
	} else {
		invalid = false;
	}

	return invalid;
}

static void default_callback (bool *invalid)
{
	if (*invalid) {
		return;
	}

	return;
}

static void callback (bool *invalid)
{
	if (*invalid) {
		return;
	}

	double sale = _number_ ;
	double cost = _cost_ ;
	if (sale <= cost) {
		printf("The sale value must be greater than the cost\n");
		*invalid = true;
	}

	return;
}

static void callback_cnt (bool *invalid)
{
	if (*invalid) {
		return;
	}

	if (floor(_number_) != ceil(_number_)) {
		printf("The shoe count must be an integral value\n");
		*invalid = true;
	}

	return;
}

static void validData (const char *fname,
		       const char *prompt,
		       void (*cb)(bool *invalid) = default_callback,
		       const char *msg = "Please input valid data")
{
	_number_ = 0;
	memset(*_temp_, 0, _sz_);
	printf("%s", prompt);
	ssize_t chars = 0;
	bool invalid = true;
	do {
		errno = 0;
		chars = getline(_temp_, &_sz_, stdin);
		if (chars == -1) {	// caters EOF
			if (errno) {
				fprintf(stderr, "%s: %s\n", fname, strerror(errno));
				cleanup();
				exit(EXIT_FAILURE);
			}
			clearerr(stdin);
			printf("\n%s\n", msg);
			printf("%s", prompt);
		} else if (chars > MAX_STRING_LEN) {

			invalid = true;
			char msg[] = "The input exceeds the max number of chars %d\n";
			printf(msg, MAX_STRING_LEN);
			*_temp_ = (char*) realloc(*_temp_, MAX_BUFFER_SIZE);
			if (!*_temp_) {
				fprintf(stderr, "validData: %s\n", strerror(errno));
				cleanup();
				exit(EXIT_FAILURE);
			}
			memset(*_temp_, 0, MAX_BUFFER_SIZE);
			_sz_ = MAX_BUFFER_SIZE;
			printf("%s", prompt);

		} else {

			if (!is_numeric(_temp_)) {
				invalid = true;
			} else {
				invalid = toNumber(_temp_);
			}

			if (_number_ < 0) {
				invalid = true;
			}

			cb(&invalid);

			if (invalid) {
				printf("%s\n", msg);
				printf("%s", prompt);
			}
		}

	} while (chars == -1 || invalid);
}

void *Util_Free (void *p)
{
	if (!p) {
		return NULL;
	}

	m_chain_t *node = ((m_chain_t*) p) - 1;
	if (node->hash != HASH) {
		fprintf(stderr, "Util_Free: unregistered object error\n");
		return p;
	}

	size_t const size = node->size;
	node = Util_Remove(node);

	m_size -= size;
	--m_count;

	return NULL;
}

void Util_Clear (void)
{
	m_chain_t *next = NULL;
	for (m_chain_t *node = m_chain.next; node; node = next) {
		next = node->next;
		void *data = node->data;
		node = (m_chain_t*) Util_Free(data);
	}

	m_size = 0;
	m_count = 0;
}

void *Util_Malloc (size_t const sz)
{
	size_t const size = sizeof(m_chain_t) + sz;
	void *p = malloc(size);
	if (!p) {
		fprintf(stderr, "Util_Malloc: %s\n", strerror(errno));
		return NULL;
	}

	m_chain_t* node = (m_chain_t*) p;
	void *data = (node + 1);

	node = Util_Chain(node);
	node->data = data;
	node->hash = HASH;
	node->size = size;

	m_size += size;
	++m_count;

	return data;
}

char *Util_CopyString (const char *string)
{
	size_t const len = strlen(string);
	size_t const sz = (len + 1);
	void *ptr = Util_Malloc(sz);
	if (!ptr) {
		fprintf(stderr, "Util_CopyString: error\n");
		return NULL;
	}

	const char *src = string;
	char *dst = (char*) ptr;
	return strcpy(dst, src);
}

void init (void)
{
	size_t const sz = MAX_BUFFER_SIZE;
	*_temp_ = (char*) Util_Malloc(sz);
	if (!*_temp_) {
		fprintf(stderr, "init: %s\n", strerror(errno));
		cleanup();
		exit(EXIT_FAILURE);
	}

	*_code_ = (char*) Util_Malloc(sz);
	if (!*_code_) {
		fprintf(stderr, "init: %s\n", strerror(errno));
		cleanup();
		exit(EXIT_FAILURE);
	}

	*_info_ = (char*) Util_Malloc(sz);
	if (!*_info_) {
		fprintf(stderr, "init: %s\n", strerror(errno));
		cleanup();
		exit(EXIT_FAILURE);
	}

	_sz_ = sz;
}

void head (void)
{
	printf("SHOE SALES INVENTORY PROGRAM\n");
}

void gcode (void)
{
	size_t n = MAX_BUFFER_SIZE;
	bool invalid = true;
	ssize_t chars = 0;
	char *text = NULL;
	const char prompt[] = "Input the shoe reference code:";
	printf("%s", prompt);
	memset(*_code_, 0, MAX_BUFFER_SIZE);
	do {
		errno = 0;
		chars = getline(_code_, &n, stdin);
		if (chars == -1) {

			if (errno) {
				fprintf(stderr, "gcode: %s\n", strerror(errno));
				cleanup();
				exit(EXIT_FAILURE);
			}

			clearerr(stdin);
			printf("\nPlease input valid data\n");
			printf("%s", prompt);

		} else if (chars > MAX_STRING_LEN) {
			invalid = true;
			char msg[] = "The input exceeds the max number of chars %d\n";
			printf(msg, MAX_STRING_LEN);
			*_code_ = (char*) realloc(*_code_, MAX_BUFFER_SIZE);
			if (!*_code_) {
				fprintf(stderr, "gcode: %s\n", strerror(errno));
				cleanup();
				exit(EXIT_FAILURE);
			}
			memset(*_code_, 0, MAX_BUFFER_SIZE);
			n = MAX_BUFFER_SIZE;
			printf("%s", prompt);
		} else {
			text = *_code_ ;
			skipWhiteSpace(&text);
			if (!*text || *text == '\n') {
				invalid = true;
				printf("Please input a valid reference code\n");
				printf("%s", prompt);
			} else {
				invalid = false;
			}
		}

	} while (chars == -1 || invalid);

	char *code = *_code_ ;
	skipWhiteSpace(&code);
	nullTrailWhiteSpace(&code);
	memmove(*_code_, code, 1 + strlen(code));
}

void ginfo (void)
{
	size_t n = MAX_BUFFER_SIZE;
	bool invalid = true;
	ssize_t chars = 0;
	char *text = NULL;
	const char prompt[] = "Input the shoe description:";
	printf("%s", prompt);
	memset(*_info_, 0, MAX_BUFFER_SIZE);
	do {
		errno = 0;
		chars = getline(_info_, &n, stdin);
		if (chars == -1) {

			if (errno) {
				fprintf(stderr, "ginfo: %s\n", strerror(errno));
				cleanup();
				exit(EXIT_FAILURE);
			}

			clearerr(stdin);
			printf("\nPlease input valid data\n");
			printf("%s", prompt);

		} else if (chars > MAX_STRING_LEN) {
			invalid = true;
			char msg[] = "The input exceeds max number of chars %d\n";
			printf(msg, MAX_STRING_LEN);
			*_info_ = (char*) realloc(*_info_, MAX_BUFFER_SIZE);
			if (!*_info_) {
				fprintf(stderr, "ginfo: %s\n", strerror(errno));
				cleanup();
				exit(EXIT_FAILURE);
			}
			memset(*_info_, 0, MAX_BUFFER_SIZE);
			n = MAX_BUFFER_SIZE;
			printf("%s", prompt);
		} else {
			text = *_info_ ;
			skipWhiteSpace(&text);
			if (!*text || *text == '\n') {
				invalid = true;
				printf("Please input a valid description\n");
				printf("%s", prompt);
			} else {
				invalid = false;
			}
		}

	} while (chars == -1 || invalid);

	char *info = *_info_ ;
	skipWhiteSpace(&info);
	nullTrailWhiteSpace(&info);
	memmove(*_info_, info, 1 + strlen(info));
}

void gsize (void)
{
	char prompt[] = "Input the shoe size:";
	validData("gsize", prompt);
	_size_ = _number_ ;
}

void gavail (void)
{
	char *text = NULL;
	ssize_t chars = 0;
	bool invalid = true;
	memset(*_temp_, 0, _sz_);
	char prompt[] = "Input N/Y if the shoe is (un)available for sale:";
	printf("%s", prompt);
	do {
		errno = 0;
		chars = getline(_temp_, &_sz_, stdin);
		if (chars == -1) {

			if (errno) {
				fprintf(stderr, "gavail: %s\n", strerror(errno));
				cleanup();
				exit(EXIT_FAILURE);
			}

			clearerr(stdin);
			printf("\nPlease input N/Y\n");
			printf("%s", prompt);

		} else if (chars > MAX_STRING_LEN) {

			invalid = true;
			char msg[] = "The input exceeds the max number of chars %d\n";
			printf(msg, MAX_STRING_LEN);
			printf("Please input just N/Y\n");
			*_temp_ = (char*) realloc(*_temp_, MAX_BUFFER_SIZE);
			if (!*_temp_) {
				fprintf(stderr, "gavail: %s\n", strerror(errno));
				cleanup();
				exit(EXIT_FAILURE);
			}
			memset(*_temp_, 0, MAX_BUFFER_SIZE);
			_sz_ = MAX_BUFFER_SIZE;
			printf("%s", prompt);

		} else {

			text = *_temp_ ;
			skipWhiteSpace(&text);
			char const c = *text ;
			if (c == 'y' || c == 'Y' || c == 'n' || c == 'N'){
				invalid = false;
			} else {
				invalid = true;
			}

			if (invalid) {
				printf("Please input N/Y\n");
				printf("%s", prompt);
			}
		}

	} while (chars == -1 || invalid);

	_avail_ = *text;
	if (_avail_ == 'y') {
		_avail_ = 'Y';
	}

	if (_avail_ == 'n') {
		_avail_ = 'N';
	}
}

void gcost (void)
{
	char prompt[] = "Input the shoe cost:";
	char msg[] = "Please input a valid shoe cost value";
	validData("gcost", prompt, default_callback, msg);
	_cost_ = _number_ ;
}

void gsale (void)
{
	char prompt[] = "Input the shoe sale value:";
	void (*cb) (bool*) = callback;
	char msg[] = "Please input a valid shoe sale value";
	validData("gsale", prompt, cb, msg);
	_sale_ = _number_ ;
}

void gcount (void)
{
	char prompt[] = "Input the shoe count:";
	void (*cb) (bool*) = callback_cnt;
	char msg[] = "Please input a valid shoe count value";
	validData("gcount", prompt, cb, msg);
	_count_ = _number_ ;
}

void header (void)
{
	printf("THE SHOE INPUT DATA IS THE FOLLOWING\n\n");
}

void code (void)
{
	printf("REFERENCE: %s\n", *_code_);
}

void info (void)
{
	printf("DESCRIPTION: %s\n", *_info_);
}

void size (void)
{
	printf("SIZE: %.1f\n", _size_);
}

void avail (void)
{
	printf("AVAILABLE: %c\n", _avail_);
}

void cost (void)
{
	printf("COST: %.2f\n", _cost_);
}

void sale (void)
{
	printf("SALE: %.2f\n", _sale_);
}

void count (void)
{
	printf("COUNT: %.0f\n", _count_);
}

void total (void)
{
	double const cost = _cost_ ;
	double const sale = _sale_ ;
	double const units = _count_ ;
	double const total_cost = units * cost;
	double const total_sale = units * sale;
	printf("TOTAL COST: %.2f\n", total_cost);
	printf("TOTAL PROFIT OF %.0f UNITS: %.2f\n", units, total_sale);
}

void profit (void)
{
	double const cost = _cost_ ;
	double const sale = _sale_ ;
	double const units = _count_ ;
	double const profit = (sale - cost);
	double const total_cost = units * cost;
	double const net_profit = units * (sale - cost);
	printf("PROFIT PER UNIT: %.2f\n", profit);
	printf("NET PROFIT: %.2f\n", net_profit);
	printf("PROFIT PERCENTAGE: %.2f\n", (net_profit / total_cost) * 100);
}

void greet (void)
{
	printf("\nThank you for providing the information\n");
}

void cleanup (void)
{
	Util_Clear();
}

#if defined(__unix__) || defined(__linux__) || defined(__APPLE__)
void clear ()
{
        system("clear");
}
#elif defined(_WIN32) || defined(_WIN64)
void clear ()
{
        system("cls");
}
#else
void clear ()
{
	return;
}
#endif

#if defined(__unix__) || defined(__linux__) || defined(__APPLE__)
void pause ()
{
        return;
}
#elif defined(_WIN32) || defined(_WIN64)
void pause ()
{
        system("pause");
}
#else
void pause ()
{
	return;
}
#endif

/*

Inventory					February 13, 2024

source: Inventory.cpp
author: @misael-diaz

Copyright (C) 2024 Misael Díaz-Maldonado

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

References:

[0] https://en.cppreference.com/w/cpp
[1] https://www.man7.org/linux/man-pages/man3/system.3.html

*/

Kind::Kind (kind_t const kind) : kind(kind)
{
	return;
}

kind_t Kind::k () const
{
	return this->kind;
}

const char *Kind::stringify (const Kind *kind)
{
	kind_t const k = kind->k();
	switch(k) {
		case A:
			return "A";
		case B:
			return "B";
		default:
			return "C";
	}
}

kind_t Kind::enumerator (const char *kind)
{
	if (!strcmp(kind, "A")) {
		return A;
	}

	if (!strcmp(kind, "B")) {
		return B;
	}

	if (!strcmp(kind, "C")) {
		return C;
	}

	kind_t unknown = ((kind_t) 0xffffffff);
	return unknown;
}

void *Kind::operator new (size_t size)
{
	return Util_Malloc(size);
}

void Kind::operator delete (void *p)
{
	p = Util_Free(p);
}
