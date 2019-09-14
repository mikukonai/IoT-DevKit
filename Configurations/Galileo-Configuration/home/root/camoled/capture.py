import cv2
import numpy as np

cap = cv2.VideoCapture(0)
ret,frame = cap.read()
cv2.putText(frame, 'Mikukonai\'s Galileo - OpenCV Python Test', (100,100), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255,255,255),2)
cv2.imwrite("/home/root/haruhi2.png", frame, [int(cv2.IMWRITE_PNG_COMPRESSION), 0])

