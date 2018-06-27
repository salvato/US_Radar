

module motorBushing(fn) {
    $fn = fn;
    difference() {
        union() {
            cylinder(d=25, h=15);
            translate([0, 0, 15])
                sphere(d=25);
            translate([0, 0, 14])
                cylinder(d=10, h=31);
        }
        translate([0, 0, -1])
            cylinder(d=20, h=15);
        translate([0, 0, 15])
            sphere(d=20);
        translate([0, -15, -1])
            cube([30, 30, 52]);
    }
}

// 
motorBushing(80);
