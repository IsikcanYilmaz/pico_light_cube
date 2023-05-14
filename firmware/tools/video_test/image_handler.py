#!/usr/bin/env python3

import os, sys, argparse
from util import Bitmap
from PIL import Image # requires the pillow package

OLED_WIDTH = 128
OLED_HEIGHT = 64
size = (OLED_WIDTH, OLED_HEIGHT)

WHITE_VALUE_THRESHOLD = 120

def embed_to_c_file(bytes):
    hex_str = "{\n"
    for i in range(0, len(bytes)):
        hex_str += hex(bytes[i]) + ", "
        if (i % 16 == 0 and i != 0):
            hex_str += "\n"
    hex_str += "}"
    c_str = "char img[] = {hex_str} ;".format(hex_str = hex_str)
    return c_str

def create_bitmap_from_image(img):
    b = Bitmap(int(img.size[0]), int(img.size[1]))
    hsv_img = img.convert("HSV")
    pixels = hsv_img.load()
    for x in range(0, int(img.size[0])):
        for y in range(0, int(img.size[1])):
            b.set_pixel(x, y, (1 if pixels[x, y][2] > 0 else 0))
    return b

def create_image_from_bitmap(bitmap):
    img = Image.new(mode="HSV", size=(bitmap.w, bitmap.h))
    pixels = img.load()
    for x in range(0, img.size[0]):
        for y in range(0, img.size[1]):
            pixels[x, y] = (0, 0, (255 if bitmap.get_pixel(x, y) else 0))
    return img.convert("RGB")

def convert_image_to_bw(img):
    hsv_img = img.convert("HSV")
    new_img = Image.new(mode="HSV", size=img.size)
    pixels_orig = hsv_img.load()
    pixels_bw = new_img.load()
    for x in range(0, hsv_img.size[0]):
        for y in range(0, hsv_img.size[1]):
            if (pixels_orig[x,y][2] > WHITE_VALUE_THRESHOLD):
                pixels_bw[x,y] = (0, 0, 255)
    return new_img.convert("RGB")

# thank you mr gpt
def shrink_and_stretch_image(img):
    # with Image.open(filename) as img:
    # Get the original dimensions of the image
    width, height = img.size

    # Calculate the scaling factor to stretch the image to fill the area
    scale_factor = max(128/width, 64/height)

    # Resize the image using the calculated scale factor
    new_width = int(width * scale_factor)
    new_height = int(height * scale_factor)
    img_resized = img.resize((new_width, new_height), resample=Image.BICUBIC)

    # Create a new output image with the size of 128x64
    output_img = Image.new(mode='RGB', size=size, color=(255, 255, 255))

    # Calculate the coordinates to paste the resized image onto the output image
    x = int((128 - new_width) / 2)
    y = int((64 - new_height) / 2)

    # Paste the resized image onto the output image
    output_img.paste(img_resized, (x, y))

    # Save the output image to file
    # output_filename = filename.split('.')[0] + '_shrink_stretch.png'
    # output_img.save(output_filename)

    # return output_filename
    return output_img

def main():
    global WHITE_VALUE_THRESHOLD
    image_filename = sys.argv[1]
    if (len(sys.argv) > 2):
        WHITE_VALUE_THRESHOLD = int(sys.argv[2])
        print("Threshold:", WHITE_VALUE_THRESHOLD)

    main_img = Image.open(image_filename)
    shrunk_img = shrink_and_stretch_image(main_img)
    shrunk_bw_img = convert_image_to_bw(shrunk_img)
    shrunk_bw_img.save("shrunk_bw_"+image_filename)
    bm = create_bitmap_from_image(shrunk_bw_img)
    # bmimg = create_image_from_bitmap(bm)
    # bmimg.save("from_bitmap_" + image_filename)
    f = open("oled_" + image_filename + ".bin", "wb")
    f.write(bm.get_bytes())
    f.close()
    # c_file_str = embed_to_c_file(bm.get_bytes())
    # f = open("embedded_" + image_filename + ".c", "w")
    # f.write(c_file_str)
    # f.close()

if __name__ == "__main__":
    main()
