# Steganography

## Types Discussed 
1. Least Significant Bit Steganography. (LSB)
2. Jsteg Steganography.

### Least Significant Bit Steganography
- This uses the last 2 bits of every pixel from beginning of image and converts into desirable pixels from initially 
Booleanised meassage stream. 
- Since, we only use last 2 bits of a pixel, there will be atmost 3 pixel intensity change on each pixel value of the image.

#### Original Image.
<img width="601" alt="org_img" src="https://user-images.githubusercontent.com/40040134/80108953-8e796680-859a-11ea-8d00-003ba3017027.PNG">

#### LSB Steganographed Image.
<img width="599" alt="Stego Img" src="https://user-images.githubusercontent.com/40040134/80109219-dd270080-859a-11ea-86bc-7fef46da705b.PNG">

#### Least 2 significant bits of Original Image.
<img width="601" alt="org_img_lsb" src="https://user-images.githubusercontent.com/40040134/80109243-e57f3b80-859a-11ea-932e-87b8ac4649d4.PNG">

#### Least 2 significant bits of Stegannographed Image.
<img width="603" alt="Stego_img_lsb" src="https://user-images.githubusercontent.com/40040134/80109232-e0ba8780-859a-11ea-9808-1632a86a6547.PNG">

- We can clearly see the difference if we have the image of only least 2 significant bits of every pixel.
- So we move to Jsteg Steganography.

### Jsteg Steganography 
In this algorithm we use the similar method of Jpeg compression. 
1. Divide the image into 8 x 8 pixel blocks and convert.
2. Get the standard 64 DCT coefficients of each 8 x 8 blocks considering each as a unique image.
3. Apply LSB Steganography over these coefficients after down sampling the coefficients.
4. Apply Inverse DCT to get compressed and steganographed image.
