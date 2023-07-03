#include <cstdio>
#include <algorithm>

#define POINT_LIM 3
#define ZENKEN_LIM 3
#define RULE_LIM 50

template<class T>
class List {
public:
	List() {}
	
	List(T*& new_node) {
		node = new_node;
	}

	void set_prev(List*& new_node) {
		prev = new_node;
	}

	void set_next(List*& new_node) {
		next = new_node;
	}

	void add_node(List*& new_node) {
		List* curr = this;
		while (next != NULL)
			curr = curr->next;
		new_node->set_prev(curr);
		new_node->set_next(curr->next);
		next->prev = new_node;
		next = new_node;
	}

	
private:
	T* node = NULL;
	List* next = NULL;
	List* prev = NULL;
};


class Point
{
public:
	Point() {
		printf("default Point\n");
	}

	~Point() {
		//printf("Free Point\n");
	}

	Point(double x, double g) {
		this->x = x;
		this->g = g;
	}

	void set_point(double x, double g) {
		this->x = x;
		this->g = g;
	}
	
	double px() {
		return x;
	}

	double pg() {
		return g;
	}

	void print_point() {
		//printf("(%.4lf, %.4lf)", x, g);
		printf("(%.4lf)", x);
	}

private:
	double x = 0.0;
	double g = 0.0;
};

class Fuzzyset 
{
	friend class Fuzzyrule;
	friend class Fuzzysystem;

public:
	//void add_point(double x, double g);
	//void triangle(double m1, double m2, double m3);
	//double grade(double x);

	Fuzzyset() {
		printf("default Fuzzyset\n");
	}

	~Fuzzyset() {
		//printf("Free Fuzzyset\n");
	}

	Fuzzyset(double m1, double m2, double m3) {
		if (m1 > m2 || m1 > m3 || m2 > m3) {
			printf("ERROR: Bad Triangle.\n");
		}
		triangle(m1, m2, m3);
	}

	Fuzzyset(Fuzzyset*& ori) {
		this->copy_fset(ori);
	}

	void add_point(double x, double g) {
		if (pointnum >= POINT_LIM) {
			printf("Point overflow\n");
			return;
		}
		p[pointnum] = new Point(x, g);
		pointnum += 1;
	}

	void triangle(double m1, double m2, double m3) {
		add_point(m1, 0.0); mi[0] = m1;
		add_point(m2, 1.0); mi[1] = m2;
		add_point(m3, 0.0); mi[2] = m3;
	}

	int* get_mi() {
		return this->mi;
	}

	void narrow_membership(double nar) {
		double cen = p[1]->px();
		double d = cen - p[0]->px();
		p[0]->set_point(cen - d * nar, p[0]->pg());
		p[2]->set_point(cen + d * nar, p[2]->pg());
	}

	void copy_fset(Fuzzyset*& ori) {
		for (int i = 0; i < pointnum; i++) {
			delete p[i];
		}
		pointnum = 0;
		int* m = ori->get_mi();
		this->triangle(m[0], m[1], m[2]);
	}

	double grade(double x) {
		int i = 0;
		double t = 0.0;
		while (1) {
			if (i >= pointnum)
				break;
			t = p[i]->px();
			if (x < p[i]->px()) {
				break;
			}
			i += 1;
		}
		if (i <= 0) {
			return 0.0;
		}
		if (i >= pointnum){
			return 0.0;
		}
				
		double ax = p[i - 1]->px();
		double ag = p[i - 1]->pg();
		double bx = p[i]->px();
		double bg = p[i]->pg();
		return (bg - ag) * (x - bx) / (bx - ax) + bg;
	}

	void print_tri() {
		//printf("(");
		putchar('(');
		int i = 0;
		while (pointnum) {
			printf("%lf", p[i]->px());
			i++;
			if (i >= pointnum)
				break;
			printf(", ");
		}
		putchar(')');
	}

	void print_points() {
		int i = 0;
		printf("points:[");
		while (i < POINT_LIM) {
			p[i]->print_point();
			i++;
		}
		printf("]\n");
	}

private:
	int pointnum = 0;
	Point* p[POINT_LIM];
	int mi[POINT_LIM];
};

class Fuzzyrule
{

	friend class Fuzzysystem;
	
public:
	//double get_koken();
	//void set_zenken(Fuzzyset *fset);
	//void set_koken(double koken);
	//double match(double *x);

	Fuzzyrule() {
		//printf("default Fuzzyrule\n");
		zenken_num = 0;
		for (int i = 0; i < ZENKEN_LIM; i++) {
			zenken[i] = nullptr;
		}
	}

	Fuzzyrule(int num, Fuzzyset** fset, double koken) {
		zenken_num = num;
		set_zenken(fset);
		set_koken(koken);
	}

	~Fuzzyrule() {
		//printf("Free Fuzzyrule\n");
	}

	double get_koken() {
		return koken;
	}

	void set_rule(int dim, Fuzzyset** fset, double koken) {
		zenken_num = dim;
		set_zenken(fset);
		set_koken(koken);
	}

	void set_zenken(Fuzzyset** fset) {
		for (int i = 0; i < zenken_num; i++) {
			zenken[i] = fset[i];
		}
	}

	void set_koken(double& koken) {
		this->koken = koken;
	}

	void narrow_zenken(double nar) {
		for (int i = 0; i < zenken_num; i++) {
			zenken[i]->narrow_membership(nar);
		}
	}

	double match(double* x) {
		double match = 1.0;
		double g = 0;
		for (int i = 0; i < zenken_num; i++) {
			g = zenken[i]->grade(x[i]);
			match = std::min(match, g);
		}
		return match;
	}

	void print_rule() {
		putchar('[');
		putchar('\n');
		int i = 0;
		while (zenken_num > 0) {
			//zenken[i]->print_tri();
			putchar('\t');
			zenken[i]->print_points();
			i++;
			if (i >= zenken_num)
				break;
			printf(" + ");
		}
		printf("] -> %lf\n", koken);
	}

private:
	int zenken_num = 0;					// = dimention number
	Fuzzyset* zenken[ZENKEN_LIM];		// need better way
	double koken = 0.0;
};

class Fuzzysystem 
{
public:
	//void add_rule(Fuzzyrule r);
	//double fuzzyreasoning(double *x, int num = -1, int div = 0);
	//void init_sys();
	//void copy_sys();
	//void write_file();
	//void read_file();
	
	Fuzzysystem(){
		printf("default Fuzzysystem+++++\n");
		for (int i = 0; i < RULE_LIM; i++) {
			fr[i] = nullptr;
		}
	}

	Fuzzysystem(Fuzzyrule*& ir) {
		for (int i = 0; i < RULE_LIM; i++) {
			fr[i] = nullptr;
		}
		add_rule(ir);
	}

	~Fuzzysystem() {
		//printf("Free Fuzzysystem+++++\n");
	}

	bool point_in_rule_i(double*& x, int i = -1) {
		if (i == -1) {
			i = rulenum - 1;
		}
		return (this->fr[i]->match(x) <= 0.0) ? true : false;
	}

	void add_rule(Fuzzyrule*& r) {
		if (rulenum >= RULE_LIM) {
			printf("Error : Fuzzy rules overflow.\n");
			return;
		}
		fr[rulenum] = r;
		rulenum += 1;
	}

	void delete_rule() {
		if (rulenum <= 0) {
			printf("Error : Fuzzy system empty.\n");
			return;
		}
		rulenum -= 1;
		printf("delete check: going to delete.\n");
		if (fr[rulenum] != nullptr) {
			printf("delete check: delete fr[rulenum]\n");
			delete fr[rulenum];
			fr[rulenum] = nullptr;
			printf("delete check: deleted.\n");
		}
		printf("delete check: function end.\n");
	}

	void set_rule_i(int i, int dim, Fuzzyset** fset, double koken) {
		if (i >= rulenum) {
			printf("Set rule[i] ERROR: i out of array.\n");
			return;
		}
		fr[i]->set_rule(dim, fset, koken);
	}

	void set_rule_i_ko(int i, int dim, double koken) {
		if (i >= rulenum) {
			printf("Set rule[i] postcedent ERROR: i=%d out of array.\n", i);
			return;
		}
		fr[i]->set_koken(koken);
	}

	void narrow_rule_i(int i, double nar) {
		fr[i]->narrow_zenken(nar);
	}

	double fuzzyreasoning(double* x, int num = -1) {
		if (num == -1)
			num = rulenum;
		double nu, de, e;
		nu = de = e = 0.0;
		for (int i = 0; i < num; i++) {
			//printf("R: %d:\t", i);
			e = fr[i]->match(x);
			//if (e <= 0)
			//	continue;
			nu += e * fr[i]->get_koken();
			de += e;
		}
		//printf("nu / de = %lf / %lf\n", nu, de);
		if (de == 0)
			return 0.0;

		return nu / de;
	}

	void show_fsy() {
		printf("Show Fuzzy System:\n");
		int i = 0;
		if (rulenum == 0) {
			printf("EMPTY FUZZY SYSTEM\n");
			return;
		}
		Fuzzyrule* frp = fr[i++];
		while (frp != NULL) {
			if (i >= RULE_LIM) {
				break;
			}
			printf("Rule%3d: ", i);
			frp->print_rule();
			frp = fr[i++];
		}
	}

	int get_rn() {
		return rulenum;
	}
	
	
private:
	int rulenum = 0;
	Fuzzyrule* fr[RULE_LIM];
	//Fuzzyrule fr[RULE_LIM];
	//List<Fuzzyrule>* p = new List<Fuzzyrule>;
};