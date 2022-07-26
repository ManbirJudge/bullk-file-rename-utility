#include "Utils.h"

std::string getStringTime(long long time_, std::string format) {
    time_t time {time_};
    auto localTime = *std::localtime(&time);

    std::stringstream ss;
    ss << std::put_time(&localTime, format.c_str());

    return ss.str();

}
std::string getStringTime(long long time_, const char *format ) {
    time_t time {time_};
    auto localTime = *std::localtime(&time);

    std::stringstream ss;
    ss << std::put_time(&localTime, format);

    return ss.str();

}

bool pathExists(const std::string path) {
	struct stat buffer;
	return (stat (path.c_str(), &buffer) == 0);
}
bool isDir(const std::string path) {
	struct stat buffer;
	stat(path.c_str(), &buffer);

	return buffer.st_mode & S_IFDIR;
}
