#include <sstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sys/stat.h>

std::string getStringTime(long long time_, std::string format);
std::string getStringTime(long long time_, const char *format);

bool pathExists(const std::string path);
bool isDir(const std::string path);
