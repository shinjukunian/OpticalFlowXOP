#pragma TextEncoding = "UTF-8"
#pragma rtGlobals=3				// Use modern global access method and strict wave access
#pragma DefaultTab={3,20,4}		// Set default tab width in Igor Pro 9 and later
#include <ImageSlider>




function makeArrowWave(xDisplacement,yDisplacement,sliceIDX, imageWave,scaleFactor,[threshold,arrowLength])
	wave xDisplacement,yDisplacement
	variable sliceIDX
	wave imageWave
	variable scaleFactor
	variable threshold, arrowLength
	
	if(paramIsDefault(threshold))
		threshold=0
	endif
	if(paramIsDefault(arrowLength))
		arrowLength=10
	endif
	
	duplicate /free yDisplacement rWave, phiWave
	phiwave=atan2(xDisplacement,yDisplacement)
	rwave=sqrt(xDisplacement^2+yDisplacement^2)
	
	duplicate /free /rmd=[][][sliceIDX] imageWave, sliceWave
	imageinterpolate /pxsz={scaleFactor,scaleFactor} Pixelate sliceWave
	wave M_PixelatedImage
	duplicate /free M_PixelatedImage image_Resampled
	
	imageinterpolate /pxsz={scaleFactor,scaleFactor} Pixelate rwave
	
	duplicate /free M_PixelatedImage rScaled
	imageinterpolate /pxsz={scaleFactor,scaleFactor} Pixelate phiwave
	duplicate /free M_PixelatedImage phiScaled
	
	setscale /p x, dimOffset(rWave,0) * scaleFactor, dimdelta(rWave,0) * scaleFactor, waveunits(rWave,0), rScaled, phiScaled
	setscale /p y, dimOffset(rWave,1) * scaleFactor, dimdelta(rWave,1) * scaleFactor, waveunits(rWave,1), rScaled, phiScaled
	
	make/n=(dimsize(rScaled,0)*dimsize(rScaled,1))/o xWave,yWave, colorWave
	make/n=(dimsize(rScaled,0)*dimsize(rScaled,1),2)/o arrowWave
	
	killwaves/z M_PixelatedImage
	
	xWave=nan
	yWave=nan
	
	if(sliceIDX < 0 || sliceIDX >= dimsize(rScaled,2))
		return 0
	endif
	
	variable i,j
	variable ctr=0

	for(i=0;i<dimsize(rScaled,0);i+=1)
		for(j=0;j<dimsize(rScaled,1);j+=1)

			xWave[ctr]=(dimoffset(rScaled,0)+i*dimdelta(rScaled,0)) 
			yWave[ctr]=(dimoffset(rScaled,1)+j*dimdelta(rScaled,1)) 
			arrowWave[ctr][1]=phiScaled[i][j][sliceIDX]  - pi/2
			arrowWave[ctr][0]=rScaled[i][j] > threshold ? arrowLength : nan
			colorWave[ctr]=rScaled[i][j][sliceIDX]
			ctr+=1


		endfor
	endfor
	
end
