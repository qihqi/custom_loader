import os
os.chdir('/data/home/qihan/cluster/custom_loader/third_party/opencv-python/tests')
import cv2

cap = cv2.VideoCapture("SampleVideo_1280x720_1mb.mp4")
assert cap.isOpened()
