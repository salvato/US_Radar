$fn = 80;

include <stepper28BYJ.scad>
include <HC_SR04.scad>

bHead         = false; 
bBushing      = false;
bLimit        = false;
bWithUsModule = false;

dMotore = 28.1;
module opticalLimit() {
    union() {
        color("green") cube([22, 10, 2.5]);
        translate([6.6, 2.5, 2.5]) {
            color("gray") cube([4.4, 5, 10]);
            translate([9.4, 0, 0])
                color("gray") cube([4.4, 5, 10]);
        color("gray") cube([13.7, 5, 2.5]);
        }
    }
}

module head() {
    hDist = 27;// Distance between sensor holes
    hD    = 18;// Hole diameter
    rExt  = 26;
    pcbX = 45;
    pcbY = 20;
    pcbZ = 1.2;
    rotate([0, 0, 180]) {
        translate([-((16+27)/2+1), -(18/2+1), 0]) {
            union() {
                difference() {
                    hull() {
                        translate([16/2+1, 18/2+1, 0])
                            cylinder(r=rExt, h=12);
                        translate([16/2+27+1, 18/2+1, 0])
                            cylinder(r=rExt, h=12);
                    }
                    translate([16/2+1, 18/2+1, -1])
                        cylinder(d=hD, h=14);// Sensor1 hole
                    translate([16/2+27+1, 18/2+1, -1])// Sensor2 hole
                        cylinder(d=hD, h=14);
                    translate([(pcbX)/2, pcbY-4, +5.0])
                        cube([11,8, 5], center=true);// Xtal space
                    translate([-0.5, -0.5, -1.0])
                        cube([pcbX+1, pcbY+1, 5]);// PCB space
                    translate([18.5-3.5, -17, -0.2])
                        cube([14, 18, 5]);// Space for cabling
                    translate([(pcbX)/2, 37, 6])
                        rotate([90, 0, 0])
                            cylinder(d=10.5, h=8);
                }
                // limit bar
                translate([60, 27, 11])
                    cube([2, 27, 2], center=true);
            }
        }
    }
}


module motorBushing() {
    #difference() {
        translate([0, 0, 0.1])
           cylinder(d=10, h=11);
        #intersection() {
            cylinder(r=5/2, h=13);
            cube([3, 6, 9], center=true);
        }
    }
}


module supportoMotore() {
    difference() {
        translate([0, 0, 12.5])
            cube([100, 50, 25], center = true);
        translate([0, 0, 25-19])
            cylinder(d=dMotore, h=45);
        // Scasso per Sensore ottico
        translate([-45, -3, 23])
            cube([24, 12, 2.5]);
        translate([-52, 1, 23])
            cube([0, 4, 2.5]);
        translate([-9, -30, 21])
            cube([18, 20, 5]);
    }
}


bAll = !bHead && !bBushing && !bLimit;

if(bAll) {
    union() {
        // Supporto Motore
        supportoMotore();
        // Motore
        translate([0, 0, 25-18.5])
            stepper28BYJ();
        translate([0, 8, 25+3]) {
            // Boccola
            motorBushing();
            // Testa U-S
            translate([0, 6, 30])
                rotate([90, 0, 0])
                    head(true);
            // Limit Sensor
            translate([-51, -10, -6])
                opticalLimit();
        }
        // US module
        translate([45/2, 12, 68])
            rotate([90, 180, 0])
                HC_SR04();
    }
}
else {
    if(bHead)
        head(false);
    else if(bBushing)
        motorBushing();
    else if(bLimit)
        opticalLimit();
}
