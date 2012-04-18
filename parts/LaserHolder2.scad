$fn = 40; //change this to make the edges more round! Rendering will become slower.

lhIR = 6; //this is where you define the radius of your laser module!
lhOR = lhIR+6; 
lhH = 8;

shIR = 2.5;
shXS = shIR+2*lhH;
shYS = 2*lhOR;
shZS = 15;

nutY = 2.6;
nutZ = 5.6;

module screw(){
	rotate([-90,0,0])
	cylinder(h=25, r=1.5);
}

//breit: 5.4mm
//hoch: 2.3mm
module nut(){
	scale([25, nutY, nutZ])
	cube();
}

module laserHolder(){
	difference(){
		union(){
			rotate([0,90,0])
			cylinder(h=lhH, r=lhOR);
			translate([0,-lhOR,-(shZS+lhIR)])
			scale([lhH,2*lhOR,shZS+lhIR])
			cube(1);
		}
		rotate([0,90,0])
		translate([0,0,-1])
		cylinder(h=lhH+2, r=lhIR);
		translate([-2,(lhIR+lhOR)/2-1.5,-nutZ/2])
		nut();
		translate([lhH/2,0,0])
		screw();
	}
}

module stepperHolder(){
	difference(){
		translate([-shXS/2, -shYS/2])
		scale([shXS, shYS, shZS])
		cube(1);
		translate([0,0,-1])
		cylinder(h=shZS+2, r=shIR);	
	}
}

module complete(){
	difference(){
		union(){
			translate([0,0,lhIR+shZS]){
				translate([-lhH-shIR,0,0])
				laserHolder();
				translate([shIR,0,0])
				laserHolder();
			}
			stepperHolder();
		}
		rotate([0,0,90])
		translate([0,0,5])
		screw();
		rotate([90,90,0])
		translate([-3*nutZ,-7.5,-nutZ/2])
		nut();
	}
}

//laserHolder();
//stepperHolder();
complete();