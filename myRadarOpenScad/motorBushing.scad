

module motorBushing(fn) {
    $fn = fn;
    difference() {
        union() {
            translate([0, 0, 0])
               cylinder(d=18, h=10);
            translate([0, 0, 9.99])
               sphere(d=18);
            translate([0, 0, 0.1])
               cylinder(d=10, h=31);
        }
        translate([0, 0, -0.1])
           cylinder(d=14, h=10);
        translate([0, 0, 9.99])
           sphere(d=14);
        /*
        translate([-10, 0, -1.0])
           cube([20, 20, 35]);
        */
    }
}

// 
motorBushing(80);
