#include "marchgl.h"

#include <iostream>
#include <thread>
#include <string>


using namespace std;

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
const unsigned int THREAD_AMNT = thread::hardware_concurrency() / 2;
const char* RMODE = "CPU";

const char* HELP =
"MarchGL - Marching Cubes IsoSurfaces render"
""
"-W, --width   [int]\t\t\tSets width pixels. Default is 1280px"
"-H, --height  [int]\t\t\tSets height pixels. Default is 720px"
"-T, --threads [int]\t\t\tOverrides calculated ammount of threads"
""
"-h, --help         \t\t\tDisplays help and exits"
"";

int argHandler(int argc, const char* argv[], Arguments& args) {

	if (argc == 2 && ( strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0 )) {
		cout << HELP;

		return -1;
	}

	args.width = SCR_WIDTH;
	args.height = SCR_HEIGHT;
	args.threads = ( THREAD_AMNT <= 0 ) ? 1 : THREAD_AMNT;
	args.rMode = RMODE;

	if (( argc - 1 ) % 2 == 0) {
		unsigned int amount = ( argc - 1 ) / 2;
		for (size_t i = 0; i < amount; i++) {
			const char* tag = argv[1 + ( i * 2 )];
			const char* arg = argv[2 + ( i * 2 )];

			if (strcmp(tag, "-W") == 0 || strcmp(tag, "--width") == 0) {
				args.width = stoi(arg);
			}

			if (strcmp(tag, "-H") == 0 || strcmp(tag, "--height") == 0) {
				args.height = stoi(arg);
			}

			if (strcmp(tag, "-T") == 0 || strcmp(tag, "--threads") == 0) {
				args.threads = stoi(arg);

				if (args.threads > (unsigned int) thread::hardware_concurrency)
					throw invalid_argument("You requested more threads than it can be handled. Max possible threads: " + (unsigned int) thread::hardware_concurrency);

				if (args.threads > THREAD_AMNT)
					cout << "You're using more than " << THREAD_AMNT << " threads, which may degrade performance on your system." << endl;
			}
		}
	}
}

int main(int argc, const char* argv[]) {
	Arguments args;
	int returnValue;

	try {
		returnValue = argHandler(argc, argv, args);
		switch (returnValue) {
			case -1:
				return 0;
			default:
				break;
		}
	} catch (const exception& e) {
		cout << e.what() << endl;
		return -1;
	}


	MarchGL marchgl({1920,1080,3, "GPU" });

	if (!marchgl.launchSuccessful()) {
		printf("Couldn't start MarchGL\n");
		return -1;
	}
	string square = "0000";
	int case_n = std::stoi(square, 0, 2);

	printf("%d\n", case_n);

	marchgl.main();
	
	return 0;
}