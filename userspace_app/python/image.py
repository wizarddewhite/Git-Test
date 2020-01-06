import os
import sys
import subprocess
import shutil
import time
import math
from PIL import Image, ImageDraw, ImageEnhance
import random
import json
import re
# https://www.jianshu.com/p/2c9459059400
import pytesseract

def main():
    im = Image.open('./oppo_screen.png')
    print im.size

    # crop the top range
    # im1 = im.crop((200, 500, 800, 800))
    # im1 = im1.resize((1800, 300), Image.ANTIALIAS)
    im1 = im.resize((108, 192), Image.ANTIALIAS)
    im1 = im.convert('L')

    img = im1.convert('RGB')
    enhancer = ImageEnhance.Color(img)
    enhancer = enhancer.enhance(0)
    enhancer = ImageEnhance.Brightness(enhancer)
    enhancer = enhancer.enhance(2)
    enhancer = ImageEnhance.Contrast(enhancer)
    enhancer = enhancer.enhance(8)
    enhancer = ImageEnhance.Sharpness(enhancer)
    im1 = enhancer.enhance(20)

    im1.save('./oppo_top_screen.png', dpi=(300, 300))
    text = pytesseract.image_to_string((im1), lang='chi_sim')
    print (text)

    # crop the middle range
    im1 = im.crop((0, 800, 1080, 1400))
    #im1.save('./oppo_mid_screen.png')

if __name__ == '__main__':
    main()
