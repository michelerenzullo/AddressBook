//#include <chrono>
#include <unordered_set>
#include <cstring>
#include <fstream>
#include <algorithm>
#include "dirent.h"
#include <sys/stat.h>
#include "AddressBook.h"


std::string AddressBook::get_file_contents(const char* filename) {
	FILE* fp = fopen(filename, "rb");
	if (fp) {
		std::string contents;
		fseek(fp, 0, SEEK_END);
		contents.resize(ftell(fp));
		rewind(fp);
		fread(&contents[0], 1, contents.size(), fp);
		fclose(fp);
		return contents;
	}
	return "";
}

bool AddressBook::fileExists(const char* filename)
{
	struct stat sb;
	if (stat(filename, &sb) == 0 && S_ISREG(sb.st_mode))
		return true;
	else
		return false;
}

char* AddressBook::mystrsep(char*& stringp, const char* delim)
{
	char* start = stringp;
	char* p;

	if ((p = (start != NULL) ? strpbrk(start, delim) : NULL) == NULL)
	{
		stringp = NULL;
	}
	else
	{
		*p = '\0';
		stringp = p + 1;
	}

	return start;
}

void AddressBook::search(const char* name, const char* lastname, bool insensitive, size_t* occurrences) {
	//preparation for type of search sensitive or insensitive
	pair_string prefix = { name ? name : "", lastname ? lastname : "" };

	if (insensitive) {
		std::transform(prefix.first.begin(), prefix.first.end(), prefix.first.begin(), ::tolower);
		std::transform(prefix.second.begin(), prefix.second.end(), prefix.second.begin(), ::tolower);
		if (database_insensitive.size() == 0) {

			for (std::multimap<pair_string, std::list<std::string>::const_iterator>::const_iterator it = database.begin(); it != database.end(); ++it) {
				pair_string key_insensitive = { it->first.first, it->first.second };

				std::transform(key_insensitive.first.begin(), key_insensitive.first.end(), key_insensitive.first.begin(), ::tolower);
				std::transform(key_insensitive.second.begin(), key_insensitive.second.end(), key_insensitive.second.begin(), ::tolower);

				database_insensitive.insert({ key_insensitive, it });
			}
		}
	}
	search_execute(prefix, insensitive, occurrences);
}

void AddressBook::search_execute(const pair_string& prefix, bool insensitive, size_t* occurrences) {
	if (occurrences) *occurrences = 0;

	//search for prefix from lower_bound of prefix till the end or the first non match with our prefix name
	if (insensitive)
		for (auto it = database_insensitive.lower_bound(prefix); it != database_insensitive.end() && it->first.first.compare(0, prefix.first.size(), prefix.first) == 0; ++it)
		{
			if (it->first.second.compare(0, prefix.second.size(), prefix.second) == 0) {
				if (occurrences) (*occurrences)++;
				printf("found: %s %s %s\n", it->second->first.first.c_str(), it->second->first.second.c_str(), (*it->second->second).c_str());
			}
		}
	else
		for (auto it = database.lower_bound(prefix); it != database.end() && it->first.first.compare(0, prefix.first.size(), prefix.first) == 0; ++it)
		{
			if (it->first.second.compare(0, prefix.second.size(), prefix.second) == 0) {
				if (occurrences) (*occurrences)++;
				printf("found: %s %s %s\n", it->first.first.c_str(), it->first.second.c_str(), (*it->second).c_str());
			}
		}
	printf("occurrences %d -- database.size() = %d - database_insensitive.size() = %d\n", (occurrences) ? (int)*occurrences : 0, (int)database.size(), (int)database_insensitive.size());
}

void AddressBook::CSVparser() {
	if (!fileExists(file_name.c_str())) return;

	std::string csv = get_file_contents(file_name.c_str());
	if (csv.length() < 2) return;

	const char delimiters[] =
#if defined __WIN32__ || defined _WIN32
		",\r";                    //carriage return on windows,we'll ignore the \n later
	csv[csv.length() - 2] = '\0';  //ignore last \r\n for strsep, avoid empty at the end
#else 
		",\n";
	csv[csv.length() - 1] = '\0';  //ignore last \n for strsep
#endif
	char* csv_ = &csv[0];

	if (database.size() != 0) database.clear();
	if (database_swappedKey.size() != 0) database_swappedKey.clear();
	if (database_insensitive.size() != 0) database_insensitive.clear();

	pair_string key;
	int32_t i = 0;
	//We parse the tuple in a map already sorted alphabetically for name
	while (char* token = mystrsep(csv_, delimiters))
	{
		switch (i++ % 3) {
		case 0:
#if defined __WIN32__ || defined _WIN32
			if (*token == '\n') ++token; //ignore \n after \r
#endif
			key.first = token;
			break;
		case 1:
			key.second = token;
			break;
		case 2:
			//push the phone number in front of the list as first element and get the iterator to it with .begin()
			phones.push_front(token);
			database.insert({ key, phones.begin() });
			if (database.size() > (ENTRIES_MAX - 1)) {
				printf("reached limit 100.000 entries, parsing stopped\n");
				csv_ = nullptr;
			}
			i = 0;
			break;
		}
	}
	printf("%d people in map\n", (int)database.size());

}

void AddressBook::sort(bool byname) {
	if (database.size() == 0) return;
	//if users wants a different sort and that map doesn't exist yet, create it swapping the key
	if (!byname && database_swappedKey.size() == 0)
		for (const auto& [key, list_it] : database)
			database_swappedKey.insert({ {key.second, key.first }, list_it });

	if (byname)
		for (const auto& [key, list_it] : database) printf("Name: \"%s\"  Lastname: \"%s\"  Num: \"%s\"\n",
			key.first.c_str(), key.second.c_str(), (*list_it).c_str());
	else
		for (const auto& [key, list_it] : database_swappedKey) printf("Name: \"%s\"  Lastname: \"%s\"  Num: \"%s\"\n",
			key.second.c_str(), key.first.c_str(), (*list_it).c_str());


}

bool AddressBook::addentry(const char* name, const char* lastname, const char* num) {
	if (!name) return false;

	std::string num_ = num ? num : "";
	pair_string key = { name ? name : "", lastname ? lastname : "" };

	phones.push_front(num_);
	std::multimap<pair_string, std::list<std::string>::const_iterator>::const_iterator it = database.insert({ key, phones.begin() });

	if (database_swappedKey.size() != 0) database_swappedKey.insert({ {key.second, key.first }, phones.begin() });

	//check if search has been called for the first time, if not don't add any entry to the insensitive map, it will be cloned later eventually if the user search.
	if (database_insensitive.size() != 0) {
		pair_string key_insensitive = key;

		std::transform(key_insensitive.first.begin(), key_insensitive.first.end(), key_insensitive.first.begin(), ::tolower);
		std::transform(key_insensitive.second.begin(), key_insensitive.second.end(), key_insensitive.second.begin(), ::tolower);

		database_insensitive.insert({ key_insensitive, it });
	}


	if (FILE* bookcsv = fopen(file_name.c_str(), "a")) {
		fprintf(bookcsv, "%s,%s,%s\n", name, lastname, num_.c_str());
		fclose(bookcsv);
	}
	else return false;

	return true;
}

bool AddressBook::removentry(const char* name, const char* lastname, const char* num) {
	if (!name) return false;
	if (database.size() == 0) return false;

	// * stands for all the phone numbers by default 
	std::string num_ = num ? num : "*";

	pair_string key = { name ? name : "", lastname ? lastname : "" };

	const auto& it = database.find(key);

	if (it != database.end()) {
		std::string line = key.first + "," + key.second + ",";
		std::unordered_multiset<std::string> lines_toerase;
		size_t erased = database.size();
		//if database_insensitive exists erase from both
		if (database_insensitive.size() != 0) {
			pair_string key_insensitive = key;
			std::transform(key_insensitive.first.begin(), key_insensitive.first.end(), key_insensitive.first.begin(), ::tolower);
			std::transform(key_insensitive.second.begin(), key_insensitive.second.end(), key_insensitive.second.begin(), ::tolower);

			//search for which entry has as value the key we are searching for

			const auto& range = database_insensitive.equal_range(key_insensitive);
			auto it = range.first;
			while (it != range.second) {
				if (it->second->first == key) {
					// if num has not been passed or is passed "*", extract all the records that match, add to a set for the delete from file and delete from the map
					// if num has been passed check if the value in the original multimap we
					//are pointing to with database_insensitive, is the same as phone_number, if yes erase both otherwise continue
					const auto& range2 = database.equal_range(key);
					auto it2 = range2.first;
					while (it2 != range2.second) {
						if (num_ == "*" xor *it2->second == num_) {
							lines_toerase.insert(line + *it2->second);
							it2 = database.erase(it2); //same as "database.erase(it2++);"
							it = database_insensitive.erase(it); //same as "database_insensitive.erase(it++);"
						}
						else {
							++it2;
							++it;
						}
					}
					//erase from the database with swappedKey if exists
					if (database_swappedKey.size() != 0) {
						const auto& range3 = database_swappedKey.equal_range({ key.second,key.first });
						auto it3 = range3.first;
						while (it3 != range3.second)
							(num_ == "*" xor *it3->second == num_) ? it3 = database_swappedKey.erase(it3) : ++it3;
					}
				}
				else ++it;
			}
		}
		// erase only inside multimap, if database_insensitive_map has not been created / called yet
		else {
			const auto& range = database.equal_range(key);
			auto it = range.first;
			while (it != range.second) {
				if (num_ == "*" xor *it->second == num_) {
					lines_toerase.insert(line + *it->second);
					it = database.erase(it);
				}
				else ++it;
			}
			if (database_swappedKey.size() != 0) {
				const auto& range2 = database_swappedKey.equal_range({ key.second,key.first });
				auto it2 = range2.first;
				while (it2 != range2.second)
					(num_ == "*" xor *it2->second == num_) ? it2 = database_swappedKey.erase(it2) : ++it2;
			}
		}
		erased -= database.size();
		printf("erased %d elements from map\n", (int)erased);
		if (erased) {
			printf("Erasing file...\n");
			const char* filename = file_name.c_str();
			std::ifstream in(filename);
			std::ofstream out("~$temp.csv");
			while (getline(in, line))
			{
				const auto& it = lines_toerase.find(line);
				if (it != lines_toerase.end())
					lines_toerase.erase(it);
				else
					out << line << "\n";
			}
			in.close();
			out.close();
			remove(filename);
			int32_t r = rename("~$temp.csv", filename);
			printf("lines_toerase.size() = %d - database.size() = %d - database_insensitive.size() = %d\n", (int)lines_toerase.size(), (int)database.size(), (int)database_insensitive.size());
		}
		else return false;
	}
	else return false;

	return true;
}
