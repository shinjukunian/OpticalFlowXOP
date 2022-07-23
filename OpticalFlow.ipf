#pragma TextEncoding = "UTF-8"
#pragma rtGlobals=3				// Use modern global access method and strict wave access
#pragma DefaultTab={3,20,4}		// Set default tab width in Igor Pro 9 and later
#include <ImageSlider>

// This function draws the optical flow field of an image stack as color-coded arrows.
//xDisplacement, yDisplacement ar the outputs of the optical flow calculation
//images is the image stack
//threshold is the minimum displacement for which arrows should be drawn. 0 is a good starting value.
//scaleFactor determines by how much to downscale the optical flow field for display. 
//While dense optical flow provides the displacement of every pixel in the image, this results in very crowded plots.
//Using a scale factor of 20 would result in arrows being drawn for 15x15 pixel areas for a 300x300 pixel image.

function makeOpticalFlowOverlay(xDisplacement, yDisplacement, images, threshold, scaleFactor)
	wave xDisplacement, yDisplacement, images
	variable threshold, scaleFactor

	Display /W=(319,102,766,599)
	AppendImage images
	ModifyImage $nameofwave(images) ctab= {*,*,Grays,0}
	ModifyGraph margin(left)=28,margin(bottom)=28,margin(top)=28,margin(right)=28,width={Plan,1,bottom,left}
	ModifyGraph mode=3

	ModifyGraph mirror=2
	ModifyGraph noLabel=2
	ModifyGraph standoff=0
	ModifyGraph axThick=0
	SetAxis/A/R left
	ControlBar 50
	
	Slider slider_opticalFlow,pos={65,10.00},size={300,15.00},proc=SliderProc_opticalFlowOverlay
	Slider slider_opticalFlow,limits={0,dimsize(images,2)-1,0},value=1,vert=0,ticks=0
	SetVariable setvar_opticalFlowFrame pos={10,10},proc=SetVarProc_opticalFlow,value= _NUM:1,limits={0,dimsize(images,2)-1,1}
	SetVariable setvar_opticalFlowResolution title="Scale",size={80,20}, pos={10,30},proc=SetVarProc_opticalFlow,value= _NUM:scaleFactor,limits={0,dimsize(images,1)/4,1}
	SetVariable setvar_opticalFlowMinDisplacement,pos={105,30.00},size={60.00,14.00},proc=SetVarProc_opticalFlow
	SetVariable setvar_opticalFlowMinDisplacement,title="Min"
	SetVariable setvar_opticalFlowMinDisplacement,limits={0,dimsize(images,1)/4,0.1},value=_NUM:0
	
	
	
	duplicate /free /rmd=[][][0] yDisplacement rWave
	rwave=sqrt(xDisplacement[p][q][0]^2+yDisplacement[p][q][0]^2)
	variable maxD=wavemax(rWave)
	
	SetVariable setvar_opticalFlowMaxDisplacement,pos={180,30.00},size={60.00,14.00},proc=SetVarProc_opticalFlow
	SetVariable setvar_opticalFlowMaxDisplacement,title="Max"
	SetVariable setvar_opticalFlowMaxDisplacement,limits={0,dimsize(images,0.1),1},value=_NUM:maxD
	
	makearrowWave(xDisplacement, yDisplacement,1,images,scalefactor, minDisplacement=0, maxDisplacement=maxD)
	wave xWave
	wave yWave
	wave colorWave
	wave arrowWave
	
	appendtograph yWave vs xWave
	ModifyGraph mode=3
	ModifyGraph zColor(yWave)={colorWave,*,*,Rainbow}
	ModifyGraph arrowMarker(yWave)={arrowWave,1,10,0.5,0}
	string uD="xD:"+getwavesDataFolder(xDisplacement,2)+";yD:"+getwavesDataFolder(yDisplacement,2)
	ud+=";threshold:"+num2str(threshold) + ";"
	ud+="scale:"+num2str(scaleFactor)+";"
	setwindow kwTopWin userdata(OpticalFlow) = uD
	
	ColorScale/C/N=text0/F=0/A=RC/X=5.00/Y=0.00/E trace=yWave
	ColorScale/C/N=text0/B=1/A=RT/X=-1.89/Y=12.43/E=0 heightPct=20
	ColorScale/C/N=text0/X=0.00/Y=0.00
	ColorScale/C/N=text0/G=(48059,48059,48059)

	
end


function makeArrowWave(xDisplacement,yDisplacement,sliceIDX, imageWave,scaleFactor,[minDisplacement,maxDisplacement,arrowLength])
	wave xDisplacement,yDisplacement
	variable sliceIDX
	wave imageWave
	variable scaleFactor
	variable arrowLength, minDisplacement,maxDisplacement
	
	if(paramIsDefault(minDisplacement))
		minDisplacement=0
	endif
	if(paramIsDefault(arrowLength))
		arrowLength=10
	endif
	if(paramIsDefault(maxDisplacement))
		maxDisplacement=1
	endif
	
	if(sliceIDX == 0)
		wave xwave
		if(waveexists(xWave))
			xWave=nan	
		endif
		return 0
	endif
	
	duplicate /free /rmd=[][][sliceIDX-1] yDisplacement rWave, phiWave
	phiwave=atan2(xDisplacement[p][q][sliceIDX-1],yDisplacement[p][q][sliceIDX-1])
	rwave=sqrt(xDisplacement[p][q][sliceIDX-1]^2+yDisplacement[p][q][sliceIDX-1]^2)
	
	duplicate /free /rmd=[][][sliceIDX] imageWave, sliceWave
	imageinterpolate /pxsz={scaleFactor,scaleFactor} Pixelate sliceWave
	wave M_PixelatedImage
	duplicate /free M_PixelatedImage image_Resampled
	
	duplicate /free  rWave, rr
	imageinterpolate /pxsz={scaleFactor,scaleFactor} Pixelate rr
	
	duplicate /free M_PixelatedImage rScaled
	
	duplicate /free  phiwave, pp
	imageinterpolate /pxsz={scaleFactor,scaleFactor} Pixelate pp
	duplicate /free M_PixelatedImage phiScaled
	
	setscale /p x, dimOffset(rWave,0) * scaleFactor, dimdelta(rWave,0) * scaleFactor, waveunits(rWave,0), rScaled, phiScaled
	setscale /p y, dimOffset(rWave,1) * scaleFactor, dimdelta(rWave,1) * scaleFactor, waveunits(rWave,1), rScaled, phiScaled
	
	make/n=(dimsize(rScaled,0)*dimsize(rScaled,1))/o xWave,yWave, colorWave
	make/n=(dimsize(rScaled,0)*dimsize(rScaled,1),2)/o arrowWave
	
	killwaves/z M_PixelatedImage
	
	xWave=nan
	yWave=nan
	
	if(sliceIDX < 1 || sliceIDX > dimsize(xDisplacement,2))
		return 0
	endif
	
	variable i,j
	variable ctr=0

	for(i=0;i<dimsize(rScaled,0);i+=1)
		for(j=0;j<dimsize(rScaled,1);j+=1)

			xWave[ctr]=(dimoffset(rScaled,0)+i*dimdelta(rScaled,0)) 
			yWave[ctr]=(dimoffset(rScaled,1)+j*dimdelta(rScaled,1)) 
			arrowWave[ctr][1]=phiScaled[i][j]  - pi/2
			arrowWave[ctr][0]=rScaled[i][j] > minDisplacement ? arrowLength : nan
			colorWave[ctr]=rScaled[i][j]
			ctr+=1


		endfor
	endfor
	
end



Function SliderProc_opticalFlowOverlay(sa) : SliderControl
	STRUCT WMSliderAction &sa

	switch( sa.eventCode )
		case -3: // Control received keyboard focus
		case -2: // Control lost keyboard focus
		case -1: // Control being killed
			break
		default:
			if( sa.eventCode & 1 ) // value set
				Variable curval = sa.curval
				setvariable setvar_opticalFlowFrame value=_NUM:round(curval)
				updateOverlayView(sa.win,round(curval))

			endif
			break
	endswitch

	return 0
End


static function updateOverlayView(graphName, frame)
	string graphName
	variable frame

	string images=imageNameList(graphName,";")
	string image=stringfromList(0,images)
	wave imageWave=imagenameToWaveRef(graphName,image)

	ModifyImage/W=$graphName $image plane=frame
	string uD=getuserData(graphName, "", "OpticalFlow")
	wave xD=$stringbyKey("xD",uD)
	wave yD=$stringbyKey("yD",uD)
	
	controlInfo /W=$graphname setvar_opticalFlowResolution
	variable scale=v_value
	controlInfo /W=$graphname setvar_opticalFlowMinDisplacement
	variable minD=v_value
	controlInfo /W=$graphname setvar_opticalFlowMaxDisplacement
	variable maxD=v_value
	wave colorWave
	makearrowWave(xD, yD,frame,imageWave,scale,minDisplacement=minD, maxDisplacement=maxD)
	ModifyGraph zColor(yWave)={colorWave,minD,maxD,Rainbow,0}
end





function renderOverlayFrames(graphName)
	string graphName

	string images=imageNameList(graphName,";")
	string image=stringfromList(0,images)
	wave imageWave=imagenameToWaveRef(graphName,image)

	variable step=3
	variable i

	for(i=30;i<dimsize(imageWave,2);i+=step)
		updateoverlayView(graphName,i)
		doupdate/w=$graphname
		SavePICT/O/P=home/E=-5/B=288/WIN=$graphname as num2str(i)+".png"
	endfor

end

Function SetVarProc_opticalFlow(sva) : SetVariableControl
	STRUCT WMSetVariableAction &sva

	switch( sva.eventCode )
		case 1: // mouse up
		case 2: // Enter key
		case 3: // Live update
			strswitch(sva.ctrlName)
			case "setvar_opticalFlowFrame":
				Variable dval = sva.dval
				Slider slider_opticalFlow value=dval
				updateOverlayView(sva.win,dval)
				break
			default:
				controlInfo /W=$sva.win setvar_opticalFlowFrame
				updateOverlayView(sva.win,V_Value)
				break
			endswitch
			break
		case -1: // control being killed
			break
	endswitch

	return 0
End


function loadImages()

	variable refnum
	String fileFilters = "Image Files (*.png,*.tif,*.jpeg):.png,.tif,.jpeg;"
	fileFilters += "All Files:.*;"
	open /D /R /mult=1 refnum

	string outputPaths = S_fileName

	if (strlen(outputPaths) == 0)
		Print "Cancelled"
	else
		Variable numFilesSelected = ItemsInList(outputPaths, "\r")
		Variable i
		string loadedWaves=""
		for(i=0; i<numFilesSelected; i+=1)
			String path = StringFromList(i, outputPaths, "\r")
			imageload path
			wave loaded=$stringfromlist(0,S_waveNames)
			if(waveexists(loaded))
				Redimension/N=(-1,-1,3) loaded
				imagetransform rgb2gray loaded
				wave M_RGB2Gray
				killwaves loaded
				concatenate {M_RGB2Gray}, loadedImages
			endif
			
		endfor
		
		
	endif

end
