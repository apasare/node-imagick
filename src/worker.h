#ifndef SRC_IMAGE_WORKER_H
#define SRC_IMAGE_WORKER_H

#include <string>
#include <nan.h>
#include <Magick++.h>

#include "image.h"

struct ExtentParams {
  std::string size;
  std::string color;
  Magick::GravityType gravity;
  ExtentParams(): gravity(Magick::GravityType::CenterGravity) {}
};

struct InputParams {
  bool fromBuffer;
  std::string file;
  InputParams(): fromBuffer(false) {}
};

struct OutputParams {
  bool toBuffer;
  Magick::Blob blob;
  std::string file;
  OutputParams(): toBuffer(false) {}
};

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

  InputParams *input = nullptr;
  OutputParams *output = nullptr;
  ExtentParams *extent = nullptr;

  std::string resize;
  std::string crop;

  bool autorotate = false;
  double rotate = 0;

  bool strip = false;
};

#endif // SRC_IMAGE_WORKER_H
