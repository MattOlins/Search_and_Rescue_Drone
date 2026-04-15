import face_recognition
import cv2
import numpy as np
from picamera2 import Picamera2
import time
import pickle
from gpiozero import OutputDevice

#libs for emailing stuff
import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email.mime.base import MIMEBase
from email import encoders



#load all known encodings
print("[INFO] loading encodings...")
with open("encodings.pickle", "rb") as f:
    data = pickle.loads(f.read())
known_face_encodings = data["encodings"]
known_face_names = data["names"]

#init cam
picam2 = Picamera2()
picam2.configure(picam2.create_preview_configuration(main={"format": 'XRGB8888', "size": (1920, 1080)}))
picam2.start()

#Chose GPIO Pin to be GPIO Pin 4 - outputs high when face detected
output = OutputDevice(4, active_high = True, initial_value = False)
email_sent = False

#global variables
cv_scaler = 4 # this has to be a whole number
face_locations = []
face_encodings = []
face_names = []
frame_count = 0
start_time = time.time()
fps = 0
frame_count = 0

#all names that trigger output to turn on
authorized_names = ["Matt", "Melissa", "Quinn"]  #HAS TO BE UPDATED

def send_email(name):
	#using html email style here 
	#this part is what will be changed around based off who is sending to whom
	msg = MIMEMultipart()
	sender_email = "ece414drone@gmail.com"
	recipient_email = "hughesq@lafayette.edu" #CHANGES EMAIL WHERE PHOTOS SENT TO
	app_password = "dfrl mmta uvma gzcz" 
	msg["From"] = sender_email
	msg["To"] = recipient_email
	msg["Subject"] = "Drone has found a person!!! MATT CAN GO TO SLEEP NOW!"
	html_body = f"<html>{name} was lost, but now found. IT WORKS BITCH. SUCK ME OFF NOW</html>"
	msg.attach(MIMEText(html_body, "html"))
	
	#throw in a jpg of what got captured
	with open("image.jpg", "rb") as f:
		part = MIMEBase("application", "octet-stream")
		part.set_payload(f.read())
		encoders.encode_base64(part)
		part.add_header("Content-Disposition", 'attachment; filename="image.jpg"')
		msg.attach(part)
		
	#try sending with app password - given me lots of errors
	try:
		server = smtplib.SMTP("smtp.gmail.com", 587)
		server.starttls()
		server.login(sender_email, app_password)
		server.sendmail(sender_email, recipient_email, msg.as_string())
		server.quit()
		print("Email sent successfully!")
	except Exception as e:
		print("Error sending email:", e)


def process_frame(frame):
    global face_locations, face_encodings, face_names, email_sent
    
    # Resize the frame using cv_scaler to increase performance (less pixels processed, less time spent)
    resized_frame = cv2.resize(frame, (0, 0), fx=(1/cv_scaler), fy=(1/cv_scaler))
    
    # Convert the image from BGR to RGB colour space, the facial recognition library uses RGB, OpenCV uses BGR
    rgb_resized_frame = cv2.cvtColor(resized_frame, cv2.COLOR_BGR2RGB)
    
    # Find all the faces and face encodings in the current frame of video
    face_locations = face_recognition.face_locations(rgb_resized_frame)
    face_encodings = face_recognition.face_encodings(rgb_resized_frame, face_locations, model='large')
    
    face_names = []
    authorized_face_detected = False 
    authorized_face_name = None
    
    for face_encoding in face_encodings:
        # See if the face is a match for the known face(s)
        matches = face_recognition.compare_faces(known_face_encodings, face_encoding)
        name = "Unknown"
        
        # Use the known face with the smallest distance to the new face
        face_distances = face_recognition.face_distance(known_face_encodings, face_encoding)
        best_match_index = np.argmin(face_distances)
        if matches[best_match_index]:
            name = known_face_names[best_match_index]
            # Check if the detected face is in our authorized list
            if name in authorized_names:
                authorized_face_detected = True
                authorized_face_name= name
        face_names.append(name)
    
    #control GPIO and email sending
    if authorized_face_detected:
        output.on()  # Turn on Pin
    else:
        output.off()  # Turn off Pin

    if authorized_face_detected and not email_sent:
        cv2.imwrite("image.jpg", frame)
        send_email(authorized_face_name)
        email_sent = True
	
    if not authorized_face_detected:
        email_sent = False
		
    return frame


def draw_results(frame):
    # Display the results
    for (top, right, bottom, left), name in zip(face_locations, face_names):
        #scale back up for output
        top *= cv_scaler
        right *= cv_scaler
        bottom *= cv_scaler
        left *= cv_scaler
        
        #Drawing a box/labeling
        cv2.rectangle(frame, (left, top), (right, bottom), (0, 0, 255), 3)
        cv2.rectangle(frame, (left -3, top - 35), (right+3, top), (244, 42, 3), cv2.FILLED)
        font = cv2.FONT_HERSHEY_DUPLEX
        cv2.putText(frame, name, (left + 6, top - 6), font, 1.0, (255, 255, 255), 1)
        
        #Checking authorization, prolly not very useful for final project but cool
        if name in authorized_names:
            cv2.putText(frame, "Authorized", (left + 6, bottom + 23), font, 0.6, (0, 255, 0), 1)
    
    return frame

def calculate_fps():
    global frame_count, start_time, fps
    frame_count += 1
    elapsed_time = time.time() - start_time
    if elapsed_time > 1:
        fps = frame_count / elapsed_time
        frame_count = 0
        start_time = time.time()
    return fps

while True:
    frame = picam2.capture_array()
    processed_frame = process_frame(frame)
    display_frame = draw_results(processed_frame)
    current_fps = calculate_fps()
    cv2.putText(display_frame, f"FPS: {current_fps:.1f}", (display_frame.shape[1] - 150, 30), 
                cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
    cv2.imshow('Video', display_frame)
    if cv2.waitKey(1) == ord("q"):
        break

cv2.destroyAllWindows()
picam2.stop()
output.off() 
