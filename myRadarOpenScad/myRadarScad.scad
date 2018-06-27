$fn = 20;

include <stepper28BYJ.scad>
include <HC_SR04.scad>
include <US_head.scad>
include <opticalLimit.scad>
include <motorBushing.scad>
include <supportoMotore.scad>
include <ArduinoNano.scad>
include <StepperDriver.scad>

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
            translate([0, 0, 77]) {
                rotate([90, 0, 0]) {
                    union() {
                        US_head(20);
                        rotate([0, 0, 180])
                            translate([-45/2, -10, -4])
                                HC_SR04(20);
                    }
                }
            }
        //
        // Limit Sensor
        translate([-40, 0, 22])
            opticalLimit($fn);
        //
        // Arduino Nano
        translate([0, 40, 12])
            rotate([0, 0, 0])
                ArduinoNano($fn, false);
        //
        // StepperDriver
        translate([-40, 20, 12])
            rotate([0, 0, 0])
                StepperDriver($fn, true);
    }// End union()
}
