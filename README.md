Visual-Databases
================

This is code for image quality decider, which uses human aesthic preferences for photographs in 
deciding whether or not an image is of high or low quality. It was trained on a database of 1000 
photographs taken from a photo competition website (500 of the most highly and lowly rated images 
in the database). This was done using a Naive Bayesian Classifier, returning a 90% accuracy for 
low quality images and 80% accuracy for high quality images.

Metrics

High Level Features

Hue Count

The hue count of a photograph can be used as a simple measure for an image’s simplicity. For each 
image we convert it from RGB to HSV then create a 20-bin hue histogram, H, considering only pixels 
where the value is in the range [.15 to .95] and saturation is above .2. From this histogram we 
determine m the maximum value, m, in the histogram and N operating as

N = {i | H(i) > αm}.

From this we decide a value

fh = 20 - || N ||.

We set alpha to 0.05 to control the noise sensitivity of the metric.

Low Level Features
  
One the other hand, low-level features deal with information at the pixel level. These features 
capture averages for an image.

Contrast

To the measure the amount of contrast in an image we computed a 255-bin histogram for the red, 
green and blue channels of an image. We then create a combined grey level image, H, which takes 
the sum of the red, green and blue histograms for each bin. After normalizing the histogram contrast, 
fc, is calculated as being the amount of bins in the middle that take up 98% of the histogram mass.

Brightness

This is a simple measure of brightness. Since most cameras adjust a photo’s brightness to be 50% 
grey. However, professional photos will adjust the exposure between the subject and background of 
an image changing the brightness of the image.  For each image we calculate the average brightness 
for a pixel and return it as fb.

Aesthetic Features

These are features were described in the paper Lou et al. These metrics rely on the fact that 
people do not always rely the idea of identifying the subject of an image before analyzing the 
picture. This is done by deciding for each pixel what the likely kernel size, k*, was involved 
during a box filter convolution with an original unblurred image. From this a binary image, U, 
is created such that U(i, j) = 1 when k* = 1 and 0 otherwise)

From this image a bounding box containing 91% percent of the pixels equal to one is created. 
This bounding box will contain the subject of the image.

Clarity Contrast

In many professional photographs, to attract the attention of the viewer to the subject, 
professionals typically keep blur the background keeping the subject in focus. A 
measurement, fc, can be calculated to measure the amount clarity visible in the subject 
versus the image as a whole.

fc = (||MR||/||R||) / (||MI||/||I||),

where ||R|| and ||I|| are the areas of the subject region and total image respectively while  
||MR|| and ||MI|| size of Fourier transform where value is higher than certain threshold.

Lighting

Many professional photographers use different lighting on the subject and background leaving 
the brightness of the subject and background be significantly different. However most lay 
photographers allow the camera to adjust the brightness of the image, reducing the brightness 
difference between these two types of photos. To calculate this difference, fl, is formulated 
as

Fl = | log(Bs/Bb)},

where BS  and Bb are the average brightness of the image.

Simplicity

The simplicity of a picture is measured by taken by splitting the RGB channels into 16 values 
and creating a 163 histogram to count the quantized colors of the image background. From there
we get hmax as the max count of the histogram. This is done to calculate:
S = {i|H(i) ≥ γhmax}.

Using a γ = 0.01 in the experiment fs = (||S||/4096) x 100%.

Composition Geometry

This metric is based off of the Rule of Thirds for images. This rule states that if a photo is
divided into nine equally sized regions by two lines dividing both image horizontally and 
vertically into thirds, the center of the subject should be at one of the four intersections 
of these lines.  To formulate this criterion we calculate the minimum distance from the center 
of the subject to the each of these four intersection points. Using (CRx, CRy) and the center 
of the subject region and (Pix, Piy) where i = 1, 2, 3, or 4 as the four intersection points 
of the image the value of the closest (Pix, Piy) to the (CRx, CRy) is returned as the 
composition geometry value.

Color Harmony Feature

Harmonic colors are sets of colors that are aesthetically pleasing the to human visual 
perception. The color harmony of measured by first learning the of color combinations of a 
test-set. Using a test set of 3000 high and low quality photos each, we create average Hue, 
Saturation, and Value histograms. Using these histograms we create an image quality for color 
harmony, fh:

fh = Hues x Sats x Bris,

where Hues = Huehigh / Huelow, Sats = Sathigh / Satlow, Bris = Brihigh / Brilow.
Huehigh is the correlation between input hue histogram and the average hue histogram for high 
quality images, the same is done for each variable in the formula. 
