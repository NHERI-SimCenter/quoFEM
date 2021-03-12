
materialQuality	= 0.1			# float, [0,1], no units
planArea = 1500		# float, [800., 2400.] inch
aspectRatio = 0.7		# float, [0.4, 0.8], no units
damping	 = 0.07				# float, [0.02, 0.1] percent
extDensity_x = 0.75				# float, [0.5, 0.8], no units (where extDensity_x*Perimeter = length of exterior wall on first floor in x direction)
extDensity_z = 0.8		# float, [0.5, 0.8], no units (where extDensity_z*Perimeter = length of exterior wall on first floor in z direction)
intDensity = 0.07			# float, [0.06, 0.08], no units (where rho_int*Area =  total length of interior wall on first floor)
extEcc_x =0.2		# float, [0.0, 0.5], no units (offset from CoD to x-direction exterior wall)
extEcc_z = 0.2					# float, [0.0, 0.5], no units (offset from CoD to z-direction exterior wall)
intEcc_x = 0.2					# float, [0.0, 0.5], no units (offset from CoD to x-direction exterior wall)
intEcc_z = 0.2					# float, [0.0, 0.5], no units (offset from CoD to z-direction exterior wall)
floor1Weight = 10.5			# float, [10.0, 15.5] psf
floor2Weight = 13.5			# float, psf (range dependent on construction era)
roofWeight	 = 11.5			# float, psf (range dependent on construction era)