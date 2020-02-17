#include "stdafx.h"
#include "controller.h"

int main(int argc, char* argv[])
{
	clientController controller(argc, argv);
	return controller.init();
}
