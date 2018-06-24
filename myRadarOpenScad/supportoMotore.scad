$fn = 80;


module supportoMotore() {
    union() {
        // Base
        difference() {
            cylinder(d=120, h=5);
            for(alfa=[0:90:269]) {
                echo(alfa);
                rotate([0, 0, alfa])
                    translate([0, -35, -1])
                        cylinder(d=25, h=7);
            }
            for(alfa=[0:90:359]) {
                rotate([0, 0, alfa+45])
                    translate([0, -35, -1])
                        cylinder(d=20, h=7);
            }
            translate([0, 0, -1])
               cylinder(d=20, h=7);
        }
        // Supporto Vite motore sinistra
        difference() {
            translate([-17.5, 0, 0])
                cylinder(d=7, h=25);
            translate([-17.5, 0, 10])
                cylinder(d=2.5, h=16);
        }
        difference() {
            translate([17.5, 0, 0])
                cylinder(d=7, h=25);
            translate([17.5, 0, 10])
                cylinder(d=2.5, h=16);
        }
        // Supporto per Sensore ottico
        difference() {
            translate([-55, -4, 0])
                cube([30, 14, 25]);
            translate([-52, -3, 20])
                cube([24, 12, 25]);
        }
    }
}
