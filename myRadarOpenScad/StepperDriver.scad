//Gabriele Salvato
//23/06/2018


module StepperDriver(fn, bPins) {
    pcbX = 29.0;
    pcbY = 21.0;
    pcbZ = 1.2;
    $fn = fn;
    union() {
        difference() {
            color("DARKBLUE")
            // PCB
            translate([0, 0, pcbZ/2])
                cube([pcbX, pcbY, pcbZ], center=true);
            //
            // Fori X Viti
            translate([pcbX/2-1.27, pcbY/2-1.27, pcbZ/2])
                cylinder(d=1.26 , h=2*pcbZ, center=true);
            translate([-pcbX/2+1.27, pcbY/2-1.27, pcbZ/2])
                cylinder(d=1.26 , h=2*pcbZ, center=true);
            translate([pcbX/2-1.27, -pcbY/2+1.27, pcbZ/2])
                cylinder(d=1.26 , h=2*pcbZ, center=true);
            translate([-pcbX/2+1.27, -pcbY/2+1.27, pcbZ/2])
                cylinder(d=1.26 , h=2*pcbZ, center=true);
        }
        //
        // ULN2003A
        color("black")
        translate([2.95, -2, pcbZ])
            rotate([0, 0, 90])
                cube([9.9, 3.91, 1.20], center=true);
        //
        // Pin Headers
        if(bPins) {
            translate([pcbX/2-4.0, -(2.54*5)/2, pcbZ+.1])
            //-pcbY/2+pippo, pcbZ+.1])
                rotate([0, 0, 90])
                    headerPin(6);
        }
        //
        color("white")
        translate([-pcbX/2+10, 0, pcbZ+3.5]) {
            difference() {
                cube([6, 15, 7], center=true);
                translate([0, 0, 1])
                    cube([5, 14, 7], center=true);
            }
        }
    }
}


module headerPin(numberOfPins) {
	for (i = [0:numberOfPins-1]) {
		color("GOLD") 
        translate([(i*2.54),0,-2]) 
            cube([0.5, 0.5, 8]);
		color("BLACK") 
        translate([(i*2.54-2.54/2+0.25),-2.54/2+0.25,0]) 
            cube([2.54, 2.54, 1]);
	}
}


//headerPin(4);
//StepperDriver(80, true);
