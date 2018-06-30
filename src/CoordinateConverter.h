/*
 * CoordinateConverter.h
 *
 *  Created on: Jun 29, 2018
 *      Author: Francisco
 */

#ifndef COORDINATECONVERTER_H_
#define COORDINATECONVERTER_H_

#include "json.hpp"
#include "pistache/endpoint.h"
#include "pistache/http.h"
#include "pistache/router.h"

namespace traxx {

using namespace Pistache;

class CoordinateConverter {
public:
	CoordinateConverter(Address addr);
	virtual ~CoordinateConverter();

	void init(size_t);
	void start();
	void shutdown();

private:
	void setupRoutes();
	std::shared_ptr<Http::Endpoint> httpEndpoint;
	Rest::Router router;
	

};

} /* namespace traxx */

#endif /* COORDINATECONVERTER_H_ */
