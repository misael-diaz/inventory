#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cctype>

static size_t _sz_ = 0;		// size of temporary placeholder
static char *_temp_[] = {NULL};	// temporary placeholder for fetching the entire line
static char *_code_[] = {NULL};	// shoe reference code could be alpha numeric
static char *_info_[] = {NULL};	// shoe information might be a phrase
static double _size_ = 0;	// shoe size
static char _avail_ = 0;	// shoe availability (Y/N)
static double _cost_ = 0;	// shoe cost
static double _sale_ = 0;	// shoe sale value

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
// loggers:
void code(void);
void info(void);
void size(void);
void avail(void);
void cost(void);
void sale(void);
void greet(void);
// memory handling utility:
void cleanup(void);
// console manipulators:
void clear(void);
void pause(void);

int main ()
{
	head();

	gcode();
	ginfo();
	gsize();
	gavail();
	gcost();
	gsale();

	clear();

	header();
	code();
	info();
	size();
	avail();
	cost();
	sale();
	greet();

	cleanup();
	pause();
	return EXIT_SUCCESS;
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
	while (**txt && **txt <= ' ') {
		++*txt;
	}
}

static bool is_numeric (char **text)
{
	char *start = *text;
	if (!**text || **text == '\n') {
		return false;
	}

	skipWhiteSpace(text);
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

static bool toNumber (char **text, double *number)
{
	bool invalid = true;
	errno = 0;
	char *endptr[] = {NULL};
	*number = strtod(*text, endptr);
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

	if (_sale_ <= _cost_) {
		printf("The sale value must be greater than the cost\n");
		*invalid = true;
	}

	return;
}

static void validData (const char *fname,
		       const char *prompt,
		       double *number,
		       void (*cb)(bool *invalid) = default_callback,
		       const char *msg = "Please input valid data")
{
	*number = 0;
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
		} else {

			if (!is_numeric(_temp_)) {
				invalid = true;
			} else {
				invalid = toNumber(_temp_, number);
			}

			if (*number < 0) {
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

void head (void)
{
	printf("SHOE SALES INVENTORY PROGRAM\n");
}

void gcode (void)
{
	size_t n = 0;
	ssize_t chars = 0;
	const char prompt[] = "Input the shoe reference code:";
	printf("%s", prompt);
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
		}

	} while (chars == -1);
}

void ginfo (void)
{
	size_t n = 0;
	ssize_t chars = 0;
	const char prompt[] = "Input the shoe description:";
	printf("%s", prompt);
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
		}

	} while (chars == -1);
}

void gsize (void)
{
	char prompt[] = "Input the shoe size:";
	validData("gsize", prompt, &_size_);
}

void gavail (void)
{
	char *text = NULL;
	ssize_t chars = 0;
	bool invalid = true;
	char prompt[] = "Input N/Y if the shoe is (un)available for sale:";
	printf("%s", prompt);
	do {
		errno = 0;
		chars = getline(_temp_, &_sz_, stdin);
		if (chars == -1) {

			if (errno) {
				fprintf(stderr, "ginfo: %s\n", strerror(errno));
				cleanup();
				exit(EXIT_FAILURE);
			}

			clearerr(stdin);
			printf("\nPlease input N/Y\n");
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
	validData("gcost", prompt, &_cost_, default_callback, msg);
}

void gsale (void)
{
	char prompt[] = "Input the shoe sale value:";
	void (*cb) (bool*) = callback;
	char msg[] = "Please input a valid shoe sale value";
	validData("gsale", prompt, &_sale_, cb, msg);
}

void header (void)
{
	printf("THE SHOE INPUT DATA IS THE FOLLOWING\n\n");
}

void code (void)
{
	printf("REFERENCE: %s", *_code_);
}

void info (void)
{
	printf("DESCRIPTION: %s", *_info_);
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

void greet (void)
{
	printf("\nThank you for providing the information\n");
}

void cleanup (void)
{
	if (*_temp_) {
		free(*_temp_);
		*_temp_ = NULL;
	}

	if (*_code_) {
		free(*_code_);
		*_code_ = NULL;
	}

	if (*_info_) {
		free(*_info_);
		*_info_ = NULL;
	}
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
