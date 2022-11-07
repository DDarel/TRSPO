#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <iomanip>
#include <ctime>
#include <chrono>

std::mutex mtx1;
std::mutex mtx2;

class NumberOne {
private:
	double one_var;
public:
	NumberOne() {
		one_var = 3.14;
	}
	NumberOne(double var) {
		one_var = var;
	}
	NumberOne(const NumberOne& var) {
		one_var = var.one_var;
	}
	double GetOne() {
		return one_var;
	}
	void SetOne(double var) {
		one_var = var;
	}
	void AddOne(double var) {
		mtx1.lock();
		one_var += var;
		mtx1.unlock();
	}
};

class NumberTwo {
private:
	double two_var;
public:
	NumberTwo() {
		two_var = 14.3;
	}
	NumberTwo(double var) {
		two_var = var;
	}
	NumberTwo(const NumberTwo& var) {
		two_var = var.two_var;
	}

	double GetTwo() {
		return two_var;
	}
	void SetTwo(double var) {
		two_var = var;
	}
	void AddTwo(double var) {
		//std::this_thread::sleep_for(std::chrono::nanoseconds(1));
		mtx2.lock();
		two_var += var;
		mtx2.unlock();
	}
};

void AddFUNC1(NumberOne* One, NumberTwo* Two) {
	for (int i = 0; i < std::rand() % 10000 + 10000; i++) {
		double random1 = std::rand() % 100 / static_cast<float>(10);
		One->AddOne(random1);
		double random2 = std::rand() % 100 / static_cast<float>(10);
		Two->AddTwo(random2);
	}
}

void AddFUNC2(NumberOne* One, NumberTwo* Two) {
	for (int i = 0; i < std::rand() % 10000 + 10000; i++) {
		double random2 = std::rand() % 100 / static_cast<float>(10);
		Two->AddTwo(random2);
		double random1 = std::rand() % 100 / static_cast<float>(10);
		One->AddOne(random1);
	}
}

int main() {

	std::srand(0);

	NumberOne firstclass;
	NumberTwo secondclass;

	clock_t start1 = clock();

	std::vector<std::thread> threadMass;
	int n = std::rand() % 10 + 10;

	for (int i = 0; i < n; i++) {
		if (i < n / 2) {
			threadMass.push_back(std::thread(AddFUNC1, &firstclass, &secondclass));
		}
		else {
			threadMass.push_back(std::thread(AddFUNC2, &firstclass, &secondclass));
		}
	}

	for (int i = 0; i < n; i++) {
		threadMass[i].join();
	}

	clock_t end1 = clock();
	double seconds1 = (double)(end1 - start1);

	std::cout << "Time: " << seconds1 << std::endl;

	std::cout << "In first: " << std::setprecision(10) << firstclass.GetOne() << " and in second:  " << std::setprecision(10) << secondclass.GetTwo() << std::endl;

	firstclass.SetOne(3.14);
	secondclass.SetTwo(14.3);

	clock_t start2 = clock();

	for (int i = 0; i < n; i++) {
		if (i < n / 2) {
			AddFUNC1(&firstclass, &secondclass);
		}
		else {
			AddFUNC2(&firstclass, &secondclass);
		}
	}

	clock_t end2 = clock();
	double seconds2 = (double)(end2 - start2);
	std::cout << "Time: " << seconds2 << std::endl;
	std::cout << "In  first: " << std::setprecision(10) << firstclass.GetOne() << " and in second:  " << std::setprecision(10) << secondclass.GetTwo() << std::endl;

}