$fn = 80;


module opticalLimit() {
    union() {
        //PCB
        translate([0, 0, 1.25])
            color("green") cube([22, 10, 2.5], center=true);
        // Sensor
        translate([-4.6, -2.5, 2.50]) {
            color("gray") cube([4.4, 5, 10]);
            translate([9.4, 0, 0])
                color("gray") cube([4.4, 5, 10]);
            color("gray") cube([13.7, 5, 2.5]);
        }
    }
}

//opticalLimit();