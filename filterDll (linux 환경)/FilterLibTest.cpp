#include <vector>
#include <iostream>
#include "LogHelper.h"

#include "FilterDll.h"

int main() {
	InitFitting();
	const _FilterResult& result = GetResultData();

	std::cout << __PRETTY_FUNCTION__ << " -> Result : "
		<< LOG_ARG_("index", result.index)
		<< LOG_ARG_("total_index", result.total_index)
		<< LOG_ARG("lowpass_index", result.lowpass_index)
		<< std::endl;
}