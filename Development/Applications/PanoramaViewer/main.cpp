#include <cstdlib>
#include "PanoramaViewer.h"

#include <VrLib/Kernel.h>

extern "C"
{
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
}

int main(int argc, char* argv[])
{
	vrlib::Kernel* kernel = vrlib::Kernel::getInstance();
	vrlib::Application* application = new PanoramaViewer();
	av_register_all();
	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "--config") == 0)
		{
			i++;
			kernel->loadConfig(argv[i]);
		}
	}

	kernel->setApp(application);
	kernel->start();
	return 0;
}

