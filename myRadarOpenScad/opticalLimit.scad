$fn = 80;


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
