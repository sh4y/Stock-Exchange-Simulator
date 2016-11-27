#include <vector>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <streambuf>
#include <map>
#include "json.hpp"


using json = nlohmann::json;
using namespace::std;

vector<string> col = { "Date", "Open", "High", "Low", "Close", "Volume", "Adj. Close" };

vector<double> mult_vectors(vector<double> a, vector<double> b)
{
	if (a.size() != b.size()) {
		exit(1);
	}

	vector<double> res;
	for (size_t i = 0; i < a.size(); i++) {
		res.push_back(a[i] * b[i]);
	}

	return res;
}

double sum_vector(vector<double> v)
{
	double sum = 0;
	for (size_t i = 0; i < v.size(); i++) {
		sum += v[i];
	}
	return sum;

}

double mean_vector(vector<double> v)
{
	double sum = sum_vector(v);
	return sum / v.size();
}

double best_fit_slope(vector<double> x, vector<double> y)
{
	double num = (mean_vector(x) * mean_vector(y)) - mean_vector(mult_vectors(x, y));
	double dem = pow(mean_vector(x), 2) - mean_vector(mult_vectors(x, x));
	return num / dem;
}

double squared_error(vector<double> ys, vector<double> yo)
{
	if (ys.size() != yo.size())
		exit(1);
	vector<double> v;
	for (size_t i = 0; i < ys.size(); i++) {
		v.push_back(ys[i] - yo[i]);
	}
	double sum = sum_vector(mult_vectors(v,v));
	return sum;
}

double r_squared(vector<double> y, vector<double> ys)
{
	double mean = mean_vector(y);
	vector<double> v;
	for (size_t i = 0; i < y.size(); i++) {
		v.push_back(mean);
	}

	double squared_error_regr = squared_error(y, ys);
	double squared_error_mean = squared_error(y, v);
	return 1 - (squared_error_regr / squared_error_mean);
}

double best_fit_intercept(vector<double> x, vector<double> y, double m)
{
	return mean_vector(y) - (m*mean_vector(x));
}

string read_into_string(string file)
{
	std::ifstream ifs(file);
	std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	return str;
}

class StockDay {
public:
	string day;
	std::map<string, double> vals;
	/*	"Date",
		"Open",
		"High",
		"Low",
		"Close",
		"Volume",
		"Adjusted Close"*/
	void makePair(string s, double d);
	string returnRepr();
};

void StockDay::makePair(string s, double d)
{
	vals[s] = d;
}

string StockDay::returnRepr()
{
	string line;
	for (string x : col) {
		string str;
		if (x == "Date") {
			str = day;
		}
		else {
			std::ostringstream strs;
			strs << (vals[x]);
			str = strs.str();
		}
		line.append(x + ": " + str + " ");
	}
	return line;
}

vector<StockDay> create_stock_vectors(string file)
{
	json j = json::parse(read_into_string(file));
	string latest_date = j["dataset"]["newest_available_date"];
	vector<StockDay> temp;
	int len_data = end(j["dataset"]["data"]) - begin(j["dataset"]["data"]);
	for (int i = 0; i < len_data; i++) {
		string date = j["dataset"]["data"][i][0];
		auto data = j["dataset"]["data"][i];
		StockDay d;
		d.day = date;
		for (size_t j = 1; j < 7; j++) {
			d.makePair(col[j], data[j]);
			//cout << col[j] << "=> " << data[j];
		}
		temp.push_back(d);
	}
	return temp;
}

double perform_regression(vector<StockDay> v, string val, int n)
{
	vector<double> xs;
	vector<double> ys;
	for (int i = n; i > n-7; i--) {
		//cout << MS_days[i].returnRepr() << endl;
		xs.push_back(double(i));
		ys.push_back(v[i].vals[val]);
	}

	double m = best_fit_slope(xs, ys);
	double b = best_fit_intercept(xs, ys, m);
	return m;
}

double ten_day_low(vector<StockDay> v, int n)
{
	double init_low = v[n].vals["Adj. Close"];
	for (int i = 0; i < 9; i++) {
		if (v[n-i].vals["Adj. Close"] < init_low)
			init_low = v[n-i].vals["Adj. Close"];
	}
	return init_low;
}

double ten_day_high(vector<StockDay> v, int n)
{
	double init_high = v[n].vals["Adj. Close"];
	for (int i = 0; i < 9; i++) {
		if (v[n - i].vals["Adj. Close"] > init_high)
			init_high = v[n - i].vals["Adj. Close"];
	}
	return init_high;
}

double get_stop_price(double price, double pcent)
{
	return price - (price*pcent);
}

int main() {
	string line;
	vector<string> list = { "MS.txt", "DIS.txt" };
	/*
	for (int i = 5; i < 31; i += 5) {
		cout << i << " day regression of closing price." << endl;
		for (string x : list) {
			//cout << x << ": ";
			vector<StockDay> v = create_stock_vectors(x);
			cout << x << ": ";
			perform_regression(v, "Adj. Close", i);
		}
		cout << endl;
	}*/
	vector<StockDay> v = create_stock_vectors("MS.txt");
	double cash = 1000.00;
	double historical_low = 10000000.0, historical_high = 0;
	int stock_owned = 0;
	double buy_price, fees, cur_price;
	double stop_price;
	for (size_t i = 9; i < v.size(); i++) {

		cur_price = v[i].vals["Adj. Close"];
		fees = 7.95;
		if (cur_price < historical_low) {
			historical_low = cur_price;
			cout << "historical low" << endl;
		}
		else if (cur_price > historical_high) {
			historical_high = cur_price;
			cout << "historical high" << endl;
		}
		if (stock_owned == 0) {
			if (cur_price == ten_day_low(v, i) || ( cur_price <= historical_low && perform_regression(v, "Adj. Close", i) >= 0)) {
				//buying stock
				stock_owned += int(floor(cash / cur_price));
				buy_price = cur_price;
				stop_price = get_stop_price(buy_price, 0.05);
				cash = cash - (stock_owned * cur_price) - fees ;
				cout << "made trade, day " << i << " cash: " << cash << endl;
			}
			else if (cur_price >= historical_high && perform_regression(v, "Adj. Close", i) >= 0) {
				stock_owned += int(floor(cash / cur_price));
				buy_price = cur_price;
				stop_price = get_stop_price(buy_price, 0.05);
				cash = cash - (stock_owned * cur_price) - fees;
				cout << "made trade, day " << i << " cash: " << cash << endl;
			}
		}
		else {
			//selling stock
			if (cur_price == ten_day_high(v, i) || cur_price >= historical_high) {
				cash += (cur_price * stock_owned) - fees;
				stock_owned = 0;
				cout << "made trade, day " << i << " cash: " << cash << endl;
			}
			else if (cur_price <= historical_low) {
				cash += (cur_price * stock_owned) - fees;
				stock_owned = 0;
				cout << "made trade, day " << i << " cash: " << cash << endl;
			} 
			else if (cur_price <= stop_price && (perform_regression(v, "Adj. Close", i) < 0)) {
				cash += (cur_price * stock_owned) - fees;
				stock_owned = 0;
				cout << "made trade, day " << i << " cash: " << cash << endl;
			}
			else if (cur_price >= buy_price*1.10 && (perform_regression(v, "Adj. Close", i) < 0)) {
				cash += (cur_price * stock_owned) - fees;
				stock_owned = 0;
				cout << "made trade, day " << i << " cash: " << cash << endl;
			}

		}
	}
	cout << "Cash: " << cash;
	cin >> line;
	return 0;
}
