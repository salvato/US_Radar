$fn = 80;

// 28BYJ-48 Stepper Motor Model
// Mark Benson
// 23/07/2013
// Creative Commons Non Commerical
// http://www.thingiverse.com/thing:122070
module stepper28BYJ() {
    difference() {
        union()	{
            //Body
            color("SILVER") cylinder(r=28/2, h=19);
            //Base of motor shaft
            color("SILVER") translate([0,8,19]) cylinder(r=9/2, h=1.5);
            //Motor shaft
            color("SILVER") translate([0,8,20.5]) 
            intersection() {
                cylinder(r=5/2, h=9);
                cube([3, 6, 9],center=true);
            }
            //Left mounting lug
            color("SILVER") translate([-35/2,0,18.5]) mountingLug();
            //Right mounting lug
            color("SILVER") translate([35/2,0,18.5]) rotate([0,0,180]) mountingLug();
            difference() {
                //Cable entry housing
                color("BLUE") translate([-17.5/2,-17,1.9]) cube([17.5,17,17]);
                cylinder(r=27/2, h=29);
            }
        }
        union() {
            //Flat on motor shaft
            //translate([-5,0,22]) cube([10,7,25]);
        }
    }
}//end of stepper28BYJ module wrapper

module mountingLug() {
	difference() {
		hull() {
			cylinder(r=7/2, h=0.5);
			translate([0,-7/2,0]) cube([7,7,0.5]);
		}
		translate([0,0,-1]) cylinder(r=4.2/2, h=2);
	}
}


//HC-SR04 Ultrasonic distance sensor module
//Mark Benson
//23/07/2013
//Creative Commons non commercial
//http://www.thingiverse.com/thing:122136
module HC_SR04() {
    difference() {
        union() 	{
            color("DARKBLUE") cube([45, 20, 1.2]);
            translate([16/2+1, 18/2+1, 1.3]) ultrasonicSensor();
            translate([16/2+27+1, 18/2+1, 1.3]) ultrasonicSensor();
            color("SILVER")translate([45/2-3.25, 20-4, 1.3]) xtal();
            translate([(45/2)-((2.45*4)/2)+0.6, 2, -0.2]) rotate([180,0,0]) headerPin(4);
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
			translate([0,0,0]) cylinder(r=16/2, h=13.8, $fn=50);
			//Hole in body
			translate([0,0,0]) cylinder(r=12.5/2, h=14, $fn=50);
		}
		//Insert to indicate mesh
		color("GREY") translate([0,0,0]) cylinder(r=12.5/2, h=13, $fn=50);
	}
}


module xtal() {
	//Xtal maximum dimensions for checking model
	//color("red")translate([0,0,-1]) cube([11.5,4.65,1]);
	//color("red")translate([(11.54-10.3)/2,(4.65-3.8)/2,0]) cube([10.3,3.8,3.56]);
	union() {
		//Base
		hull() {
			translate([0,0,0]) cylinder(r=4.65/2, h=0.4, $fn=50);
			translate([6.5,0,0]) cylinder(r=4.65/2, h=0.4, $fn=50);
		}
		//Body
		hull() {
			translate([0,0,0]) cylinder(r=3.7/2, h=3.56, $fn=50);
			translate([6.5,0,0]) cylinder(r=3.7/2, h=3.56, $fn=50);
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






dMotore = 28.1;


union() {
    /*
    // Supporto Motore
    difference() {
        translate([0, 0, 12.5])
            cube([50, 50, 25], center = true);
        translate([0, 0, 25-19])
            cylinder(d=dMotore, h=45);
    }
    
    // Motore
    translate([0, 0, 25-18.5])
        stepper28BYJ();
    
    // raccordo per asse motore
    translate([0, 8, 25+3])
        #difference() {
            translate([0, 0, 0.1])
               cylinder(d=10, h=7);
            #intersection() {
                cylinder(r=5/2, h=9);
                cube([3, 6, 9], center=true);
            }
        }
    */
    difference() {
		rExt= 35/2;
        hull() {
			translate([0, -rExt/2, 0]) 
                cylinder(r=rExt, h=12);
			translate([0, rExt/2, 0]) 
                cylinder(r=rExt, h=12);
		}
        translate([0, -13, -1])
            cylinder(r=16/2, h= 14);
        translate([0, 13, -1])
            cylinder(r=16/2, h= 14);
    }
    HC_SR04();
}

