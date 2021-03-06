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
	std::cout << "Initializing" << std::endl;
	httpEndpoint->init(opts);
	setupRoutes();
}

void CoordinateConverter::start() {
	std::cout << "Starting Coordinate Converter." << std::endl;
	httpEndpoint->setHandler(router.handler());
	httpEndpoint->serve();
}

void CoordinateConverter::shutdown() {
	std::cout << "Shuting down..." << std::endl;
	httpEndpoint->shutdown();
}

void CoordinateConverter::setupRoutes() {
	using namespace Rest;

	Routes::Get(router, "/status", Routes::bind(&handleStatus));
	Routes::Post(router, "/addcoordinates", Routes::bind(&CoordinateConverter::addCoordinates, this));
	Routes::Get(router, "/getcoordinates", Routes::bind(&CoordinateConverter::getCoordinates, this));
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

	if (ddLat.empty() || ddLong.empty()) {
		response.send(Http::Code::Unprocessable_Entity);
	}
	else {
		// Store coordinates
		storeCoordinates(ddLat, ddLong);

		// Build the response JSON object
		json respJSON;
		respJSON["lat"] = ddLat;
		respJSON["long"] = ddLong;

		std::cout << respJSON.dump() << std::endl;
		response.setMime(MIME(Application, Json));
		response.send(Http::Code::Ok, respJSON.dump());
	}
}

void CoordinateConverter::getCoordinates(const Rest::Request &request, Http::ResponseWriter response) {
	if (coorDB.empty()) {
		std::cout << "Database is empty, no coordinates to retrieve" << std::endl;
		response.send(Http::Code::No_Content);
	}
	else {
		std::string latlong = coorDB.back();


		std::cout << "Retrieved: " << latlong << std::endl;

		size_t pos = latlong.find(",");
		std::string ddLat = latlong.substr(0, pos);
		std::string ddLong = latlong.substr(pos+1);

		// Build the response JSON object
		json respJSON;
		respJSON["lat"] = ddLat;
		respJSON["long"] = ddLong;
		response.setMime(MIME(Application, Json));
		response.send(Http::Code::Ok, respJSON.dump());
	}
}

void CoordinateConverter::storeCoordinates(std::string pLat, std::string pLong) {
	// Build coordinate pair string
	std::string latlong;
	latlong.append(pLat).append(",").append(pLong);
	// Save to database
	std::cout << "Storing: " << latlong << std::endl;
	coorDB.push_back(latlong);
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
  	int endpos, cardinalCounter;
	unsigned int pos;
	// removes all white spaces
  	coordinate.erase(std::remove_if(coordinate.begin(),coordinate.end(),isspace),coordinate.end()); 
	
  	//search for cardinal direction letters
	sz = coordinate.find_first_of("NnEeWwSs");
	
  if(sz == std::string::npos){
    //return empty string of letters other than NnEeWwSs are found
    newFormat = "";
    return newFormat;
  }
  else{
    cardinalDirection = coordinate.substr(sz,1);
    coordinate.erase(sz,1);
    coordinate.append(cardinalDirection);
  }
  cardinalCounter = coordinate.length();

  if(cardinalCounter > 12){
    newFormat = "";
    return newFormat;
  }

  //Parse out string to find degrees minutes seconds
  	pos = coordinate.find("°");
    
  //string must constain degree symbol as minimum requirement else return empty string
    if(pos == std::string::npos) {
      newFormat = "";
      return newFormat;
    }
  	deg = coordinate.substr(0, pos); //copies everything before position
  	endpos = coordinate.find("\'");
  	min = coordinate.substr(pos+2,endpos-(pos+2));
    pos = endpos;
    endpos = coordinate.find("\"");
    sec = coordinate.substr(pos+1,endpos-(pos+1));
    
  //Converts to Degree Decimal
  if(coordinate.find("\"") != std::string::npos){
    //Convert to double
   	degrees = std::stod (deg,&sz);
  	minute = std::stod(min,&sz);
	second = std::stod(sec,&sz);
    DD = degrees + (minute/60) + (second/3600);
  }
  else{
    //Convert to double
    degrees = std::stod (deg,&sz);
  	minute = std::stod(min,&sz);
    DD = degrees + (minute/60);
  }

	newFormat = std::to_string(DD);
	
	//takes only the first letter
	cardinalDirection = cardinalDirection.substr(0,1);

	//Assigns + or - depending on cardinal direction
	if (cardinalDirection == "N" || cardinalDirection == "n") {
		newFormat.insert(0,cardDirPositive);
	}
	else if (cardinalDirection == "E" || cardinalDirection == "e"){
		newFormat.insert(0,cardDirPositive);
	}
	else if (cardinalDirection == "W" || cardinalDirection == "w" ){
		newFormat.insert(0,cardDirNegative);
	}
	else if (cardinalDirection == "S" || cardinalDirection == "s"){
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
