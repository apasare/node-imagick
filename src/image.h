#ifndef SRC_TEST_IMAGE_H_
#define SRC_TEST_IMAGE_H_

#include <nan.h>
#include <Magick++.h>

class Image : public Nan::ObjectWrap {
 public:
  static NAN_MODULE_INIT(Init);

  Magick::Image image;
  Magick::Blob blob;

 private:
  explicit Image();
  ~Image();

  static NAN_METHOD(New);
  static NAN_METHOD(Copy);
  static NAN_METHOD(Process);
  static NAN_METHOD(Size);
  static inline Nan::Persistent<v8::Function> & constructor();
};

#endif // SRC_TEST_IMAGE_H_
