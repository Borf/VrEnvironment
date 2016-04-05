#include <cstdlib>
#include "ShadowMap.h"

#include <VrLib/Kernel.h>

int main(int argc, char* argv[])
{
	vrlib::Kernel* kernel = vrlib::Kernel::getInstance();
	ShadowMap* application = new ShadowMap();

	for( int i = 1; i < argc; ++i )
	{
		if(strcmp(argv[i], "--config") == 0)
		{
			i++;
			kernel->loadConfig(argv[i]); 
		}
	}

	kernel->setApp(application);
	kernel->start();
	return 0;
}

