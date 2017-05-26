#undef main
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>
#include "GameApp.h"

class AppTest : public ska::GameApp {
public:

	virtual int onTerminate (ska::TerminateProcessException&) { return 0; };
	virtual int onException(ska::GenericException&) { return 0; }

	virtual void run() {
		doctest::Context context;
		context.setOption("order-by", "name");            // sort the test cases by their name
		context.run();		
	};
};

ska::GameApp& ska::GameApp::get(){
	static AppTest app;
	return app;
}

