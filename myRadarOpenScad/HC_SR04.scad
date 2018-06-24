
//HC-SR04 Ultrasonic distance sensor module
//Mark Benson
//23/07/2013
//Creative Commons non commercial
//http://www.thingiverse.com/thing:122136
//Modified by
//Gabriele Salvato
//23/06/2018

sD = 16;  // Sensor diameter
sH = 13.8;// Sensor Height
dS = 27;  // Distance between Sensors
pcbX = 45;
pcbY = 20;
pcbZ = 1.2;

module HC_SR04(fn) {
    $fn = fn;
    difference() {
        union() 	{
            color("DARKBLUE") cube([pcbX, pcbY, pcbZ]);// PCB
            translate([sD/2+1, sD/2+2, pcbZ+0.1]) ultrasonicSensor();
            translate([sD/2+dS+1, sD/2+2, pcbZ+0.1]) ultrasonicSensor();
            color("SILVER")translate([pcbX/2-3.25, pcbY-4, pcbZ+0.1]) xtal();
            translate([(pcbX/2)-((2.45*4)/2)+0.6, 2, -0.2]) rotate([180,0,0]) headerPin(4);
        }
        union() 	{
            color("DARKBLUE") translate([2.5, 2.5, -1]) cylinder(r=2/2, h=3);
            color("DARKBLUE") translate([2.5+40, 2.5+15,- 1]) cylinder(r=2/2, h=3);
        }
    }
}


module ultrasonicSensor() {
	union() { 
		//Sensor body
		color("SILVER")
		difference() {
			//Body
			translate([0,0,0]) cylinder(r=sD/2, h=sH);
			//Hole in body
			translate([0,0,1]) cylinder(r=(sD-3.5)/2, h=sH+1);
		}
		//Insert to indicate mesh
		color("GREY") translate([0,0, 2]) cylinder(r=12.5/2, h=sH-3);
	}
}


module xtal() {
	//Xtal maximum dimensions for checking model
	//color("red")translate([0,0,-1]) cube([11.5,4.65,1]);
	//color("red")translate([(11.54-10.3)/2,(4.65-3.8)/2,0]) cube([10.3,3.8,3.56]);
	union() {
		//Base
		hull() {
			translate([0,0,0]) cylinder(r=4.65/2, h=0.4);
			translate([6.5,0,0]) cylinder(r=4.65/2, h=0.4);
		}
		//Body
		hull() {
			translate([0,0,0]) cylinder(r=3.7/2, h=3.56);
			translate([6.5,0,0]) cylinder(r=3.7/2, h=3.56);
		}
	}
}


module headerPin(numberOfPins) {
	for (i = [0:numberOfPins-1]) {
		color("GOLD") translate([(i*2.54),0,-2]) cube([0.5,0.5,8]);
		color("BLACK") translate([(i*2.54-2.54/2+0.25),-2.54/2+0.25,0]) cube([2.54,2.54,1]);
	}
}


//Example call to headerPin module. The integer passed in defines the number of pins in the model
//headerPin(4);

//Call model - comment out when including this file in another model
//ultrasonicSensor();

//HC_SR04(80);
