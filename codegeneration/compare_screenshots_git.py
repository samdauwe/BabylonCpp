"""
This script should be run after having generated new screenshot images
with `BabylonStudio -a`
It will proocess all images inside assets/screenshots/ScreenshotsData/
and compare them with the previous git version.

Only the images with significant differences will be kept with their new version.
The other ones will be replaced by the previous version.

This helps creating commits that contain only the relevant screenshot modifications.

How to run it:

- First make sure to use python >= 3.6
- Install the required modules:
> pip3 install --upgrade opencv-python scikit-image imutils
- Run it:
From the main repo folder, run

> python3 codegeneration/compare_screenshots_git.py

"""

from skimage.metrics import structural_similarity
import argparse
import imutils
import cv2
import os
import os.path
import numpy as np
import shutil
import subprocess

THIS_SCRIPT_DIR=os.path.dirname(os.path.abspath(__file__))
SCREENSHOTS_FOLDER = THIS_SCRIPT_DIR + "/../assets/screenshots/ScreenshotsData/"

def compare_image(image_file1, image_file2) -> (float, np.array):
    imageA = cv2.imread(image_file1)
    imageB = cv2.imread(image_file2)
    grayA = cv2.cvtColor(imageA, cv2.COLOR_BGR2GRAY)
    grayB = cv2.cvtColor(imageB, cv2.COLOR_BGR2GRAY)
    grayB = cv2.resize(grayB, (grayA.shape[1], grayA.shape[0]) )

    # compute the Structural Similarity Index (SSIM) between the two
    # images, ensuring that the difference image is returned
    (score, diff) = structural_similarity(grayA, grayB, full=True)
    diff = (diff * 255).astype("uint8")
    return score, diff


def list_images(folder):
    r = []
    for root, dirs, files in os.walk(folder, topdown=False):
        for file_name in files:
            if file_name[-4:] == ".jpg":
                r.append(file_name)
    return r


def do_compare():
    images_files = list_images(SCREENSHOTS_FOLDER)

    for image_file in images_files:
        # print("processing {}".format(image_file))

        image_filename = SCREENSHOTS_FOLDER + "/" + image_file
        image_temp_filename = SCREENSHOTS_FOLDER + "/" + image_file + ".new.jpg"

        os.rename(image_filename, image_temp_filename)
        try:
            subprocess.check_call(["git", "checkout", SCREENSHOTS_FOLDER + "/" + image_file])
        except subprocess.CalledProcessError:
            os.rename(image_temp_filename, image_filename)
            continue

        similarity_score, diff = compare_image(image_filename, image_temp_filename)
        # print("{} : similarity_score {}".format(image_file, similarity_score))
        if similarity_score < 0.65:
            print(f"{image_file} has new version, similarity_score={similarity_score}")
            os.remove(image_filename)
            os.rename(image_temp_filename, image_filename)
        else:
            # print(f"{image_file} is similar")
            os.remove(image_temp_filename)


do_compare()
