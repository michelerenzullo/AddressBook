#include <cstring>
#include <string>
#include <algorithm>
#include "AddressBook.h"

#define LASTNAME_MAX_LENGTH 30
#define NAME_MAX_LENGTH 20
#define NUM_MAX_LENGTH 12

void test_addentry(AddressBook& book) {

	char name[NAME_MAX_LENGTH + 1] = {};
	char lastname[LASTNAME_MAX_LENGTH + 1] = {};
	char num[NUM_MAX_LENGTH + 1] = {};
	int32_t r;

	printf("\n\n** TEST - ADD ENTRY **\n");
	do {
		printf("Name: ");
		r = scanf("%20s", name);
	} while (
		!std::all_of(name, name + strlen(name), [](unsigned char c) { return ::isalpha(c); }));

	do {
		printf("Lastname( \"/\" if none): ");
		r = scanf("%30s", lastname);
	} while (*lastname != '/' &&
		!std::all_of(lastname, lastname + strlen(lastname), [](unsigned char c) { return ::isalpha(c); }));

	do {
		printf("Num( \"/\" if none): ");
		r = scanf("%12s", num);
	} while (*num != '/' &&
		!std::all_of(num, num + strlen(num), [](unsigned char c) { return ::isdigit(c); }));

	if (*lastname == '/') *lastname = '\0';
	if (*num == '/') *num = '\0';

	printf("Added with%s success", book.addentry(name, lastname, num) ? "" : "out" );

}


void test_removentry(AddressBook& book) {

	char name[NAME_MAX_LENGTH + 1] = {};
	char lastname[LASTNAME_MAX_LENGTH + 1] = {};
	char num[NUM_MAX_LENGTH + 1] = {};
	int32_t r;
	printf("\n\n** TEST - REMOVE ENTRIES, DUPLICATE AS WELL **\n");
	do {
		printf("Name: ");
		r = scanf("%20s", name);
	} while (
		!std::all_of(name, name + strlen(name), [](unsigned char c) { return ::isalpha(c); }));

	do {
		printf("Lastname( \"/\" if none): ");
		r = scanf("%30s", lastname);
	} while (*lastname != '/' &&
		!std::all_of(lastname, lastname + strlen(lastname), [](unsigned char c) { return ::isalpha(c); }));

	do {
		printf("Num( \"*\" for all or \"/\" if empty): ");
		r = scanf("%12s", num);
	} while (*num != '*'  && *num != '/' &&
		!std::all_of(num, num + strlen(num), [](unsigned char c) { return ::isdigit(c); }));

	if (*lastname == '/') *lastname = '\0';
	if (*num == '/') *num = '\0';

	printf("Removed with%s success\n", book.removentry(name, lastname, num) ? "" : "out");
}

void test_search(AddressBook& book) {

	//search for prefix, you can use both name and lastname together or just one for time
	//if we don't find, we ask the user if try case insensitive, in this case we create a new map with all chars lower case
	char name[NAME_MAX_LENGTH + 1] = {};
	char lastname[LASTNAME_MAX_LENGTH + 1] = {};
	char num[NUM_MAX_LENGTH + 1] = {};
	int32_t r;
	printf("\n\n** TEST - SEARCH FOR NAME AND/OR LASTNAME SENSITIVE OR INSENSITIVE **\n");
	printf("Name: ");
	r = scanf("%20s", name);
	printf("Lastname: ");
	r = scanf("%30s", lastname);
	if (*name == '/') *name = '\0';
	if (*lastname == '/') *lastname = '\0';

	size_t occurrences = 0;
	book.search(name, lastname, false, &occurrences);

	char answer;
	do {
		printf(!occurrences ? "No occurences found, try with case insensitive?(y or n): " : "try to find more entries with case insensitive(y or n): ");
		r = scanf(" %c", &answer);
	} while (answer != 'y' && answer != 'n');

	if (answer == 'y') book.search(name, lastname, (answer == 'y'), &occurrences);
}

void test_sort(AddressBook& book) {

	printf("\n\n** TEST - SORT **\n");
	char answer;
	int32_t r;
	do {
		printf("Alphabetically sort for name(1) or lastname(0): ");
		r = scanf(" %c", &answer);
	} while (answer != '0' && answer != '1');

	book.sort(answer == '1');

}

void tests() {
	
	AddressBook book("book.csv");

	test_sort(book);

	test_search(book);

	test_addentry(book);

	test_removentry(book);

}

int main(void) {

	tests();

	_CrtDumpMemoryLeaks();
	return false;

}