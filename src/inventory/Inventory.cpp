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
	const char *stringify(const Kind *kind);
	kind_t enumerator(const char *kind);
	void *operator new(size_t size);
	void operator delete(void *p);
};

struct Item
{
	char *code = NULL;
	char *info = NULL;
	char *avail = NULL;
	double *size = NULL;
	double *cost = NULL;
	double *sale = NULL;
	double *count = NULL;
	Kind *kind = NULL;
	Item(char *code,
	     char *info,
	     char *avail,
	     double *size,
	     double *cost,
	     double *sale,
	     double *count,
	     Kind *kind);
	void log() const;
	void total() const;
	void profit() const;
	void *operator new(size_t size);
	void operator delete(void *p);
};

struct Stack
{
	void **_stack_ = NULL;
	void **_begin_ = NULL;
	void **_avail_ = NULL;
	void **_limit_ = NULL;
	size_t _allot_ = 8;
	size_t _size_ = 0;
	int init();
	void *copy() const;
	size_t bytes () const;
	int grow();
	Stack(void);
	size_t cap() const;
	size_t numel() const;
	int add(void *elem);
	void **begin();
	void **end();
	void *operator new(size_t size);
	void operator delete(void *p);
};

static m_chain_t _m_chain_ ;
static size_t _m_size_ = 0;
static size_t _m_count_ = 0;

static size_t _sz_ = 0;		// size of temporary placeholder
static char *_temp_[] = {NULL};	// temporary placeholder for fetching the entire line
static char *_code_[] = {NULL};	// shoe reference code could be alpha numeric
static char *_info_[] = {NULL};	// shoe information might be a phrase
static double _size_ = 0;	// shoe size
static char _avail_ = 0;	// shoe availability (Y/N)
static double _cost_ = 0;	// shoe cost
static double _profit_ = 0;	// shoe profit per unit
static double _sale_ = 0;	// shoe sale value
static double _number_ = 0;	// placeholder for real numbers
static double _count_ = 0;	// shoe count
static kind_t _kind_ = A;	// shoe kind
static bool _new_ = false;	// true/false (no) new shoe

void head(void);
// getters:
void get(void);
void gnew(void);
Item *gitem(void);
// loggers:
void log(void);
void greet(void);
// memory handling utilities:
void init(void);
void cleanup(void);
// console manipulators:
void clear(void);
void pause(void);
// post-processing:
void aggregate(Stack *stack);

int main ()
{
	head();
	init();
	Stack *stack = new Stack();
	if (!stack) {
		fprintf(stderr, "main: error\n");
		exit(EXIT_FAILURE);
	}

	do {
		get();
		Item *item = gitem();
		item->log();
		item->total();
		item->profit();
		stack->add(item);
		gnew();
	} while (_new_);
	aggregate(stack);
	greet();
	cleanup();
	pause();
	return EXIT_SUCCESS;
}

static m_chain_t *Util_Chain (m_chain_t *node)
{
	m_chain_t *next = (_m_chain_.next)? _m_chain_.next : NULL;
	if (next) {
		next->prev = node;
	}

	node->next = next;
	node->prev = &_m_chain_ ;
	_m_chain_.next = node;
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

	if (_number_ <= 0) {
		printf("The cost value must be greater than zero\n");
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

	_m_size_ -= size;
	--_m_count_ ;

	return NULL;
}

void Util_Clear (void)
{
	m_chain_t *next = NULL;
	for (m_chain_t *node = _m_chain_.next; node; node = next) {
		next = node->next;
		void *data = node->data;
		node = (m_chain_t*) Util_Free(data);
	}

	_m_size_ = 0;
	_m_count_ = 0;
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

	_m_size_ += size;
	++_m_count_ ;

	return data;
}

char *Util_CopyString (char *string)
{
	size_t const len = strlen(string);
	size_t const sz = (len + 1);
	void *ptr = Util_Malloc(sz);
	if (!ptr) {
		fprintf(stderr, "Util_CopyString: error\n");
		return NULL;
	}

	char *src = string;
	char *dst = (char*) ptr;
	return strcpy(dst, src);
}

double *Util_CopyNumber (double *num)
{
	double *ptr = (double*) Util_Malloc(sizeof(*num));
	if (!ptr) {
		fprintf(stderr, "Util_CopyNumber: error\n");
		return NULL;
	}

	*ptr = *num;
	return ptr;
}

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

Item::Item (char *code,
            char *info,
            char *avail,
            double *size,
            double *cost,
            double *sale,
            double *count,
            Kind *kind)
{
	this->code = code;
	this->info = info;
	this->avail = avail;
	this->size = size;
	this->cost = cost;
	this->sale = sale;
	this->count = count;
	this->kind = kind;
}

void Item::log () const
{
	printf("REFERENCE: %s\n", this->code);
	printf("DESCRIPTION: %s\n", this->info);
	printf("SIZE: %.1f\n", *this->size);
	printf("AVAILABLE: %s\n", this->avail);
	printf("COST: %.2f\n", *this->cost);
	printf("SALE: %.2f\n", *this->sale);
	printf("COUNT: %.0f\n", *this->count);
	printf("KIND: %s\n", this->kind->stringify(this->kind));
}

void Item::total () const
{
	double const cost = *this->cost;
	double const sale = *this->sale;
	double const units = *this->count;
	double const total_cost = units * cost;
	double const total_sale = units * sale;
	printf("TOTAL COST: %.2f\n", total_cost);
	printf("TOTAL PROFIT OF %.0f UNITS: %.2f\n", units, total_sale);
}

void Item::profit () const
{
	double const cost = *this->cost;
	double const sale = *this->sale;
	double const units = *this->count;
	double const profit = (sale - cost);
	double const total_cost = units * cost;
	double const net_profit = units * (sale - cost);
	printf("PROFIT PER UNIT: %.2f\n", profit);
	printf("NET PROFIT: %.2f\n", net_profit);
	printf("PROFIT PERCENTAGE: %.2f\n", (net_profit / total_cost) * 100);
}

void *Item::operator new (size_t size)
{
	return Util_Malloc(size);
}

void Item::operator delete (void *p)
{
	p = Util_Free(p);
}

static void stk_err_create ()
{
	fprintf(stderr, "Stack::create: error\n");
}

static void stk_err_init ()
{
	fprintf(stderr, "Stack::init: error\n");
}

static void stk_err_add ()
{
	fprintf(stderr, "Stack::add: error\n");
}

static void stk_err_copy ()
{
	fprintf(stderr, "Stack::copy: error\n");
}

static void stk_err_grow ()
{
	fprintf(stderr, "Stack::grow: error\n");
}

static void **stk_create (size_t const allot)
{
	size_t const limit = (allot + 1);
	size_t const size = limit * sizeof(void*);
	void *p = Util_Malloc(size);
	if (!p) {
		stk_err_create();
		return NULL;
	}

	memset(p, 0, size);
	return ((void**) p);
}

Stack::Stack (void)
{
	return;
}

size_t Stack::cap () const
{
	return (this->_limit_ - this->_begin_);
}

size_t Stack::numel () const
{
	return (this->_avail_ - this->_begin_);
}

size_t Stack::bytes () const
{
	return this->_size_;
}

void **Stack::begin ()
{
	return this->_begin_;
}

void **Stack::end ()
{
	return this->_avail_;
}

void *Stack::copy () const
{
	size_t const numel = this->numel();
	size_t const size = numel * sizeof(void*);
	void *dst = Util_Malloc(size);
	if (!dst) {
		stk_err_copy();
		return NULL;
	}

	const void *src = ((const void*) this->_stack_);
	memcpy(dst, src, size);
	return dst;
}

int Stack::grow ()
{
	int rc = 0;
	void **stack = NULL;
	size_t const numel = this->numel();
	size_t const size = numel * sizeof(void*);
	size_t const allot = 2 * numel;
	void *data = this->copy();
	if (!data) {
		goto err;
	}

	stack = stk_create(allot);
	if (!stack) {
		goto err;
	}

	memcpy(stack, data, size);
	data = Util_Free(data);

	this->_stack_ = stack;
	this->_begin_ = stack;
	this->_avail_ = stack + numel;
	this->_limit_ = stack + allot;
	this->_allot_ = allot;
	return rc;

err:
	rc = -1;
	stk_err_grow();
	return rc;
}

int Stack::init ()
{
	int rc = 0;
	this->_stack_ = stk_create(this->_allot_);
	if (!this->_stack_) {
		rc = -1;
		stk_err_init();
		return rc;
	}

	this->_begin_ = this->_stack_;
	this->_avail_ = this->_stack_;
	this->_limit_ = this->_stack_ + this->_allot_;
	return rc;
}

int Stack::add (void *elem)
{
	int rc = 0;
	if (!this->_stack_) {
		rc = this->init();
		if (rc != 0) {
			goto err;
		}
	}

	if (this->_avail_ == this->_limit_) {
		rc = this->grow();
		if (rc != 0) {
			goto err;
		}
	}

	*this->_avail_ = elem;
	++this->_avail_;
	this->_size_ += sizeof(void*);
	return rc;

err:
	stk_err_add();
	return rc;
}

void *Stack::operator new (size_t size)
{
	return Util_Malloc(size);
}

void Stack::operator delete (void *p)
{
	p = Util_Free(p);
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

void gnew (void)
{
	char *text = NULL;
	ssize_t chars = 0;
	bool invalid = true;
	memset(*_temp_, 0, _sz_);
	char prompt[] = "Input N/Y if there is (no) other new shoe to add:";
	printf("\n");
	printf("%s", prompt);
	do {
		errno = 0;
		chars = getline(_temp_, &_sz_, stdin);
		if (chars == -1) {

			if (errno) {
				fprintf(stderr, "gnew: %s\n", strerror(errno));
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
				fprintf(stderr, "gnew: %s\n", strerror(errno));
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

	char const c = *text;
	if (c == 'y' || c == 'Y') {
		_new_ = true;
	} else {
		_new_ = false;
	}

	printf("\n");
}

void gcost (void)
{
	char prompt[] = "Input the shoe cost:";
	char msg[] = "Please input a valid shoe cost value";
	validData("gcost", prompt, callback, msg);
	_cost_ = _number_ ;
}

void gprofit (void)
{
	switch (_kind_) {
		case A:
			_profit_ = 0.50;
			break;
		case B:
			_profit_ = 0.40;
			break;
		default:
			_profit_ = 0.30;
	}
}

void uprofit (void)
{
	if (_kind_ == A) {
		_profit_ = 0.50;
	} else if (_kind_ == B) {
		_profit_ = 0.40;
	} else {
		_profit_ = 0.30;
	}
}

void gsale (void)
{
	double const cost = _cost_ ;
	double const profit = _profit_ ;
	double const sale = cost * (profit + 1.0);
	_sale_ = sale;
}

void gcount (void)
{
	char prompt[] = "Input the shoe count:";
	void (*cb) (bool*) = callback_cnt;
	char msg[] = "Please input a valid shoe count value";
	validData("gcount", prompt, cb, msg);
	_count_ = _number_ ;
}

Item *gitem (void)
{
	char *code = Util_CopyString(*_code_);
	if (!code) {
		Util_Clear();
		fprintf(stderr, "gitem: error\n");
		exit(EXIT_FAILURE);
	}

	char *info = Util_CopyString(*_info_);
	if (!info) {
		Util_Clear();
		fprintf(stderr, "gitem: error\n");
		exit(EXIT_FAILURE);
	}

	memset(*_temp_, 0, MAX_BUFFER_SIZE);
	**_temp_ = _avail_ ;
	char *avail = Util_CopyString(*_temp_);
	if (!avail) {
		Util_Clear();
		fprintf(stderr, "gitem: error\n");
		exit(EXIT_FAILURE);
	}

	double *size = Util_CopyNumber(&_size_);
	if (!size) {
		Util_Clear();
		fprintf(stderr, "gitem: error\n");
		exit(EXIT_FAILURE);
	}

	double *cost = Util_CopyNumber(&_cost_);
	if (!cost) {
		Util_Clear();
		fprintf(stderr, "gitem: error\n");
		exit(EXIT_FAILURE);
	}

	double *sale = Util_CopyNumber(&_sale_);
	if (!sale) {
		Util_Clear();
		fprintf(stderr, "gitem: error\n");
		exit(EXIT_FAILURE);
	}

	double *count = Util_CopyNumber(&_count_);
	if (!count) {
		Util_Clear();
		fprintf(stderr, "gitem: error\n");
		exit(EXIT_FAILURE);
	}

	Kind *kind = new Kind(_kind_);
	if (!kind) {
		Util_Clear();
		fprintf(stderr, "gitem: error\n");
		exit(EXIT_FAILURE);
	}

	Item *item = new Item(code, info, avail, size, cost, sale, count, kind);
	if (!item) {
		Util_Clear();
		fprintf(stderr, "gitem: error\n");
		exit(EXIT_FAILURE);
	}

	return item;
}

void gkind (void)
{
	if (_cost_ > 60.0e3) {
		_kind_ = C;
	} else if (_cost_ > 30.0e3 && _cost_ <= 60.0e3) {
		_kind_ = B;
	} else {
		_kind_ = A;
	}
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

void kind (void)
{
	char k = ((kind_t) 0);
	switch (_kind_) {
		case A:
			k = 'A';
			break;
		case B:
			k = 'B';
			break;
		default:
			k = 'C';
	}

	printf("KIND: %c\n", k);
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

void aggregate (Stack *stack)
{
	double profit = 0;
	for (void **it = stack->begin(); it != stack->end(); ++it) {
		Item *item = (Item*) *it;
		double const units = *item->count;
		double const sale = *item->sale;
		double const cost = *item->cost;
		profit += units * (sale - cost);
	}

	printf("AGGREGATE PROFIT: %.2f\n", profit);
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

#if defined(SWITCH) && SWITCH
void get (void)
{
	gcode();
	ginfo();
	gsize();
	gavail();
	gcost();
	gkind();
	gprofit();
	gsale();
	gcount();
}
#else
void get (void)
{
	gcode();
	ginfo();
	gsize();
	gavail();
	gcost();
	gkind();
	uprofit();
	gsale();
	gcount();
}
#endif

void log (void)
{
	header();
	code();
	info();
	size();
	avail();
	cost();
	kind();
	sale();
	count();
	total();
	profit();
	greet();
}

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
