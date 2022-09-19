#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "LogHelper.h"

#include "FilterDll.h"

void tokenize(std::string const &str, const char delim,
            std::vector<std::string> &out)
{
    // construct a stream from the string
    std::stringstream ss(str);

    std::string s;
    while (std::getline(ss, s, delim)) {
		if (s.size() != 1) {
			s.erase(remove(s.begin(), s.end(), '\t'), s.end());
			out.push_back(s);
		}
    }
}

int main() {
	InitFitting();

	std::ifstream readFile("../rawdata.txt");
	if (readFile.is_open()) {
		while (!readFile.eof()) {
			char arr[256];
			readFile.getline(arr, 256);

			const char delim = ' ';
			std::vector<std::string> out;
			tokenize(std::string(arr), delim, out);

			if (out.size() == 3) {
				for (int i = 0; i < out.size(); i++) {
					out[i].erase(remove(out[i].begin(), out[i].end(), '\t'), out[i].end());
				}

				Filter_Result(std::stod(out[0]), std::stod(out[1]), std::stod(out[2]), false, 1003, 1004, 1005);
			}
		}
	}

	const _FilterResult& result = GetResultData();

	std::cout << __PRETTY_FUNCTION__ << " -> Result : "
		<< LOG_ARG_("index", result.index)
		<< LOG_ARG_("total_index", result.total_index)
		<< LOG_ARG("lowpass_index", result.lowpass_index)
		<< std::endl;
}