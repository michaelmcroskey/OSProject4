# Operating Systems Project 4

A project by [Michael McRoskey](http://michaelmcroskey.com/) (mmcrosk1) and [Maggie Thomann](http://maggiethomann.com/) (mthomann)

Project Overview
--------

- [Project Requirements](http://www3.nd.edu/~dthain/courses/cse30341/spring2017/project4/project4.html)

Operating Systems Project 4 is an SEO-Optimization tool that tracks how often specified keywords appear on certain websites by constantly fetching and parsing those websites via [libcurl](https://curl.haxx.se/libcurl/). The project utilizes multi-threading, mutex locks, and conditional variables to ensure thread-safety with global resources such as boolean values and queues. A user will edit configuration files, specify the terms to search and URLs to access, and then run `site-tester` to constantly output processed word count data into CSV files.

## Files
1. **`site-tester.cpp`**: Given a configuration file argument, it uses producer/consumer threads to fetch websites and parse fetched content to count the occurrences of defined queries in real time
2. **`Makefile`**: Running the command `make` in this directory will properly compile `site-tester.cpp`
3. **`ConfigFile.h`**: Loads in configuration parameters and sets defaults if necessary
4. **`LibCurl.h`**: Grabs a webpage via [libcurl](https://curl.haxx.se/libcurl/) and stores into a C++ string
5. **`Config.txt`**: Example configuration plain text file. Lists arguments for `site-tester` in the form `PARAMETER=VALUE`
6. **`Search.txt`**: Example search terms plain text file with each term on its own line
7. **`Sites.txt`**: Example sites plain text file with each `http://`-prefixed URL on its own line
8. **`README.md`**: Describes how to build, run, and configure code
9. **`EC1.txt`**: Extra credit description
10. **`html/`**
⋅⋅11. **`1.html`**: Webpage to view results of `1.csv`. Similarly, `n.html` where `n` is an integer will show the results of `n.csv`
⋅⋅12. **`append.txt`**: file with minified html that gets appended to output to form `1.html`, `2.html`, etc.
⋅⋅13. **`styles.css`**: CSS styles for html output
14. **`csv/`**
⋅⋅13. **`1.csv`**: first CSV file -- all generated CSV files will go here
16. **`images/`**
⋅⋅17. **`ec_screenshot.png`**: screenshot of extra credit at [http://localhost:8000/html/1.html]()

<img src="images/ec_screenshot.png" width="500">

## System Requirements
System should have a `g++` compiler installed and be able to compile with the following flags:
- `-g`
- `-Wall` for errors
- `-std=gnu++11` for C++11
- `-lpthread` for threading
- `-lcurl` for [libcurl](https://curl.haxx.se/libcurl/) library 

## Usage
1. Edit `Config.txt`, `Search.txt`, and/or `Sites.txt` accordingly to configure options for number of threads, fetch period, URLs to parse, and search terms. See File Requirements below.
2. Run `$ make` to build the executables.
3. Run `$ site-tester Config.txt` to begin fetching/parsing URLs. The program will display to stdout each thread's actions.
4. After the first period, `site-tester` will output `1.csv`, `2.csv`, and so on in the current directory. You can view the word counts for various sites.
5. To end the run, `CTRL-C` in the command line.
6. Run `$ make clean` to delete `*.csv` files and executables.

### Extra Credit
1. Run steps 1-5 above.
2. `$ cd root_project_directory`
3. `$ python -m SimpleHTTPServer`
4. Navigate to [http://localhost:8000/html/1.html]() in a web browser and replace `1.html` with any html that's been generated.

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

## What's Going On

Below is a pseudocode version of the `main()` function in `site-tester.cpp`

```python
int main():

	catch_signals()
	initialize_config_parameters()
	config.display()
	
	num = 1
	# Every period
	while(1):
		
		fetch_queue.lock()
		fetch_queue.push(urls[])
		fetch_queue.unlock()
		
		# Fetch Threads
		for thread in num_fetch_threads:
			create_thread(fetch())
			
		stop_fetching(); # use condition variable to stop fetching
					
		for thread in num_fetch_threads:
			join_thread()
			
		delete fetch_threads
		
		search_terms[] = load_search_terms()
		
		# Parse Threads
		for thread in num_parse_threads:
			create_thread(parse())
			
		stop_parsing(); # use condition variable to stop fetching
					
		for thread in num_parse_threads:
			join_thread()
			
		delete parse_threads
		
		output_to_file(to_string(num++) + ".csv");
		
		sleep_this_thread(config.period);
```

The `fetch()` and `parse()` functions each make use of `unique_lock<mutex>` to lock and unlock queues and two `condition_variable` variables to alert the other theads when they can attempt to acquire the mutex again.

**Rubric Qualifications**

| Feature                        | Description                                                             |
|--------------------------------|-------------------------------------------------------------------------|
| Coding style/Formatting        | Good! (at least this README is well documented!)                        |
| Correct config parsing         | Yes (gives warning for unknown params, sets defaults)                   |
| Single-site testing            | Passes                                                                  |
| Multiple-site testing          | Passes                                                                  |
| Error-prone site testing       | Yes (`FOLLOWLOCATION` and `TIMEOUT_MS` curl flags specified)            |
| Graceful exit (SIGHUP/CTRL-C)  | Yes (will not exit while writing to file)                               |
| Config error protection        | Yes (exceptions for I/O errors, default values, thread limits)          |
| Single output file per fetch   | Yes (`1.csv`, `2.csv`, ...)                                             |
| Thread variations work         | Yes (default # threads, works with 1-8 threads for parse/fetch threads) |
| Multi-thread, multi-site, multi-search works   | Yes (tested with 5 sites, 5 search terms, 3 fetch threads, 2 parse threads) |
| Use of condition variables     | Yes (`fetch_cv` and `parse_cv`)                                         |
| Use of threading               | Yes (used `<std::thread>`)                                              |
