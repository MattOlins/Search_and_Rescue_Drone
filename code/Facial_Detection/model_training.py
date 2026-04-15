import cv2
import os
from imutils import paths
import face_recognition
import pickle

print("[DEBUG] Starting image processing...")
imagePaths= list(paths.list_images("dataset") )

#initialize known names and encodings
knownEncodings = []
knownNames = []

#loop for all img paths
for (i, imagePath) in enumerate(imagePaths): #this was stupid to make one plural
	#and the other one not...
	
	#pull name of person from folder
	print("[DEBUG] prcoessing image {}/{}".format(i+1, len(imagePaths)))
	name = imagePath.split(os.path.sep)[-2]
	
	#load input and covert it BRG (OpenCV ordering to dlib RGB)
		#check this line if machine has errors since I think that I might not need to convert it????
	image=cv2.imread(imagePath)
	rgb_img = cv2.cvtColor(image, cv2.COLOR_BGR2RGB) #again not sure if needed
	
	#finding x and y coordinates of the bounding facial 'box'
	boxes = face_recognition.face_locations(rgb_img, model="hog")
	
	#compute facial embedding of the face
	encodings = face_recognition.face_encodings(rgb_img, boxes)
	
	#loop for all encodings
	for encoding in encodings:
		#add each encoding + name to known names and encodings
		knownEncodings.append(encoding)
		knownNames.append(name)
		
#dump facial encoding and names to disk
print("[DEBUG] serializing encodings...")
data = {"encodings": knownEncodings, "names": knownNames}
f = open("encodings.pickle", "wb")
f.write(pickle.dumps(data))
f.close()
print("encodings saved to encodings.pickle")
