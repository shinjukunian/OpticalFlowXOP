#Optical Flow XOP
This code wraps [openCV](https://opencv.org) to provide dense optical flow calculations of image stacks in [Igor Pro](https://www.wavemetrics.com).

##How to Build
To build this project, first obtain the [XOP Toolkit](https://www.wavemetrics.com/products) from Wavemetrics. 

On macOS, we can statically link agains openCV. The easiest way to achieve this is to build openCV as an `.xcframework` using the script in `/platforms/apple` in the openCV repository.

On Windows, we have to dynamically link against the `opencv_world<version>.dll` included in the pre-built openCV binaries for Windows.


##Usage
The XOP provides Dense Inverse Search (DIS) optical flow and Farneback optical flow via the `OpticalFlowDIS` and `OpticalFlowFarneback` Igor Pro operations. Please see `Optical Flow.ihf` for details.

## Dependencies

* openCV 4.6
* Igor XOP Toolkit 8.0

## Licence 
MIT for the wrapper code.
openCV comes with its own licence. The XOP tookit is s commercial product.