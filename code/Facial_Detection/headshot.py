import cv2
import os
from datetime import datetime
from picamera2 import Picamera2
import time

#Name of person you are taking photos of
PERSON_NAME = "Katie"
def create_folder(name):
	dataset_folder = "dataset"
	if not os.path.exists(dataset_folder):
		os.makedirs(dataset_folder)
		
	person_folder = os.path.join(dataset_folder, name)
	if not os.path.exists(person_folder):
		os.makedirs(person_folder)	
	return person_folder
	
def capture_photos(name):
	folder = create_folder(name)
	#photo formatting
	cam = Picamera2()
	cam.configure(cam.create_preview_configuration(main={"format": 'XRGB8888',"size": (640,480)}))
	cam.start()
	time.sleep(2)

	img_counter = 0

	while True:
		frame = cam.capture_array()      # capture frame
		#image = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)  # convert to OpenCV BGR
		cv2.imshow("Press SPACE to take photo | Press q to quit", frame)
		key = cv2.waitKey(1) & 0xFF
		if key == ord('q'): #if q is pressed quit
			break
		elif key == ord(' '): #if space is pressed take photo and save it
			img_name = os.path.join(folder, f"image_{img_counter}.jpg")
			cv2.imwrite(img_name, frame)
			print(f"{img_name} written!")
			img_counter += 1
				

	cv2.destroyAllWindows()
	cam.close()

capture_photos(PERSON_NAME)
