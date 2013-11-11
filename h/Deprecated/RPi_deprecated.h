/* Deprecated methods
 */

//from RPi_general
void RPi_deprecated_humistor(struct Humistor *humistor1) {
	struct timespec sleep_time;
		sleep_time.tv_sec = HUMISTOR_SLPSEC;
		sleep_time.tv_nsec = HUMISTOR_SLPNSC * HUMISTOR_CRCNSC;  //APPLY A CORRECTION FACTOR TO COMPENSATE FOR RASPI TIME LOSSES
	printf("%lu", sleep_time.tv_nsec);
	/*
struct Humistor {
	struct timespec start_meas;
	struct timespec end_meas;
	unsigned long elapsed_us;
	float voltage_initial;
	float voltage;
	float capacitance_uf;
	float humidex;
	unsigned short ADC;
	unsigned short ADC_initial;
	unsigned long parses;
};
	unsigned long t_esecs;
	unsigned long t_ensec;
	struct timespec sleep_time;
	sleep_time.tv_sec = 0;
	sleep_time.tv_nsec = 1000000;
	(*humistor1).parses = 0; //set the parses to zero
	double capacitance_alt;
	double capacitance_alt2;
	
	clock_gettime(CLOCK_MONOTONIC, &((*humistor1).start_meas)); //record the current time, for humistor
	(*humistor1).ADC_initial = MCP3008_SingleEndedRead(HUMISTOR_CH); //store the initial voltage at humistor (should be zero)
	do {
		(*humistor1).ADC = MCP3008_SingleEndedRead(HUMISTOR_CH);
		(*humistor1).voltage = ((*humistor1).ADC*REF_VOLTAGE)/1024;
		clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep_time, NULL);
		//printf("%f \n", (*humistor1).voltage);
		(*humistor1).parses++;
	} while ((*humistor1).voltage < (3.3*(1 - exp(-1))));
	clock_gettime(CLOCK_MONOTONIC, &((*humistor1).end_meas)); //record the current time.
	
	************************************************************
	************ PROCESS DATA FROM HUMISTOR ********************
	************************************************************
	(*humistor1).voltage_initial = ((*humistor1).ADC_initial*REF_VOLTAGE)/1024;
	t_esecs = ((*humistor1).end_meas.tv_sec - (*humistor1).start_meas.tv_sec);
	t_ensec = ((*humistor1).end_meas.tv_nsec - (*humistor1).start_meas.tv_nsec);
	(*humistor1).elapsed_us = ((t_esecs * 1000000) + (t_ensec / 1000) + 0.5);
	(*humistor1).capacitance_uf = (((*humistor1).elapsed_us / (float)HUMISTOR_R1) - //must scale capacitance to compensate for sampling losses
		(0)); // quick sampling consumes power, use of high ohm resistor makes this matter
	printf("%ld \n", (*humistor1).parses);
	capacitance_alt = (*humistor1).capacitance_uf * exp(-(((*humistor1).parses) / (1 + (*humistor1).parses + ((sleep_time.tv_nsec) / ((*humistor1).parses)))));
	capacitance_alt2 = (*humistor1).capacitance_uf - (*humistor1).capacitance_uf * (((*humistor1).parses) / ((*humistor1).parses + (1000000000/sleep_time.tv_nsec) * pow((sleep_time.tv_nsec / (*humistor1).parses), 0.36737)));
	printf("|| HUMIS1: %.2fVi %fVf %.2fVt %ldus %.2fuf %.2lfuf1 %.2lfuf2 %dADC\n", (*humistor1).voltage_initial, (*humistor1).voltage, 
		(3.3*(1 - exp(-1))), (*humistor1).elapsed_us, (*humistor1).capacitance_uf, capacitance_alt, capacitance_alt2, (*humistor1).ADC);
	
	/
	(*humistor1).start_meas = (*humistor1).end_meas;
	clock_gettime(CLOCK_MONOTONIC, &((*humistor1).end_meas));
	printf("%u.%u diff: %u\n", (unsigned int)(*humistor1).end_meas.tv_sec, (unsigned int)(*humistor1).end_meas.tv_nsec, ((unsigned int)(*humistor1).end_meas.tv_nsec-(unsigned int)(*humistor1).start_meas.tv_nsec));
	*/	
	
	/* MORE DEPRECATED CODE:
	(*humistor1).ADC_initial = MCP3008_SingleEndedRead(HUMISTOR_CH); //store the initial voltage at humistor (should be zero)
	clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep_time, NULL);
	(*humistor1).ADC = MCP3008_SingleEndedRead(HUMISTOR_CH);
	
	//calcs the data
	(*humistor1).voltage_initial = ((*humistor1).ADC_initial*REF_VOLTAGE)/1024;
	(*humistor1).voltage = ((*humistor1).ADC*REF_VOLTAGE)/1024;
	//(*humistor1).capacitance_uf = (sleep_time.tv_sec * 1000000 + sleep_time.tv_nsec/1000) / 
	//	(-log((REF_VOLTAGE - ((*humistor1).voltage)) / REF_VOLTAGE) * (float)HUMISTOR_R1);
	(*humistor1).capacitance_pf = (sleep_time.tv_nsec*1000) / 
		(-log((REF_VOLTAGE - ((*humistor1).voltage)) / REF_VOLTAGE) * (float)HUMISTOR_R1);
	(*humistor1).humidex = ((*humistor1).capacitance_pf - 298) / 0.6;
	printf("|| HUMIS1: %.2fVi %.2fVf %.1fpf %dADCi %dADC\n", (*humistor1).voltage_initial, (*humistor1).voltage, 
		(*humistor1).capacitance_pf, (*humistor1).ADC_initial, (*humistor1).ADC); 
	*/
}
