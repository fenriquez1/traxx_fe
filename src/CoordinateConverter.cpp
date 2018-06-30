/*
 * CoordinateConverter.cpp
 *
 *  Created on: Jun 29, 2018
 *      Author: Francisco
 */

#include "CoordinateConverter.h"

using json = nlohmann::json;

void handleStatus(const Pistache::Rest::Request&, 
	Pistache::Http::ResponseWriter response) {
    json tJSON;
	tJSON["status"] = "running";
	response.send(Pistache::Http::Code::Ok, tJSON.dump());
}

namespace traxx {

using namespace Pistache;

CoordinateConverter::CoordinateConverter(Address addr)
	: httpEndpoint(std::make_shared<Http::Endpoint>(addr)) {
	// TODO Auto-generated constructor stub
}

CoordinateConverter::~CoordinateConverter() {
	// TODO Auto-generated destructor stub
}


void CoordinateConverter::init(size_t thr) {
	auto opts = Http::Endpoint::options().threads(thr).flags(Tcp::Options::InstallSignalHandler);
	httpEndpoint->init(opts);
	setupRoutes();
}

void CoordinateConverter::start() {
	httpEndpoint->setHandler(router.handler());
	httpEndpoint->serve();
}

void CoordinateConverter::shutdown() {
	httpEndpoint->shutdown();
}

void CoordinateConverter::setupRoutes() {
	using namespace Rest;

	Routes::Get(router, "/status", Routes::bind(&handleStatus));

}

} /* namespace traxx */

int main(int argc, char *argv[]) {
	Pistache::Port port(5000);

	int thr = 1;

	Pistache::Address addr(Pistache::Ipv4::any(), port);

	traxx::CoordinateConverter converter(addr);

	converter.init(thr);
	converter.start();

	converter.shutdown();


	return 0;
}
