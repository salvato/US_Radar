$fn = 80;

include <stepper28BYJ.scad>
include <HC_SR04.scad>


dMotore = 28.1;


module head(bWithUsModule) {
    rExt= 35/2;
    difference() {
        rotate([0, 0, 180])
            translate([-((16+27)/2+1), -(18/2+1), 0])
                union() {
                    difference() {
                        hull() {
                            translate([16/2+1, 18/2+1, 0])
                                cylinder(r=rExt, h=12);
                            translate([16/2+27+1, 18/2+1, 0])
                                cylinder(r=rExt, h=12);
                        }
                        union() {
                            translate([-0.05, -0.05, -0.1])
                                cube([45.1, 20.1, 5]);
                            translate([0, 0, 5-1.5])
                                HC_SR04();
                            translate([18.5-3.5, -10, -0.2])
                                cube([14, 10, 5]);
                        }
                    }
                    if(bWithUsModule)
                        translate([0, 0, 5-1.5])
                            HC_SR04();
                }
        translate([0, -12, 6])
            rotate([90, 0, 0])
                cylinder(d= 10.5, h= 8);
    }
}


module MotorBushing() {
    #difference() {
        translate([0, 0, 0.1])
           cylinder(d=10, h=11);
        #intersection() {
            cylinder(r=5/2, h=13);
            cube([3, 6, 9], center=true);
        }
    }
}


union() {
/*
    // Supporto Motore
    difference() {
        translate([0, 0, 12.5])
            cube([50, 50, 25], center = true);
        translate([0, 0, 25-19])
            cylinder(d=dMotore, h=45);
    }
*/    
    // Motore
    translate([0, 0, 25-18.5])
        stepper28BYJ();
    translate([0, 8, 25+3]) {
        // Boccola
        MotorBushing();
        // Testa U-S
        translate([0, 6, 26])
            rotate([90, 0, 0])
                head(true);
    }

}

