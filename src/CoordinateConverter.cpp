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
	response.setMime(MIME(Application, Json));
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
	Routes::Post(router, "/addcoordinates", Routes::bind(&CoordinateConverter::addCoordinates, this));
}

void CoordinateConverter::addCoordinates(const Rest::Request &request, Http::ResponseWriter response) {
	// Get coordinates received as json
	auto latlong = request.body();

	auto tJSON = json::parse(latlong);
	
	// Search for lat and long key:value pairs
	std::string latString;
	std::string longString;
	for (json::iterator it = tJSON.begin(); it != tJSON.end(); ++it) {
		std::cout << it.key() << " : " << it.value() << "\n";
		if (it.key() == "lat") {
			latString.assign(it.value());
		}
		else if (it.key() == "long") {
			longString.assign(it.value());
		}
	}

	// std::string ddLat = degreesDecimal(latString);
	// std::string ddLong = degreesDecimal(longString);

	std::cout << "Lat = " << latString << std::endl;
	std::cout << "Long = " << longString << std::endl;

	if (latString.empty() || longString.empty()) {
		response.send(Http::Code::Unprocessable_Entity);
	}
	else {
		// Build the response JSON object
		json respJSON;
		respJSON["lat"] = latString;
		respJSON["long"] = longString;

		std::cout << tJSON.dump() << std::endl;
		response.setMime(MIME(Application, Json));
		response.send(Http::Code::Ok, respJSON.dump());
	}
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
