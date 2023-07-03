#include <stdio.h>
//#include "fuzzy_sys.h" 
//#include "chikuji.cpp"
#include <chrono>
#include "chikuji.h"


int main() {

	double** x = nullptr;
	double* y = nullptr;
	const int dim = 2;
	int size = build_data(x, y, dim);

	Fuzzysystem* s = nullptr;
	auto t_start = std::chrono::high_resolution_clock::now();
	chikuji(s, x, y, size, dim);
	auto t_end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> t_duration = t_end - t_start;
	printf("Time cost of chikuji is: %f (sec)\n", t_duration.count());

	const char* fn = "data_test_new_range_error.csv";
	if (s == nullptr)	printf("fuzzy system is null.\n");
	print_csv(fn, s, x, y, size, dim);

	//s->show_fsy();
	
	//Fuzzyset** a = new Fuzzyset * [1];
	//a[0] = new Fuzzyset(-0.01, 2.5, 5.01);
	//Fuzzyrule* r = new Fuzzyrule(1, a, 3.0);
	//s->add_rule(r);
	//s->show_fsy();

	double te = total_error(s, x, y, size);
	printf("TOTAL ERROR = %lf\n", te);


	return 0;
}
