# TSM-Face-Detector

# Implementation Info
All the versions of the TSM algorithm (except v1.3) are implemented using the default patches described in the main documentation file (folder /documentation).
The alternative patches are not included in any of the versions.

# Settings.xml Info
Change the settings xml file contents according to your needs.
Change the <outputType> xml object value to
-xx1 (binary) for export the results in XML file (decimal ex. 1,3,5,7)
-x1x (binary) for export the results in image file (decimal ex 2,3,6,7)
-1xx (binary) for export the results in visually in the screen (decimal ex 4,5,6,7)

# Build and Run
Export the containing files and folder in a directory of your choice.
Run the command shell inside the directory and execute the command (there is a makefile in the directory).
- make all

The output file that is created is named myFaceDetector.
The models and settings files are saved in "models/" folder.
There are some sample images in the "images/" folder.

Run the command
- ./myFaceDetector [image_path] [model_path] [settings_path]

