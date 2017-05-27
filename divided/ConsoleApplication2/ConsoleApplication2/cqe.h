#pragma once
#include "myconsts.h"
#include "mystdlib.h"



typedef struct {
	unsigned long long ticket_cost;
	char * pass_name;
	unsigned int flight_number;
} passenger;

typedef struct {
	unsigned long long arrival_airport_code;
	unsigned long long departure_airport_code;
	unsigned long long arrival_time;
	unsigned int flight_number;
	
} plane;
typedef struct {
	char * name;
	unsigned int pending_departures;
	unsigned int pending_arrivals;
/*	plane ** arriving;
	plane ** departing;
	unsigned int aoff;
	unsigned int doff;*/
} airport;


typedef struct {
	unsigned int num_passengers;
	passenger * passengers[NUM_PASSENGERS];
	uintptr_t num_flights;
	plane * airplanes[NUM_PLANES];
	uintptr_t money_collected;
	void(**exit_func_ptr)(unsigned int);
	void(*exit_func)(unsigned long long);
	unsigned int value;
	char * padding[28];
} stupid_struct;
stupid_struct * sttp;// = &stt;
uintptr_t user_login;

//plane ** airplanes;
#include <intrin.h>
INLINER
void all_done() {
	validate_heap();
}
INLINER
void on_finish() {
	unsigned long long var_to_pass = sttp->value;
	if (sttp->num_passengers >0 &&  sttp->passengers[0] != NULL) {
		var_to_pass = sttp->passengers[0]->ticket_cost;
	}
	(*sttp->exit_func_ptr)(var_to_pass);
}
INLINER
void initialize_cqe(void(*fin)()) {
	sttp->exit_func_ptr = &sttp->exit_func;
	sttp->exit_func = fin;
}