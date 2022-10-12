#ifndef ADDRESSBOOK_H
#define ADDRESSBOOK_H
#include <map>
#include <list>
#include <string>

#define ENTRIES_MAX 100000

typedef std::pair<std::string, std::string> pair_string;

class AddressBook {
private:
	//order is important, because the destructor will be called in the reverse order of declaration, it will be called first list, then database and finally database_insensitive, 
	//otherwise when delete database_insensitive the iterator will be deleted as well and will cause error
	std::multimap<pair_string, std::multimap<pair_string, std::list<std::string>::const_iterator>::const_iterator> database_insensitive;
	std::multimap<pair_string, std::list<std::string>::const_iterator> database_swappedKey;
	std::multimap<pair_string, std::list<std::string>::const_iterator> database;
	std::list<std::string> phones;
	std::string file_name;
	void search_execute(const pair_string& prefix, bool insensitive = false, size_t* occurrences = nullptr);
	std::string get_file_contents(const char*);
	char* mystrsep(char*&, const char*);
	bool fileExists(const char*);
public:
	bool addentry(const char* name, const char* lastname, const char* num = nullptr);
	bool removentry(const char* name, const char* lastname, const char* num = nullptr);
	void sort(bool byname = true);
	void search(const char* name, const char* lastname, bool insensitive = false, size_t* occurrences = nullptr);
	void CSVparser();
	explicit AddressBook(const char* filename) : file_name(filename ? filename : "") {
		CSVparser();
	}
	~AddressBook() {
		printf("Releasing memory\n");
	}
};
#endif