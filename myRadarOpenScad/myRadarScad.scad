$fn = 80;

include <stepper28BYJ.scad>
include <HC_SR04.scad>
include <US_head.scad>
include <opticalLimit.scad>
include <motorBushing.scad>
include <supportoMotore.scad>

bHead         = false; 
bBushing      = false;
bLimit        = false;
bWithUsModule = false;

dMotore = 28.1;

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
                    US_head();
            // Limit Sensor
            translate([-51, -10, -6])
                opticalLimit();
        }
        // US module
        translate([45/2, 12, 68])
            rotate([90, 180, 0])
                HC_SR04();
    }// End union()
}
else {
    if(bHead)
        head(false);
    else if(bBushing)
        motorBushing();
    else if(bLimit)
        opticalLimit();
}
