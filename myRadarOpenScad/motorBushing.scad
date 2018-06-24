

module motorBushing(fn) {
    $fn = fn;
    #difference() {
        translate([0, 0, 0.1])
           cylinder(d=10, h=11);
        #intersection() {
            cylinder(r=5/2, h=13);
            cube([3, 6, 9], center=true);
        }
    }
}

// 
//motorBushing(80);