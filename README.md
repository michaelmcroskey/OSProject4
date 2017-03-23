# Operating Systems Project 4

A project by [Michael McRoskey](http://michaelmcroskey.com/) (mmcrosk1) and [Maggie Thomann](http://maggiethomann.com/) (mthomann)

-------------------

Project Overview
--------

- [Project Requirements](http://www3.nd.edu/~dthain/courses/cse30341/spring2017/project4/project4.html)
Operating Systems Project 4 

----
## Files
1. **`site-tester.cpp`**: Given a configuration file argument, it uses producer/consumer threads to fetch websites and parse fetched content to count the occurrences of defined queries in real time
2. **`Makefile`**: Running the command `make` in this directory will properly compile `site-tester.cpp`
3. **`ConfigFile.h`**: Loads in configuration parameters and sets defaults if necessary
4. **`LibCurl.h`**: Grabs a webpage via [libcurl](https://curl.haxx.se/libcurl/) and stores into a C++ string
5. **`Config.txt`**: Example configuration plain text file. Lists arguments for `site-tester` in the form PARAMETER=VALUE
6. **`Search.txt`**: Example search terms plain text file with each term on its own line
7. **`Sites.txt`**: Example sites plain text file with each http:// url on its own line
8. **`README.md`**: Describes how to build, run, and configure code

----
## System Requirements
System should have a `g++` compiler installed at `/usr/bin/g++` and be able to compile with the following flags:
- `-g`
- `-Wall` for errors
- `-std=gnu++11` for C++11
- `-lpthread` for threading
- `-lcurl` for [libcurl](https://curl.haxx.se/libcurl/) library 

----
## Usage
1. Edit `Config.txt`, `Search.txt`, and/or `Sites.txt` accordingly to configure options for number of threads, fetch period, urls to parse, and search terms. See File Requirements below.
1. Run `$ make` to build the executables.
2. Run `$ site-tester Config.txt` to begin fetching / parsing urls.
4. After the first period, `site-tester` will output `1.csv`, `2.csv`, and so on in the current directory. You can view the word counts for various sites
5. To end the run, `CTRL-C` in the command line.
5. Run `$ make clean` to delete `*.csv` files and executables.

----
## File Requirements

**`Config.txt`**
*See `Config.txt` for working example*
```
PERIOD_FETCH=<int::period>
NUM_FETCH=<int::number_of_fetch_threads>
NUM_PARSE=<int::number_of_parse_threads>
SEARCH_FILE=<string::search_terms_filename>
SITE_FILE=<string::sites_filename>
```

**`Search.txt`**
*See `Search.txt` for working example*
```
<string::search_term_1>
<string::search_term_2>
...
<string::search_term_n>
```

**`Sites.txt`**
*See `Sites.txt` for working example*
```
<string::url_to_fetch_1>
<string::url_to_fetch_2>
...
<string::url_to_fetch_3>
```

---
## What's Going On


README file that describes how to build, run, and configure your code.

A useful README file that describes how your system works and various parameters (5%)
Good coding style, including clear formatting, sensible variable names, and useful comments. (10%)
Correct parsing of various configuration parameters (10%)
Correct parsing / counting when tested with single error-free site (15%)
Correct parsing / counting when tested with multiple error-free sites (25%)
Correct operation when sites have errors (timeouts) (5%)
Correct exiting on SIGHUP or Control C (5%)
Protection against various configuration errors (5%)
Single output file per batch of fetches (5%)
Variations on thread settings correctly impact operation (5%)
Successful operation (multi-thread, multi-site, multi-search) over multiple batches of data (10%)