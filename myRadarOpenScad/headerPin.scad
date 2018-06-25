
//Mark Benson
//23/07/2013
//Creative Commons non commercial
//http://www.thingiverse.com/thing:122136
//Modified by
//Gabriele Salvato
//23/06/2018

module headerPin(numberOfPins) {
	for (i = [0:numberOfPins-1]) {
		color("GOLD") translate([(i*2.54),0,-2]) cube([0.5,0.5,8]);
		color("BLACK") translate([(i*2.54-2.54/2+0.25),-2.54/2+0.25,0]) cube([2.54,2.54,1]);
	}
}


//Example call to headerPin module. The integer passed in defines the number of pins in the model
//headerPin(4);
