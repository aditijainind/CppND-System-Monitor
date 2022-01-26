#include <string>
#include <sstream>
#include <iomanip>
#include "format.h"

using std::string;
using namespace std;
const int min_to_sec = 60;
// TODO(Done): Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
     string formattedValue=""; 
     long h = seconds / (min_to_sec*min_to_sec);
     seconds = seconds%(min_to_sec*min_to_sec);
     long m = seconds / min_to_sec;
     seconds = seconds % min_to_sec;
     std::stringstream ss;
     ss<<setfill('0')<<setw(2)<<h<<":"<<setw(2)<<m<<":"<<setw(2)<<seconds;
     ss>>formattedValue;
     return formattedValue;
}
