# pip install --upgrade scikit-image
# pip install --upgrade imutils

from skimage.measure import compare_ssim
import argparse
import imutils
import cv2
import os
import os.path
import numpy as np
import shutil

THIS_SCRIPT_DIR=os.path.dirname(os.path.abspath(__file__))

def compare_image(image_file1, image_file2) -> (float, np.array):
    imageA = cv2.imread(image_file1)
    imageB = cv2.imread(image_file2)
    grayA = cv2.cvtColor(imageA, cv2.COLOR_BGR2GRAY)
    grayB = cv2.cvtColor(imageB, cv2.COLOR_BGR2GRAY)

    # compute the Structural Similarity Index (SSIM) between the two
    # images, ensuring that the difference image is returned
    (score, diff) = compare_ssim(grayA, grayB, full=True)
    diff = (diff * 255).astype("uint8")
    return score, diff

def list_images(folder):
    r = []
    for root, dirs, files in os.walk(folder, topdown=False):
        for file_name in files:
            if file_name[-4:] == ".jpg":
                r.append(file_name)
    return r

CURRENT_IMAGES_FOLDER = THIS_SCRIPT_DIR + "/../samples_current/"
NEW_IMAGES_FOLDER = THIS_SCRIPT_DIR + "/../samples_new"
UPDATE_IMAGES_FOLDER = THIS_SCRIPT_DIR + "/../samples_updated"

def do_compare():
    current_images_files = list_images(CURRENT_IMAGES_FOLDER)
    new_images_files = list_images(NEW_IMAGES_FOLDER)
    if (os.path.isdir(UPDATE_IMAGES_FOLDER)):
        shutil.rmtree(UPDATE_IMAGES_FOLDER)
    os.mkdir(UPDATE_IMAGES_FOLDER)

    for image_file in new_images_files:
        new_image_fullpath = NEW_IMAGES_FOLDER + "/" + image_file
        shall_update = False
        if image_file in current_images_files:
            current_image_fullpath = CURRENT_IMAGES_FOLDER + "/" + image_file            
            score, diff = compare_image(current_image_fullpath, new_image_fullpath)
            print("{} : score {}".format(new_image_fullpath, score))
            if score < 0.7:
                shall_update = True
        else:
            print("{} : NEW".format(new_image_fullpath))
            shall_update = True

        if shall_update:
            shutil.copyfile(new_image_fullpath, UPDATE_IMAGES_FOLDER + "/" + image_file)

do_compare()
