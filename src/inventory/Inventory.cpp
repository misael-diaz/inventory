#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>

static size_t _sz_ = 0;		// size of temporary placeholder
static char *_temp_[] = {NULL};	// temporary placeholder for fetching the entire line
static char *_code_[] = {NULL};	// shoe reference code could be alpha numeric
static char *_info_[] = {NULL};	// shoe information might be a phrase
static int _size_ = 0;		// shoe size
static char _avail_ = 0;	// shoe availability (Y/N)
static int _cost_ = 0;		// shoe cost
static int _sale_ = 0;		// shoe sale value

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

void head (void)
{
	printf("SHOE SALES INVENTORY PROGRAM\n");
}

void gcode (void)
{
	size_t n = 0;
	printf("Input the shoe reference code:");
	ssize_t const chars = getline(_code_, &n, stdin);
	if (chars == -1) {
		fprintf(stderr, "gcode: %s\n", strerror(errno));
		cleanup();
		exit(EXIT_FAILURE);
	}
}

void ginfo (void)
{
	size_t n = 0;
	printf("Input the shoe description:");
	ssize_t const chars = getline(_info_, &n, stdin);
	if (chars == -1) {
		cleanup();
		fprintf(stderr, "ginfo: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void gsize (void)
{
	printf("Input the shoe size:");
	ssize_t const n = scanf("%d", &_size_);
	if (n != 1) {
		cleanup();
		fprintf(stderr, "gsize: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	ssize_t const chars = getline(_temp_, &_sz_, stdin);
	if (chars == -1) {
		cleanup();
		fprintf(stderr, "gsize: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void gavail (void)
{
	printf("Input the letter N/Y if the shoe is (un)available for sale:");
	ssize_t const n = scanf("%c", &_avail_);
	if (n != 1) {
		cleanup();
		fprintf(stderr, "gavail: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	ssize_t const chars = getline(_temp_, &_sz_, stdin);
	if (chars == -1) {
		cleanup();
		fprintf(stderr, "gavail: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void gcost (void)
{
	printf("Input the shoe cost:");
	ssize_t const n = scanf("%d", &_cost_);
	if (n != 1) {
		cleanup();
		fprintf(stderr, "gcost: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	ssize_t const chars = getline(_temp_, &_sz_, stdin);
	if (chars == -1) {
		cleanup();
		fprintf(stderr, "gcost: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void gsale (void)
{
	printf("Input the shoe sale value:");
	ssize_t const n = scanf("%d", &_sale_);
	if (n != 1) {
		cleanup();
		fprintf(stderr, "gsale: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	ssize_t const chars = getline(_temp_, &_sz_, stdin);
	if (chars == -1) {
		cleanup();
		fprintf(stderr, "gsale: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
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
	printf("SIZE: %d\n", _size_);
}

void avail (void)
{
	printf("AVAILABLE: %c\n", _avail_);
}

void cost (void)
{
	printf("COST: %d\n", _cost_);
}

void sale (void)
{
	printf("SALE: %d\n", _sale_);
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

Polygons				February 13, 2024

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
