
module US_head(fn) {
    $fn = fn;
    hDist = 27;// Distance between sensor holes
    hD    = 18;// Hole diameter
    rExt  = 26;
    pcbX = 45;
    pcbY = 20;
    pcbZ = 1.2;
    difference() {
        rotate([0, 0, 180]) {
            translate([-((16+27)/2+1), -(18/2+1), -6]) {
                union() {
                    difference() {
                        hull() {
                            translate([16/2+1, 18/2+1, 0])
                                cylinder(r=rExt, h=12);
                            translate([16/2+27+1, 18/2+1, 0])
                                cylinder(r=rExt, h=12);
                        }
                        translate([16/2+1, 18/2+1, -1])
                            cylinder(d=hD, h=14);// Sensor1 hole
                        translate([16/2+27+1, 18/2+1, -1])// Sensor2 hole
                            cylinder(d=hD, h=14);
                        translate([(pcbX)/2, pcbY-4, +5.0])
                            cube([11,8, 5], center=true);// Xtal space
                        translate([-0.5, -0.5, -1.0])
                            cube([pcbX+1, pcbY+1, 5]);// PCB space
                        translate([18.5-3.5, -17, -0.2])
                            cube([14, 18, 5]);// Space for cabling
                        translate([(pcbX)/2, 37, 6])
                            rotate([90, 0, 0])
                                cylinder(d=10.5, h=8);
                    }
                    // limit bar
                    translate([60, 27, 6])
                        cube([2, 27, 12], center=true);
                }
            }
        }
        translate([20, 7.5, -4])
            cylinder(d=1.5, h=4);
        translate([20, -7.5, -4])
            cylinder(d=1.5, h=4);
        translate([-20, -7.5, -4])
            cylinder(d=1.5, h=4);
        translate([-20,  7.5, -4])
            cylinder(d=1.5, h=4);
        // Smile
        translate([0, -12, 7])
            difference() {
                rotate_extrude(convexity = 10)
                    translate([10, 0, 0])
                        circle(r=2);
                translate([-25, 0, -2])
                    cube([50,50,4]);
            }
    }
}

//
//US_head(80);

