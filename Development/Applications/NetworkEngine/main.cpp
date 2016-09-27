#include <cstdlib>
#include "NetworkEngine.h"

#include <VrLib/Kernel.h>

int main(int argc, char* argv[])
{
	vrlib::Kernel* kernel = vrlib::Kernel::getInstance();
	NetworkEngine* application = new NetworkEngine();

	for( int i = 1; i < argc; ++i )
	{
		if (strcmp(argv[i], "--config") == 0)
		{
			i++;
			kernel->loadConfig(argv[i]);
		}
		if (strcmp(argv[i], "--key") == 0)
		{
			i++;
			application->tunnelKey = argv[i];
		}
	}

	kernel->setApp(application);
	kernel->start();
	return 0;
}

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}