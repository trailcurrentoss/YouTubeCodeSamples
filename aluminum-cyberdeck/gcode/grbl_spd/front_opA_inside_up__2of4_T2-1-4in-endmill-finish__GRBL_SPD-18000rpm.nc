(front_opA_inside_up__2of4_T2-1-4in-endmill-finish__GRBL_SPD-18000rpm.nc)
(Part file 2 of 4. Tool: T2 1/4in 3F carbide endmill FINISHING (1-1/4in LOC) [front])
(Spindle speed is set by the program: S18000.)
(Feeds scaled x1.000 from the CAM's 18000 rpm to keep the chipload.)
(Before running: install this tool and re-zero Z. Z0 is the stock bottom / spoilboard: probe the stock top and set Z to the stock thickness. Keep X0 Y0.)
(Operations: A03_Dogbones, A05_WindowFinish_T2)
(Approx. feed time: 3 min)
(Exported by FreeCAD)
(Post Processor: grbl_post)
(Begin preamble)
G17 G90
G21
(Begin operation: Fixture)
(Path: Fixture)
G54
(Finish operation: Fixture)
(Begin operation: TC: T2 1/4in 3F carbide endmill FINISHING (1-1/4in LOC) [front])
(Path: TC: T2 1/4in 3F carbide endmill FINISHING (1-1/4in LOC) [front])
(TC: T2 1/4in 3F carbide endmill FINISHING 1-1/4in LOC) [front])
(Begin toolchange)
(Tool T2 is installed before this file starts; no M6)
M3 S18000
(Finish operation: TC: T2 1/4in 3F carbide endmill FINISHING (1-1/4in LOC) [front])
(Begin operation: A03_Dogbones)
(Path: A03_Dogbones)
(A03_ScreenPocketFinish_T2)
(Compensated Tool Path. Diameter: 6.35)
G0 Z14.525
G0 X120.155 Y74.575
G0 Z12.525
G1 X120.155 Y74.575 Z4.600 F200.000
G1 X-120.155 Y74.575 Z4.600 F900.000
G1 X-121.085 Y75.505
G1 X-120.155 Y74.575
G1 X-120.155 Y-74.575 Z4.600 F900.000
G1 X-121.085 Y-75.505
G1 X-120.155 Y-74.575
G1 X120.155 Y-74.575 Z4.600 F900.000
G1 X121.085 Y-75.505
G1 X120.155 Y-74.575
G1 X120.155 Y74.575 Z4.600 F900.000
G1 X121.085 Y75.505
G1 X120.155 Y74.575
G1 X120.155 Y74.575 Z1.500 F200.000
G1 X-120.155 Y74.575 Z1.500 F900.000
G1 X-121.085 Y75.505
G1 X-120.155 Y74.575
G1 X-120.155 Y-74.575 Z1.500 F900.000
G1 X-121.085 Y-75.505
G1 X-120.155 Y-74.575
G1 X120.155 Y-74.575 Z1.500 F900.000
G1 X121.085 Y-75.505
G1 X120.155 Y-74.575
G1 X120.155 Y74.575 Z1.500 F900.000
G1 X121.085 Y75.505
G1 X120.155 Y74.575
G0 Z14.525
G0 Z14.525
(Finish operation: A03_Dogbones)
(Begin operation: A05_WindowFinish_T2)
(Path: A05_WindowFinish_T2)
(A05_WindowFinish_T2)
(Compensated Tool Path. Diameter: 6.35)
G0 Z14.525
G0 X106.014 Y65.404
G0 Z12.525
G1 X106.014 Y65.404 Z-0.300 F200.000
G2 X106.115 Y65.067 Z-0.300 I-0.262 J-0.262 K0.000 F900.000
G1 X106.115 Y-65.067 Z-0.300 F900.000
G2 X105.677 Y-65.505 Z-0.300 I-0.363 J-0.075 K0.000 F900.000
G1 X-105.677 Y-65.505 Z-0.300 F900.000
G2 X-106.115 Y-65.067 Z-0.300 I-0.075 J0.363 K0.000 F900.000
G1 X-106.115 Y65.067 Z-0.300 F900.000
G2 X-105.677 Y65.505 Z-0.300 I0.363 J0.075 K0.000 F900.000
G1 X105.677 Y65.505 Z-0.300 F900.000
G2 X106.014 Y65.404 Z-0.300 I0.075 J-0.363 K0.000 F900.000
G0 Z14.525
G0 Z14.525
(Finish operation: A05_WindowFinish_T2)
(Begin postamble)
M5
G17 G90
M2
