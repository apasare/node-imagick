#include <string>

#include "image.h"
#include "utils.h"
#include "worker.h"

NAN_MODULE_INIT(Image::Init) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);

  tpl->SetClassName(Nan::New("Image").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "process", Process);
  Nan::SetPrototypeMethod(tpl, "copy", Copy);
  Nan::SetPrototypeMethod(tpl, "size", Size);

  constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());

  Nan::Set(target, Nan::New("Image").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());
}

Image::Image() {}
Image::Image(const std::string &imageSpec) {
  Magick::Image tmpImage(imageSpec);
  image = tmpImage;
}
Image::Image(const std::string &size, const std::string &color) {
  Magick::Geometry tmpSize(size);
  Magick::Image tmpImage(tmpSize, color);
  image = tmpImage;
}
Image::~Image() {}

Nan::Persistent<v8::Function> & Image::constructor() {
  static Nan::Persistent<v8::Function> my_constructor;
  return my_constructor;
}

NAN_METHOD(Image::New) {
  if (info.IsConstructCall()) {
    Image *obj;

    if (!info[0]->IsUndefined() && !info[1]->IsUndefined()) {
      obj = new Image(ToString(info[0]), ToString(info[1]));
    } else if (!info[0]->IsUndefined()) {
      obj = new Image(ToString(info[0]));
    } else {
      obj = new Image();
    }

    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    return Nan::ThrowError("'Image' is a class not a function. Use 'new' operator to instantiate it.");
  }
}

NAN_METHOD(Image::Copy) {
  v8::Local<v8::Function> cons = Nan::New(constructor());
  Nan::MaybeLocal<v8::Object> maybeInstance = Nan::NewInstance(cons, 0, NULL);
  v8::Local<v8::Object> instance = maybeInstance.ToLocalChecked();

  Image *image = Nan::ObjectWrap::Unwrap<Image>(info.Holder());
  Image *newImage = Nan::ObjectWrap::Unwrap<Image>(instance);
  newImage->image = image->image;

  info.GetReturnValue().Set(instance);
}

NAN_METHOD(Image::Size) {
  Image *image = Nan::ObjectWrap::Unwrap<Image>(info.Holder());
  Magick::Geometry size;

  size_t width = Nan::To<uint32_t>(info[0]).FromJust();
  size_t height = Nan::To<uint32_t>(info[1]).FromJust();
  if (width > 0 && height > 0) {
    size.width(width);
    size.height(height);
    image->image.size(size);
  } else {
    size = image->image.size();
  }

  v8::Local<v8::Object> response = Nan::New<v8::Array>();
  Nan::Set(
    response, 
    Nan::New("width").ToLocalChecked(),
    Nan::New<v8::Uint32>((uint32_t) size.width())
  );
  Nan::Set(
    response,
    Nan::New("height").ToLocalChecked(),
    Nan::New<v8::Uint32>((uint32_t) size.height())
  );

  info.GetReturnValue().Set(response);
}

NAN_METHOD(Image::Process) {
  Nan::MaybeLocal<v8::Object> maybeOptions = Nan::To<v8::Object>(info[0]);
  if (maybeOptions.IsEmpty()) {
    return Nan::ThrowError("'options' parameter is not provided");
  }
  Nan::MaybeLocal<v8::Function> maybeCallback = Nan::To<v8::Function>(info[1]);
  if (maybeCallback.IsEmpty()) {
    return Nan::ThrowError("'callback' parameter is not provided");
  }

  Image *image = Nan::ObjectWrap::Unwrap<Image>(info.Holder());
  v8::Local<v8::Object> options = maybeOptions.ToLocalChecked();

  Nan::Callback *callback = new Nan::Callback(maybeCallback.ToLocalChecked());
  Nan::AsyncQueueWorker(new ImageWorker(callback, image, options));

  info.GetReturnValue().Set(info.This());
}
