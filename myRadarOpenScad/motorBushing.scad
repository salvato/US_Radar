

module motorBushing(fn) {
    $fn = fn;
    difference() {
        translate([0, 0, 0.1])
           cylinder(d=10, h=31);
        intersection() {
            cylinder(r=5.5/2, h=16);
            cube([3.5, 6, 32], center=true);
        }
    }
}

// 
//motorBushing(80);
