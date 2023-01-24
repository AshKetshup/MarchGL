#include "main.h"

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


	MarchGL marchgl({ 1920,1080,3, "GPU" });

	if (!marchgl.launchSuccessful()) {
		printf("Couldn't start MarchGL\n");
		return -1;
	}


	unsigned int n = std::thread::hardware_concurrency();
	std::cout << n << " concurrent threads are supported.\n";

	marchgl.main();

	return 0;
}