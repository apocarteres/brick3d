#include "platform.hpp"
#include <string>

std::string ResourcePath(std::string fileName) {
	return "resources/" + fileName;
}
