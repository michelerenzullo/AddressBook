



# AddressBook 
AddressBook using multimap and list iterators


## For dev
Add the header "AddressBook.h" and when compiling pass the AddressBook.cpp file
Tested with VS2022 and G++, Win and Linux

    g++ main.cpp AddressBook.cpp -std=c++17 -O3 -o out.exe


## API

    AddressBook book(const char* filename);

call the constructor and if exists, parse the file containing our contacts:

    NAME,,\r\n
    NAME2,,NUM2,\r\n
    NAME3,LASTNAME3,\r\n

*LASTNAME and NUM (phone number) are optionals, end line can be both \r\n or \n*

Defined maximum 100.000 entries will be read.

private functions:
```c++
    std::string get_file_contents(const char*)
    void search_execute(const pair_string& prefix, bool insensitive = false, size_t* occurrences = nullptr);
    void database_insensitive_map();
```

public functions:
```c++
    bool addentry(const char* name, const char* lastname, const char* num = nullptr);
    bool removentry(const char* name, const char* lastname, const char* num = nullptr);
    void sort(bool byname = true);
    void search(const char* name, const char* lastname, bool insensitive = false, size_t* occurrences = nullptr);
    void CSVparser();
 ```
 
1) `bool r = addentry(name, lastname, number);`
- add the entry in the map
- and in the database_insensitive map if has been already created(void search() executed before)
- append the entry in the file or create it if it doesn't exist
return true if success, false if failed
*phone number is an optional argument*

2) `bool r = removentry(name, lastname, number);`
**It removes also duplicates! if you don't pass the phone number or you pass as phone number the char "*", delete all the records that match just the key**
- check if the key exists
- if phone number is *, erase all the entries that match the key and add them to a unordered_multiset "std::unordered_multiset\<std::string\> lines_toerase"
- rewrite the file line by line skipping the lines contained in lines_toerase
return true if success, false if failed

3) `void sort(bool byname);`
- if the map exists and it has been created with the same direction requested by the user, just print it (always show names in the 1st column and lastnames in the 2nd)
- if not create a copy with swapped key and print it
 
  
4) `search(name, lastname, insensitive = false/*(default)*/, &occurrences);`
**Search for name and/or lastname, partial or total, sensitive or insensitive!**
- it prepares for the type of search, if insensitive is true it will make lower case the prefix and it will call `void database_insensitive_map();`
-- create a new map where the pair of key is the lower case of the original one and as **value store the iterator that point to the entry in the original case of database** NOTE: in the header we **declare first database_insensitive** and **database after**, so when destructors will be called we are sure that database is release first and database_insensitive will follow otherwise the destructor of database will try to release a memory free already!
I originally saved as value the case "sensitive" pair {name, lastname} but it's a waste of memory since the iterator won't be invalidated when a new entry is inserted in the map.
- it calls `void search_execute({name_tosearch,lastname_tosearch},insensitive,&occurrences);`
-- iterate with lower_bound of prefix till reached the end of the map or the first non-match, and print if there is a match with both prefix ! 
-- if case insensitive, from the iterator (saved as value) we print the original case retrieving the first element --> pair {name,lastname} and the second element --> phone number.

The bool for the case is optional, it's sensitive by default, and the occurrences is optional as well.

5. void `CSVparser();`
- if maps are not empty `.clear()` them (new file passed)
 - call `std::string get_file_contents(file_name.c_str())`
 Return a string of the file.
 *TODO: add a flag "buffermode", if we pass already a buffer we don't need to read the file, or if false we read it, I did on this C project github [CUBEParser](https://github.com/michelerenzullo/CUBEparser)*
 - set delimiters for strsep, on Win ",\r\n" on Linux ",\n"
 - ignore last "\r\n" or on Linux last "\n"
 - strsep to extract the token and with a counter check if is the 1st, 2nd or 3rd value then copy the first 2 values to our key `std::pair<std::string,std::string>`, the last one(phone number) will be the value of our map entry and insert it if `map.size() <= ENTRIES_MAX`.
 - It's not the most elegant solution, very "C-style" but seems ok, some libs could do better but for this "easy" problem will only increase the size and force the developer to link boost etc..
 
A few comments are inside the code.

## TODO: 
- Add optional buffer mode when calling CSVparser instead of filename
- ~~Potential improvement using [parallel hashmap](https://github.com/greg7mdp/parallel-hashmap) derived from Google Abseil library, for example when re-sorting we do .insert() for each key sequentially, we could parallelize, or when creating the case insensitive map etc...~~ 
using **for_each** and **std::execution::par** from PPL in VS showed that the costs of the parallelization is not justified by the increase of performance due to the simplicity of the operation .insert() or .compare() operation when searching for prefix
- Improve keyboard input parsing in tests


## FURTHER NOTES:
C++ 17 allows structured binding, it will make the code more readable rather than nested it.first.first ..
```c++
    for (const auto& [key, value] : database)
        std::cout << key.first << ", " << key.second << " has value " << value << std::endl;
   ```

