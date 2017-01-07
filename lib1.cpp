/**
 * PhantRest.cpp
 *
 *             .-.._
 *       __  /`     '.
 *    .-'  `/   (   a \
 *   /      (    \,_   \
 *  /|       '---` |\ =|
 * ` \    /__.-/  /  | |
 *    |  / / \ \  \   \_\  jgs
 *    |__|_|  |_|__\
 *    never   forget.
 *
 * Original Author: Todd Treece <todd@sparkfun.com>
 * Edited for Particle by: Jim Lindblom <jim@sparkfun.com>
 *
 * Copyright (c) 2014 SparkFun Electronics.
 * Licensed under the GPL v3 license.
 *
 */

#include "lib1.h"
#include <stdlib.h>

PhantRest::PhantRest(String host, String path, String port) {
  _host = host;
  _path = path;
  _port = port;
  _params = "\"beg\":\"beg\"";
}

void PhantRest::add(String field, String data) {

  _params += ",\"" + field + "\":\"" + data + "\"";

}


void PhantRest::add(String field, char data) {

  _params += ",\"" + field + "\":\"" + String(data) + "\"";

}


void PhantRest::add(String field, int data) {

  _params += ",\"" + field + "\":\"" + String(data) + "\"";

}


void PhantRest::add(String field, byte data) {

  _params +=",\"" + field + "\":\"" + String(data) + "\"";

}


void PhantRest::add(String field, long data) {

  _params += ",\"" + field + "\":\"" + String(data) + "\"";

}

void PhantRest::add(String field, unsigned int data) {

  _params += ",\"" + field + "\":\"" + String(data) + "\"";

}

void PhantRest::add(String field, unsigned long data) {

  _params += ",\"" + field + "\":\"" + String(data) + "\"";

}

void PhantRest::add(String field, double data, unsigned int precision) {

  String sd(data, precision);
  _params += ",\"" + field + "\":\"" + sd + "\"";

}

void PhantRest::add(String field, float data, unsigned int precision) {

  String sf(data, precision);
  _params += ",\"" + field + "\":\"" + sf + "\"";

}

String PhantRest::queryString() {
  return String(_params);
}

String PhantRest::url() {
/*
  String result = "http://" + _host + "/input/" + _pub + ".txt";
  result += "?private_key=" + _prv + _params;

  _params = "";

  return result;
*/
return "na";
}

String PhantRest::get() {
/*
  String result = "GET /output/" + _pub + ".csv HTTP/1.1\n";
  result += "Host: " + _host + "\n";
  result += "Connection: close\n";

  return result;
*/
return "na";
}

String PhantRest::getJson(String params) {
  String result = "{" + params + "}";

  return result;
}

String PhantRest::post() {

        /*

POST /api/metrics/soil/ HTTP/1.1
Host: ec2-52-35-112-204.us-west-2.compute.amazonaws.com:8080
Authorization: Bearer 699eb655-6188-46a2-a97c-244b0ad3c210
Content-Type: application/json
Cache-Control: no-cache
Postman-Token: 4dcb118b-54b8-7931-1675-2cc3ee6e1a2b

{"battery":"21.3789"}



	String params = _params.substring(1);
	String result = "POST /input/" + _pub + ".txt HTTP/1.1\n";
	result += "Host: " + _host + "\n";
	result += "Phant-Private-Key: " + _prv + "\n";
	result += "Connection: close\n";
	result += "Content-Type: application/x-www-form-urlencoded\n";
	result += "Content-Length: " + String(params.length()) + "\n\n";
	result += params;

        */

    //String t = "{\"beg\": \"beg\", \"battery\":\"155555\",\"humidity\":\"88\",\"soilmoisture1\":\"45\",\"soilmoisture2\":\"58\",\"soilmoisture3\":\"25\"}";

	String params = _params.substring(1);
	String t = getJson(_params);
	
	
	//String t = "{\"battery\":\"155555\"}";
	String result = "POST /" + _path + " HTTP/1.1\n";
	result += "Host: " + _host + ":" + _port +  "\n";	
	result += "Content-Type: application/json\n";
	result += "Cache-Control: no-cache\n";
	result += "Content-Length: " + String(t.length()) + "\n\n";
	result += t;
	
	//result += getJson(_params);

	_params = "\"beg\":\"beg\"";
	return result;
}

String PhantRest::clear() {
/*
  String result = "DELETE /input/" + _pub + ".txt HTTP/1.1\n";
  result += "Host: " + _host + "\n";
  result += "Phant-Private-Key: " + _prv + "\n";
  result += "Connection: close\n";

  return result;
*/
return "na";
}


int PhantRest::particlePost()
{
/*
	String postString = post();

	TCPClient client;
	char response[512];
	int i = 0;
	int retVal = 0;

	if (client.connect(_host, 80)) // Connect to the server
	{
		// phant.post() will return a string formatted as an HTTP POST.
		// It'll include all of the field/data values we added before.
		// Use client.print() to send that string to the server.
		client.print(postString);
		//delay(1000);
		// Now we'll do some simple checking to see what (if any) response
		// the server gives us.
		int timeout = 1000;
		while (client.available() || (timeout-- > 0))
		{
			char c = client.read();
			//Serial.print(c);	// Print the response for debugging help.
			if (i < 512)
				response[i++] = c; // Add character to response string
			delay(1);
		}
		// Search the response string for "200 OK", if that's found the post
		// succeeded.
		if (strstr(response, "200 OK"))
		{
			retVal = 1;
		}
		else if (strstr(response, "400 Bad Request"))
		{	// "400 Bad Request" means the Phant POST was formatted incorrectly.
			// This most commonly ocurrs because a field is either missing,
			// duplicated, or misspelled.
			retVal = -1;
		}
		else
		{
			// Otherwise we got a response we weren't looking for.
			retVal = -2;
		}
	}
	else
	{	// If the connection failed:
		retVal = -3;
	}
	client.stop();	// Close the connection to server.
	return retVal;	// Return error (or success) code.
 */
return 1;   
}