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


function rotation(t) = t*360.0;

teta = rotation($t);
all(teta);


module all(alfa) {
    union() {
        // Supporto Motore
        color("yellow") supportoMotore($fn);
        //
        // Motore
        translate([0, -8, 25-18.5])
            stepper28BYJ($fn);
        //
        // Boccola
        rotate([0, 0, alfa])
            translate([0, 0, 25+3])
                motorBushing($fn);
        //
        // Testa + sensore US
        rotate([0, 0, alfa])
            union() {
                // Testa U-S
                translate([0, 0, 58])
                    rotate([90, 0, 0])
                        color("blue") US_head($fn);
                //
                // US module
                rotate([0, 0, 0])
                    translate([45/2, 4, 68])
                        rotate([90, 180, 0])
                            HC_SR04($fn);
                //
            }
        //
        // Limit Sensor
        translate([-40, 0, 22])
            opticalLimit($fn);
        //
    }// End union()
}
