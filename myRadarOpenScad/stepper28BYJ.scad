$fn = 80;

// 28BYJ-48 Stepper Motor Model
// Mark Benson
// 23/07/2013
// Creative Commons Non Commerical
// http://www.thingiverse.com/thing:122070
module stepper28BYJ() {
    difference() {
        union()	{
            //Body
            color("SILVER") cylinder(r=28/2, h=19);
            //Base of motor shaft
            color("SILVER") translate([0,8,19]) cylinder(r=9/2, h=1.5);
            //Motor shaft
            color("SILVER") translate([0,8,20.5]) 
            intersection() {
                cylinder(r=5/2, h=9);
                cube([3, 6, 9],center=true);
            }
            //Left mounting lug
            color("SILVER") translate([-35/2,0,18.5]) mountingLug();
            //Right mounting lug
            color("SILVER") translate([35/2,0,18.5]) rotate([0,0,180]) mountingLug();
            difference() {
                //Cable entry housing
                color("BLUE") translate([-17.5/2,-17,1.9]) cube([17.5,17,17]);
                cylinder(r=27/2, h=29);
            }
        }
        union() {
            //Flat on motor shaft
            //translate([-5,0,22]) cube([10,7,25]);
        }
    }
}//end of stepper28BYJ module wrapper