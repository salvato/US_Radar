

module motorBushing(fn) {
    $fn = fn;
    difference() {
        translate([0, 0, 0.1])
           cylinder(d=10, h=11);
        intersection() {
            cylinder(r=5.5/2, h=8);
            cube([3.5, 6, 17], center=true);
        }
    }
}

// 
//motorBushing(80);
