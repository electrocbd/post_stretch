linear_extrude(height=3)
{
	translate([0,0])
		difference()
		{
			circle(r=2.5,$fn=30);
			circle(r=0.5,$fn=30);
		};
	translate([4.5,0])
		difference()
		{
			circle(r=3,$fn=30);
			circle(r=1,$fn=30);
		};
	rotate(70.528779)
		translate([5,0])
			difference()
			{
				circle(r=3.5,$fn=30);
				circle(r=1.5,$fn=30);
			};
	rotate(146.856761)
		translate([5.5,0])
			difference()
			{
				circle(r=4,$fn=30);
				circle(r=2,$fn=30);
			};
	rotate(228.142019)
		translate([6,0])
			difference()
			{
				circle(r=4.5,$fn=30);
				circle(r=2.5,$fn=30);
			};
	rotate(300.481309)
		translate([8.110224,0])
			difference()
			{
				circle(r=5,$fn=30);
				circle(r=3,$fn=30);
			};
	rotate(352.886535)
		translate([11.944632,0])
			difference()
			{
				circle(r=5.5,$fn=30);
				circle(r=3.5,$fn=30);
			};
	rotate(43.365327)
		translate([12.636304,0])
			difference()
			{
				circle(r=6,$fn=30);
				circle(r=4,$fn=30);
			};
};

/*
linear_extrude(height=3) difference()
{
translate([4,2]) circle(r=16,$fn=30);
translate([0,0]) circle(r=0.5,$fn=30);
translate([4.5,0]) circle(r=1,$fn=30);
rotate(70.528779) translate([5,0]) circle(r=1.5,$fn=30);
rotate(146.856761) translate([5.5,0]) circle(r=2,$fn=30);
rotate(228.142019) translate([6,0]) circle(r=2.5,$fn=30);
rotate(300.481309) translate([8.110224,0]) circle(r=3,$fn=30);
rotate(352.886535) translate([11.944632,0]) circle(r=3.5,$fn=30);
rotate(43.365327) translate([12.636304,0]) circle(r=4,$fn=30);
}
*/
