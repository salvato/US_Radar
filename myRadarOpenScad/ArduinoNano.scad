//Gabriele Salvato
//23/06/2018

pcbX = 43.18;
pcbY = 17.78;
pcbZ = 1.2;


module USB_Female(fn) {
    $fn= fn;
    color("silver")
    difference() {
        union() {
            cube([9.2, 6.10, 2], center=true);
            translate([0, 0, 2]) 
                cube([9.2, 7.70, 2], center=true);
        }
        translate([1, 0, 0])
            union() {
                translate([0, 0, .2]) 
                    cube([9.2, 5.80, 2.2], center=true);
                translate([0, 0, 2.0]) 
                    cube([9.2, 7.50, 1.6], center=true);
            }
    }
    translate([0,0,1.])
        color("black")
        cube([8,4.6,1.60], center= true);
}

module pushButton(fn) {
    $fn= fn;
    translate([0, 0, 0.5])
        union() {
            color("silver")
                cube([2.0, 4.0, 1], center=true);
            translate([0, 0, 1]) 
                color("PaleGreen")
                    cube([1.0, 2.0, 1], center=true);
        }
}


module ArduinoNano(fn) {
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
        // USB connector
        translate([(pcbX-7.70)/2+1.45, 0, pcbZ+1])
            USB_Female(fn);
        //
        color("black")
        translate([7, 0, pcbZ])
            rotate([0, 0, 45])
                cube([6.90, 6.90, 1.20], center=true);
        // Pin Headers
        translate([-(pcbX-3*2.54)/2, (pcbY-2.54)/2, -0.2]) 
            rotate([180,0,0]) headerPin(15);
        translate([-(pcbX-3*2.54)/2, -(pcbY-2.54)/2, -0.2]) 
            rotate([180,0,0]) headerPin(15);
        //
        // Push Button
        translate([-2, 0, pcbZ]) 
            pushButton(80);
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


//Example call to headerPin module. The integer passed in defines the number of pins in the model
//headerPin(4);

//ArduinoNano(80);
//USB_Female(80);
