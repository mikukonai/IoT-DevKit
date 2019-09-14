import cv2  
import numpy as np  
  
img = cv2.imread("/home/root/haruhi.jpg")  
emptyImage = np.zeros(img.shape, np.uint8)  
  
emptyImage2 = img.copy()  
  
emptyImage3=cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)  

cv2.imwrite("/home/root/haruhi2.jpg", img,[int(cv2.IMWRITE_JPEG_QUALITY), 5])  
cv2.imwrite("/home/root/haruhi3.jpg", img,[int(cv2.IMWRITE_JPEG_QUALITY), 100])  
cv2.imwrite("/home/root/haruhi2.png", img,[int(cv2.IMWRITE_PNG_COMPRESSION), 0])  
cv2.imwrite("/home/root/haruhi3.png", img,[int(cv2.IMWRITE_PNG_COMPRESSION), 9])  

