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

	std::string ddLat = degreesDecimals(latString);
	std::string ddLong = degreesDecimals(longString);

	std::cout << "Lat = " << latString << std::endl;
	std::cout << "Long = " << longString << std::endl;

	if (latString.empty() || longString.empty()) {
		response.send(Http::Code::Unprocessable_Entity);
	}
	else {
		// Build the response JSON object
		json respJSON;
		respJSON["lat"] = ddLat;
		respJSON["long"] = ddLong;

		std::cout << respJSON.dump() << std::endl;
		response.setMime(MIME(Application, Json));
		response.send(Http::Code::Ok, respJSON.dump());
	}
}
std::string CoordinateConverter::degreesDecimals(std::string coordinate){
	
	//Declare variables
	const std::string cardDirPositive = "+";
 	const std::string cardDirNegative = "-";
  	std::string cardinalDirection;
  	std::string::size_type sz;
  	std::string newFormat;
  	std::string deg;
  	std::string min;
  	std::string sec;
   	double DD,degrees, minute, second = 0;
  	int pos, endpos;

	// removes all white spaces
  	coordinate.erase(std::remove_if(coordinate.begin(),coordinate.end(),isspace),coordinate.end()); 
	
  	pos = coordinate.find("°"); 

  	deg = coordinate.substr(0, pos); //copies everything before position

  	endpos = coordinate.find("\'");

  	min = coordinate.substr(pos+2,endpos-(pos+2));

	pos = endpos;
	endpos = coordinate.find("\"");
	sec = coordinate.substr(pos+1,endpos-(pos+1));


	//Convert to double
	degrees = std::stod (deg,&sz);
	minute = std::stod(min,&sz);
	second = std::stod(sec,&sz);

	
	std::cout << std::endl << degrees << "minutes" << minute << "seconds" << second;

	//Converts to Degree Decimal
	DD = degrees + (minute/60) + (second/3600);

	std::cout << std::endl << "Degree Decimal" << DD;

	newFormat = std::to_string(DD);

	std::cout << std::endl << newFormat;

	//search for cardinal direction letters
	sz = coordinate.find_first_of("NnEeWwSs");
	cardinalDirection = coordinate.substr(sz);
	//takes only the first letter
	cardinalDirection = cardinalDirection.substr(0,1);

	std::cout << "Cardinal Direction:" << cardinalDirection;

	//Assigns + or - depending on cardinal direction
	if (cardinalDirection == "N" || cardinalDirection == "n") {
		std::cout << std::endl << "at else if n or N";
		newFormat.insert(0,cardDirPositive);
	}
	else if (cardinalDirection == "E" || cardinalDirection == "e"){
		std::cout << std::endl << "at else if e or E";
		newFormat.insert(0,cardDirPositive);
	}
	else if (cardinalDirection == "W" || cardinalDirection == "w" ){
		std::cout << std::endl << "at else if w or W";
		newFormat.insert(0,cardDirNegative);
	}
	else if (cardinalDirection == "S" || cardinalDirection == "s"){
		std::cout << std::endl << "at else if s or S";
		newFormat.insert(0,cardDirNegative);
		
	}
	//Add Degree symbol at end of coordinate
	newFormat.append("°");

	return newFormat;

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
