#pragma once
#include <stdio.h>
#include "fuzzy_sys.cpp"

int build_data(double** &x, double* &y, int dim);
void data_func(double**& x, double*& y, int size, int dim);
double total_error(Fuzzysystem* &s, double** x, double* y, int size);
double rule_covered_error(Fuzzysystem*& s, double** x, double* y, int size, int rule_num = -1);
double rule_covered_error(Fuzzysystem*& s, double** x, double* y, int size, double** range, int dim);
void print_csv(const char* fn, Fuzzysystem*& s, double** x, double* y, int n, const int dim);
void delete_fsetpp(Fuzzyset** fset);
void chikuji(Fuzzysystem* &s, double** x, double* y, int n, const int dim);
