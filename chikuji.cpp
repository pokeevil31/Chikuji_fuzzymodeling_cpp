#include <cstdio>
#include <algorithm>
#include <stdlib.h>
#include <cfloat>
#include <cmath>
#include "chikuji.h"
//#include "fuzzy_sys.cpp"

using std::pow;

// build x (and y)
int build_data(double** &x, double* &y, int dim) {
	double x_min, x_max;
	int each_size = 100;
	each_size += 1;
	int size = (int)pow(each_size, dim);
	printf("size = %d\n", size);
	
	x_min = -10.0;
	x_max = 10.0;
	double wx = x_max - x_min;

	if (x != nullptr || y != nullptr) {
		printf("x or y != nullptr \n");
	}
	
	int i, j;
	x = new double*[size];
	int n = 0;
	for (i = 0; i < size; i++) {
		x[i] = new double[dim];
		for (j = 0; j < dim; j++) {
			n = i / (int)pow(each_size,j) % each_size;
			x[i][j] = x_min + (double)n / (double)(each_size - 1) * wx;
		}
	}

	y = new double[size];
	data_func(x, y, size, dim);
	if (y == nullptr) {
		printf("y == nullptr\n");
	}

	/*
	// check the data
	for (i = 0; i < size; i++) {
		printf("%d\t", i);
		printf("[ ");
		for (j = 0; j < dim; j++) {
			printf("%2.3lf ", x[i][j]);
		}
		printf("] -> %2.3lf\n", y[i]);
	}
	*/

	return size;
}

// build y
void data_func(double** &x, double* &y, int size, int dim) {
	if (x == nullptr) {
		printf("ERROR: data x is empty.\n");
	}

	if (y == nullptr) {
		printf("ERROR: data y is empty.\n");
	}

	int i, j;
	for (i = 0; i < size; i++) {
		y[i] = 6.0;
	}
	
	if (dim == 1) {
		for (i = 0; i < size; i++) {
			//y[i] = x[i][0] + 1;
			//y[i] = std::sin(x[i][0]);
			y[i] = x[i][0] * x[i][0];
		}
	}
	else if (dim == 2) {
		for (i = 0; i < size; i++) {
			//y[i] = std::sin(x[i][0]);
			y[i] = x[i][0] * x[i][0] + x[i][1] * x[i][1];
		}
	}
	else {
		printf("dim didn't match\n");
	}
	return;
}

// error counting
double total_error(Fuzzysystem* &s, double** x, double* y, int size) {
	double te = 0;
	for (int i = 0; i < size; i++) {
		te += abs(s->fuzzyreasoning(x[i]) - y[i]);
	}
	return te;
}

double total_error_random(Fuzzysystem*& s, double** x, double* y) {
	return 0.0;
}

double rule_covered_error(Fuzzysystem*& s, double** x, double* y, int size, int rule_num) {
	// rule_num = -1 : using newest rule
	double te = 0;
	for (int i = 0; i < size; i++) {
		if (s->point_in_rule_i(x[i], rule_num)) {
			te += abs(s->fuzzyreasoning(x[i]) - y[i]);
		}
	}
	return te;
}

double rule_covered_error(Fuzzysystem*& s, double** x, double* y, int size, double** range, int dim) {
	double te = 0;
	int i = 0, j = 0;
	bool in_range = true;
	for (i = 0; i < size; i++) {
		in_range = true;
		for (j = 0; j < dim; j++) {
			if (x[i][j] <= range[j][0] || x[i][j] >= range[j][1]) {
				in_range = false;
				break;
			}
		}
		if (in_range) {
			te += abs(s->fuzzyreasoning(x[i]) - y[i]);
		}
	}
	return te;
}

void print_csv(const char* fn, Fuzzysystem*& s, double** x, double* y, int n, const int dim) {
	FILE* fp = NULL;
	// fn = "data_test.csv";
	errno_t error = fopen_s(&fp, fn, "w+");
		//fopen("data_test.csv", "w+");
	if (/*fp == NULL*/ error != 0){
		printf("Open/creat csv file failed.\n");
		return;
	}
	int i = 0, j = 0;
	for (i = 0; i < dim; i++) {
		fprintf(fp, "x%d,", i + 1);
	}
	fprintf(fp, "y,ToM\n");	// Training or Model data, 0:T, 1:M

	for (i = 0; i < n; i++) {
		fprintf(fp, "%lf", x[i][0]);
		for (j = 1; j < dim; j++) {
			fprintf(fp, ",%lf", x[i][j]);
		}
		fprintf(fp, ",%lf,0\n", y[i]);
	}

	for (i = 0; i < n; i++) {
		fprintf(fp, "%lf", x[i][0]);
		for (j = 1; j < dim; j++) {
			fprintf(fp, ",%lf", x[i][j]);
		}
		fprintf(fp, ",%lf,1\n", s->fuzzyreasoning(x[i]));
	}

	fclose(fp);
}

void delete_fsetpp(Fuzzyset** fset) {
	Fuzzyset** fpp;
	while (fset != nullptr) {
		fpp = fset;
		fpp++;
		delete* fset;
		fset = fpp;
	}
}


void chikuji(Fuzzysystem* &s, double** x, double* y, int n, const int dim) {
	printf("Start chikuji.\n");

	const int rn = 50;
	// n = len(x)
	printf("Data num : %d\n", n);
	//const int dim = 1;	// int dim = len(x[0])
	printf("Input dim : %d\n", dim);
	double e = 0;
	double te = 0;		// total error
	Fuzzyrule a;
	Fuzzyset** added_fset = nullptr;
	Fuzzyset** fset_tmp = nullptr;
	int i = 0, j = 0;
	int t = 100;		// search time

	// get y_max, y_min and width of y
	double y_max = DBL_MIN;
	double y_min = DBL_MAX;
	for (i = 0; i < n; i++) {
		if (y_max < y[i])
			y_max = y[i];
		if (y_min > y[i])
			y_min = y[i];
	}
	double yw = y_max - y_min;

	// get x_max, x_min and width of x
	double* x_max = new double[dim];
	double* x_min = new double[dim];
	// x_max = DBL_MIN; x_min = DBL_MAX; ########
	for (i = 0; i < dim; i++) {
		x_max[i] = DBL_MIN;
		x_min[i] = DBL_MAX;
		for (j = 0; j < n; j++) {
			if (x_max[i] < x[j][i])
				x_max[i] = x[j][i];
			if (x_min[i] > x[j][i])
				x_min[i] = x[j][i];
		}
	}
	// width of x
	//double xw[dim];
	double* xw = new double[dim];
	// xw = x_max - x_min #################
	for (i = 0; i < dim; i++) {
		xw[i] = x_max[i] - x_min[i];
	}
	printf("x: [%.4lf ~ %.4lf]\n", x_min[0], x_max[0]);
	printf("y: [%.4lf ~ %.4lf]\n", y_min, y_max);


	added_fset = new Fuzzyset*[dim];
	for (i = 0; i < dim; i++) {
		added_fset[i] = nullptr;
	}

	double kn = 0.0;		// new postcedent
	double ko = 0.0;			// postcedent
	double tmp = 0.0;
	double** error_range = new double* [dim];	// for couting error
	for (i = 0; i < dim; i++) {
		error_range[i] = new double[2];
	}
	bool in_range = true;

	// initail system
	if (s == nullptr) {
		s = new Fuzzysystem();
		Fuzzyrule* rtmp = new Fuzzyrule();
		for (i = 0; i < dim; i++) {
			error_range[i][0] = x_min[i] - 1;
			error_range[i][1] = x_max[i] + 1;
			added_fset[i] = new Fuzzyset(x_min[i] - 1, (x_max[i] + x_min[i]) / 2, x_max[i] + 1);
		}
		te = DBL_MAX;
		kn = 0.0;			// new postcedent
		rtmp->set_rule(dim, added_fset, ko);
		s->add_rule(rtmp);
		printf("loop i from 0 to t = %d\n", t);
		for (i = 0; i <= t; i++) {
			//printf("\n=====1:check point[%d]=====\n", i);
			ko = (y_min - yw) + i * 3 * yw / t;
			tmp = 0.0;
			//s->set_rule_i(0, dim, added_fset, ko);
			s->set_rule_i_ko(0, dim, ko);
			tmp = total_error(s, x, y, n);
			//printf("ko[%3d] = %lf, te = %lf\n", i, ko, tmp);
			if (te > tmp) {
				te = tmp;
				kn = ko;
			}
		}
		//s->set_rule_i(0, dim, added_fset, kn);
		s->set_rule_i_ko(0, dim, kn);
		//s->show_fsy();
	}
	
	// annealing liked
	double nar = 0.5;	// total narrow rate (of original range), initial 0.5
	//nar = 0.1;											// test value!!!!!!!
	double narp = 0.98; // ???? narrow rate each, nar *= narp
	double nar_tmp = nar;
	//int rc = s->get_rn();
	//while (rc > 0 && nar > 0.01) {
	//	nar *= narp;
	//	rc--;
	//}
	int narrow_count = 0;			// narrow(anealing) count
	int acc = 0;

	i = 1;
	te = DBL_MAX;
	double te_before = te;			// total error before adding rule
	double point_error = 0.0;		// reasoning error (of a point)
	Fuzzyrule* nr = nullptr;
	double* cen = nullptr;
	double narrow_rate = 0.8;		// nar *= narrow_rate
	double impr = 0;				// current_error/error_before 
	double narrow_impr = 0;			// narrowo_error/error_before
	double expc_impr = 0.985;		// expected improvement
	int total_narrow_count = 0;
	double original_ko = 0.0;		// koken before narrow
	while (i < rn) {
		printf("Adding Rule [%d]...\n", i);
		//te_before = total_error(s, x, y, n);

		point_error = 0;
		cen = nullptr;
		// maybe can search fix point in total_error()???
		for (j = 0; j < n; j++) {
			tmp = abs(s->fuzzyreasoning(x[j]) - y[j]);
			if (point_error < tmp) {
				cen = x[j];			// get the center point to be fixed
				point_error = tmp;
			}
		}

		// new fuzzy set
		added_fset = new Fuzzyset * [dim];
		for (j = 0; j < dim; j++) {
			error_range[j][0] = cen[j] - xw[j] * nar;
			error_range[j][1] = cen[j] + xw[j] * nar;
			added_fset[j] = new Fuzzyset(error_range[j][0], cen[j], error_range[j][1]);
		}

		te_before = rule_covered_error(s, x, y, n, error_range, dim);

		nr = new Fuzzyrule();
		nr->set_rule(dim, added_fset, 0.0);		// new rule
		s->add_rule(nr);
		
		te = DBL_MAX;
		// find new kokenbu (postcedent)
		for (j = 0; j <= t; j++) {
			ko = (y_min - yw) + j * 3 * yw / t;
			s->set_rule_i_ko(i, dim, ko);
			//tmp = total_error(s, x, y, n);			// using error with all data
			tmp = rule_covered_error(s, x, y, n, error_range, dim);	// using error with newest rule covered
			if (te > tmp) {
				kn = ko;
				te = tmp;
			}
		}
		s->set_rule_i_ko(i, dim, kn);

		// step narrow
		//impr = total_error(s, x, y, n) / te_before;	// ?????????
		impr = te / te_before;
		printf("\t[ improvement rate : %lf ]\n", impr);
		if (impr > expc_impr && nar > 0.01) {
			narrow_count = 0;
			
			nar_tmp = nar;
			// back up the rule before narrow
			if (fset_tmp == nullptr) {
				fset_tmp = new Fuzzyset * [dim];
			}
			for (j = 0; j < dim; j++) {
				// fset_tmp: back up; added_fset: in the rule, will be narrowed
				fset_tmp[j] = new Fuzzyset(added_fset[j]);
			}
			original_ko = kn;

			while (narrow_count < 2) {
				//tmp *= narrow_rate;
				narrow_count++;
				printf("\tTrying to narrow (count:[%d])...\n", narrow_count);
				// set rule with narrowed antecedent
				nar_tmp *= narrow_rate;
				s->narrow_rule_i(i, narrow_rate);
				// search new koken
				te = DBL_MAX;
				for (j = 0; j <= t; j++) {
					ko = (y_min - yw) + j * 3 * yw / t;
					s->set_rule_i_ko(i, dim, ko);
					//tmp = total_error(s, x, y, n);			// using error with all data
					tmp = rule_covered_error(s, x, y, n, error_range, dim);	// using error with newest rule covered
					if (te > tmp) {
						kn = ko;
						te = tmp;
					}
				}
				s->set_rule_i_ko(i, dim, kn);

				//impr = total_error(s, x, y, n) / te_before;	// ?????????
				narrow_impr = rule_covered_error(s, x, y, n, error_range, dim) / te_before;
				if (narrow_impr < impr) {		// improved with narrow
					total_narrow_count++;
					printf("\t\t[%d] Improved with narrowing: ", total_narrow_count);
					if (narrow_impr <= expc_impr) {
						printf("good improvement!\n");
						nar = nar_tmp;
						for (j = 0; j < dim; j++) {
							delete fset_tmp[j];
						}
						delete[] fset_tmp;
						fset_tmp = nullptr;
						break;
					}
					else {		// still need to think about narrowing, back up
						printf("but still need to try...\n");
						original_ko = kn;
						impr = narrow_impr;
						for (j = 0; j < dim; j++) {
							fset_tmp[j]->copy_fset(added_fset[j]);
						}
					}
				}
				else {
					printf("\t\tDid not improved...\n");
				}
			}
			// if did not narrow, recall the original rule
			if (fset_tmp != nullptr) {
				printf("\tNarrowing did not get great improvement...\n");
				s->set_rule_i(i, dim, fset_tmp, original_ko);
				for (j = 0; j < dim; j++) {
					delete added_fset[j];
				}
				delete[] added_fset;
			}
		}
		

		i++;
	}

	delete[] xw;
	delete[] x_max;
	delete[] x_min;
}
