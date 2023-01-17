
#ifndef FILESYS_H
#define FILESYS_H

#if defined(__linux__)
#include <unistd.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

#include <stdlib.h>
#include <string>
#include <libloaderapi.h>

namespace filesys {
	#if defined(_WIN32)
	inline size_t getExecutablePathName(char* pathName, size_t pathNameCapacity) {
		return GetModuleFileNameA(NULL, pathName, (DWORD) pathNameCapacity);
	}

	#elif defined(__linux__)
	inline size_t getExecutablePathName(char* pathName, size_t pathNameCapacity) {
		size_t pathNameSize = readlink("/proc/self/exe", pathName, pathNameCapacity - 1);
		pathName[pathNameSize] = '\0';
		return pathNameSize;
	}

	#elif defined(__APPLE__)
	inline size_t getExecutablePathName(char* pathName, size_t pathNameCapacity) {
		uint32_t pathNameSize = 0;

		_NSGetExecutablePath(NULL, &pathNameSize);

		if (pathNameSize > pathNameCapacity)
			pathNameSize = pathNameCapacity;

		if (!_NSGetExecutablePath(pathName, &pathNameSize)) {
			char real[PATH_MAX];

			if (realpath(pathName, real) != NULL) {
				pathNameSize = strlen(real);
				strncpy(pathName, real, pathNameSize);
			}

			return pathNameSize;
		}

		return 0;
	}

	#else
	#error provide your own implementation
	#endif

	// Wraps the getExecutablePathName() function in order to provide a simpler method
	inline std::string getAppPath(void) {
		char* path = (char*) malloc(256 * sizeof(char));
		getExecutablePathName(path, 256);
		std::string appPath(path);
		free(path);
		return appPath;
	}
}

#endif