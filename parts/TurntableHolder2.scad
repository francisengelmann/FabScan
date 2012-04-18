$fn = 40;

r_motorSchaft = 2.5;
l_motorSchaft = 21;

r_aussparung = 10;
l_aussparung = 8;

r_Stange = 7.85/2;
l_Stange = 22.5;


//Schlitz 5.4 x 2.5

d_teller = 40;

dist_schrauben = d_teller/2;


module anMotor()
{
	difference()
	{
		cylinder( h=l_aussparung, r=r_aussparung);
		translate(v=[0,0,-1]) cylinder(h=l_motorSchaft, r=r_motorSchaft);
		translate(v=[-r_aussparung-0.5,0,4.8]) rotate(a=[0,90,0]) cylinder(h=r_aussparung*2+1, r=1.5);

		translate(v=[-r_aussparung+3.5,0-5.4/2,6+5.4/2]) rotate(a=[0,90,0]) scale([9.2, 5.4, 2.5]) cube(1);//Mutter
		translate(v=[r_aussparung-6,0-5.4/2,6+5.4/2]) rotate(a=[0,90,0]) scale([9.2, 5.4, 2.5]) cube(1);//Mutter



	}
}


module M3Schraube()
{
	translate([0,0,-3])
	{
		union()
		{
			cylinder(h=l_aussparung+9, r=1.5);

			//translate([0,0,l_aussparung+9]) cylinder(h=3.3, r=5.5/2);//dickerer Kopf für Senke
			translate([0,0,l_aussparung+9])  cylinder(h=l_aussparung+9, r=1.5);//da wir erstmal nur kleben wollen sparen wir hier stützmaterial
		}
	}

}

module rundung()
{
		union()
		{
		for ( i = [0 : 90] )
		{
			translate([0,0,l_aussparung+ i*0.1]) cylinder(h=0.1, r=r_aussparung + 2* sin(i)*d_teller/4);
		}
		}
}

module anStange()
{
	difference()
	{
		rundung();

		union()
		{
			translate(v=[0,0,-1]) cylinder(h=l_motorSchaft, r=r_motorSchaft);
			translate([dist_schrauben, 0, 0]){ M3Schraube(); translate([-3.0,-3.0,6.0]) scale([6.0, 6.0, 6.0]) cube(1);}
			translate([-dist_schrauben, 0, 0]){ M3Schraube();translate([-3.0,-3.0,6.0]) scale([6.0, 6.0, 6.0]) cube(1);}
			translate([0, dist_schrauben, 0]){ M3Schraube();translate([-3.0,-3.0,6.0]) scale([6.0, 6.0, 6.0]) cube(1);}
			translate([0, -dist_schrauben, 0]){ M3Schraube();translate([-3.0,-3.0,6.0]) scale([6.0, 6.0, 6.0]) cube(1);}
		}

	}
}


module sparbox()
{
	translate([0,5,l_aussparung+4]) scale([8,16,15]) cube(1, center=true);
}

module materialsparen()
{
union()
{
translate([-10,10,0]) rotate(45)  sparbox();
translate([17,17,0]) rotate(135)  sparbox();
translate([17,-17,0]) rotate(45)  sparbox();
translate([-10,-10,0]) rotate(135)  sparbox();
}
}


module kupplung()
{
	union()
	{
		anMotor();
		anStange();
	}
}

difference()
{
kupplung();
materialsparen();
}