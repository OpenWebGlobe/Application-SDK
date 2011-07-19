Documentation:

LodeZlib (namespace)
========
unsigned decompress(std::vector<unsigned char>& out, const std::vector<unsigned char>& in, const LodeZlib_DecompressSettings& settings = LodeZlib_defaultDecompressSettings);
unsigned compress(std::vector<unsigned char>& out, const std::vector<unsigned char>& in, const LodeZlib_DeflateSettings& settings = LodeZlib_defaultDeflateSettings);


********************
EXAMPLE 1: Load PNG
********************

 const char* filename ="test.png";
  
  //load and decode
  std::vector<unsigned char> buffer, image;
  
  LodePNG::loadFile(buffer, filename); //load the image file with given filename
  LodePNG::Decoder decoder;
  decoder.decode(image, buffer.empty() ? 0 : &buffer[0], (unsigned)buffer.size()); //decode the png
  
  //get width, height and pixels
  unsigned int w = decoder.getWidth();
  unsigned int h = decoder.getHeight();
  unsigned int bpp = decoder.getBpp();
  
  *******************
  EXAMPE 2: Save PNG
  *******************
  
  LodePNG::Encoder encoder;
  encoder.addText("Comment", "Created by Huhuh");
  encoder.getSettings().zlibsettings.windowSize = 2048;
  
  //encode and save
  std::vector<unsigned char> buffer;
  encoder.encode(buffer, image.empty() ? 0 : &image[0], w, h);
  LodePNG::saveFile(buffer, "out.png");