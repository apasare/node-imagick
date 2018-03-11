#ifndef SRC_IMAGE_WORKER_H
#define SRC_IMAGE_WORKER_H

#include <string>
#include <nan.h>
#include <Magick++.h>

#include "image.h"

class ImageWorker : public Nan::AsyncWorker {
 public:
  ImageWorker(
    Nan::Callback *callback,
    Image *image,
    v8::Local<v8::Object> &task
  );
  ~ImageWorker();

  void Execute();
  void HandleOKCallback();

 private:
  Image *image;
  bool outputToBuffer = false;
  Magick::Blob outputBlob;
  std::string inputFile;
  std::string outputFile;
  std::string resize;
  std::string crop;
  bool autorotate = false;
  double rotate = 0;
  bool strip = false;
};

#endif
