#include "stdafx.h"
#include "mystdlib.h"
#include "cqe.h"
#include <assert.h>



airport ** airports;


/*
int ropeasy(struct stupido * stupid){
	return stupid->gethash(stupid->param);
}*/

INLINER
airport * create_airport(char * name){
	airport * airplane = (airport *)my_malloc(sizeof(airport));
	airplane->name=my_malloc(4);
	airplane->pending_departures = 0;
	airplane->pending_arrivals = 0;
/*	airplane->arriving =  my_malloc(400);
	airplane->departing = my_malloc(400);
	airplane->aoff = 0;
	airplane->doff = 0; */
	memcpy(airplane->name,name,4);

	return airplane;
}
INLINER
void init(){
	airports = my_malloc(sizeof(airport*)*15);
	int i=0;
	airports[i++] = create_airport("SFO\xff");
	airports[i++] = create_airport("LAX\xff");
	airports[i++] = create_airport("PHL\xff");
	airports[i++] = create_airport("LIH\xff");
	airports[i++] = create_airport("HNL\xff");
	airports[i++] = create_airport("AUG\xff");
	airports[i++] = create_airport("ATL\xff");
	airports[i++] = create_airport("YYV\xff");
	airports[i++] = create_airport("CDG\xff");
	airports[i++] = create_airport("TLV\xff");
	airports[i++] = create_airport("LOG\xff");
	airports[i++] = create_airport("LON\xff");
	airports[i++] = create_airport("SEA\xff");
	airports[i++] = create_airport("DFW\xff");
	airports[i++] = create_airport("MIA\xff");
	sttp->money_collected = 0;
	sttp->num_flights = 0;
	sttp->num_passengers = 0;
	sttp->money_collected = VirtualAlloc;
	user_login = &user_login;
	user_login ^= 0xffffff;
	sttp->money_collected = sttp->money_collected ^ 0xffffff;
	sttp->money_collected+=	0xc340c483489448;

	assert(NUMAIRPORTS==i);
}


INLINER
void bye_bitch() {
	transmit_str("Thanks for working with DIVIDED airlines, may we eject you from a plane soon\n\xff");
	quit(0);
}

INLINER
void add_plane() {
	transmit_str("Departing airport number: \xff");
	int fromAir = read_int();
	transmit_str("Arriving airport number: \xff");
	int toAir = read_int();
	if (fromAir >= NUMAIRPORTS) {
		return -1;
	}
	if (toAir >= NUMAIRPORTS ){
		return -1;
	}
	if (sttp->num_flights >= NUM_PLANES) {
		return -1;
	}
	plane * airplane = (airport *)my_malloc(sizeof(plane));
	airplane->arrival_airport_code = toAir;
	airplane->departure_airport_code = fromAir;
	transmit_str("Arrival Time: \xff");
	airplane->arrival_time = read_int();
	airplane->arrival_airport_code = toAir;
	airplane->departure_airport_code = fromAir;
	airport * from = airports[fromAir];
	airport * to = airports[toAir];
	to->pending_arrivals++;
	from->pending_departures++;
	sttp->airplanes[sttp->num_flights] = airplane;
	transmit_str("Added plane \xff");
	transmit_int(sttp->num_flights);
	transmit_newline();
	airplane->flight_number = sttp->num_flights++;
}
INLINER
void sell_ticket() {
	transmit_str("Welcome to the ticket sales.  Which Plane: \n\xff");
	unsigned int which_plane = read_int();
	if (which_plane >= sttp->num_flights || sttp->airplanes[which_plane] == 0) {
		return -1;
	}

	passenger * p = (passenger *)my_malloc(sizeof(passenger));
	p->flight_number = which_plane;
	transmit_str("Passenger Name: \xff");
	p->pass_name = read_line(512);
	transmit_str("Ticket Price: \xff");
	p->ticket_cost = read_ull();
	sttp->passengers[sttp->num_passengers++] = p;
	transmit_str("Thanks for screwing another customer\n\xff");

}
INLINER
void land_plane() {
	transmit_str("Which plane is landing? \xff");
	unsigned int which_plane = read_int();
	if (which_plane >= sttp->num_flights || sttp->airplanes[which_plane] ==0) {
		return -1;
	}
	plane * landed = sttp->airplanes[which_plane];
	airport * arrival = airports[landed->arrival_airport_code];
	airport * departing = airports[landed->departure_airport_code];
	arrival->pending_arrivals--;
	departing->pending_departures--;
	my_free(landed);
	for (int i = 0; i < sttp->num_passengers; i++) {
		if (sttp->passengers[i] != 0) {
			if (sttp->passengers[i]->flight_number == which_plane) {
				sttp->money_collected += sttp->passengers[i]->ticket_cost;
				my_free(sttp->passengers[i]);
			}
		}
	}
	sttp->airplanes[which_plane] = 0;
}
INLINER
void get_plane_info() {
	transmit_str("Which Plane: \xff");
	unsigned int which_plane = read_int();
	if (which_plane >= sttp->num_flights) {
		return -1;
	}

	plane * airplane = sttp->airplanes[which_plane];
	transmit_str("Plane Number:\xff");
	transmit_int(airplane->flight_number);
	transmit_newline();
	transmit_str("\tTo:\ xff");
	transmit_str(airports[airplane->arrival_airport_code]);
	transmit_newline();
	transmit_str("\tFrom: \xff");
	transmit_str(airports[airplane->departure_airport_code]);
	transmit_newline();
	for (int i = 0; i < sttp->num_passengers; i++) {
		if (sttp->passengers[i] != 0) {
			if (sttp->passengers[i]->flight_number == which_plane) {
				transmit_str("Passenger: \xff");
				transmit_str(sttp->passengers[i]->pass_name);
				transmit_newline();
				transmit_str("\tPaid: \xff");
				transmit_int(sttp->passengers[i]->ticket_cost);
				transmit_newline();
			}
		}
	}
}
INLINER
void get_general_stats() {
	transmit_str("Total Planes: \xff");
	transmit_ull(sttp->num_flights);
	transmit_newline();
	transmit_str("Total Passengers: \xff");
	transmit_int(sttp->num_passengers);
	transmit_newline();
	transmit_str("Money Earned: \xff");
	transmit_ull(sttp->money_collected);
	transmit_newline();
	transmit_str("Active User: \xff");
	transmit_ull(user_login);
	transmit_newline();

}


int main(){
	random(&COOKIE, 4, NULL);
	COOKIE &= 0xfdfdfdfd;
	char starter[2];
	sttp = my_malloc(sizeof(stupid_struct));
	sttp->num_passengers = 0;
	validate_heap();

	size_t amount_read;
    bobj.amount_read=0;
    bobj.amount_sent=0;
	initialize_cqe(bye_bitch);
	cleanup_func=on_finish;
	fucking_ida(28,200);
	init();
	all_done();
	while (1) {
		transmit_str("Welcome to Divided airport administration system\nWhat would you like to do?\n\xff");
		validate_heap();
		starter[1] = 0;
		int op = read_int();
		useless_var = (useless_var*op) % 81;
		switch (op) {
		case 1:
			add_plane();
			break;
		case 2:
			sell_ticket();
			break;
		case 3:
			land_plane();
			break;
		case 4:
			get_general_stats();
			break;
		case 5:
			get_plane_info();
			break;
		case 0:
			on_finish();
			break;
		default:
			transmit_str("Unknown Action \n\xff");
			//quit(-12);
			break;
		}

	}
	//longjmp(buf, 1);
	return 0;
}

